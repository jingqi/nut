/**
 * NTT 代码参考:
 *   https://www.zhihu.com/question/19678341
 *
 * 计算过程中需要注意 unsigned / signed 相互取模问题:
 * - long long a = -1, b = a % 6LL; 将得到正确结果 b == -1
 * - long long a = -1, b = a % 6ULL; 将隐式转换为 unsigned long long 之后再做模
 *   操作, 负数 a 被错误转成无符号正数, b == 3
 */

#include <mutex> // for std::call_once()

#include "../../platform/endian.h"
#include "../../platform/int_type.h"
#include "gcd.h"
#include "mod.h"
#include "ntt.h"
#include "prime.h"


namespace nut
{

NUT_API bool can_use_ntt_multiply(size_t bits)
{
    const size_t word_siglen = (bits + NUT_NTT_BASE_BITS - 1) / NUT_NTT_BASE_BITS;
    unsigned word_count_bits = highest_bit1((uint64_t) word_siglen) + 1; // word_count 必须是 2 的幂次
    if ((1ULL << word_count_bits) < word_siglen)
        ++word_count_bits;
    assert((1ULL << word_count_bits) >= word_siglen);
    ++word_count_bits; // 2**(k+1) points NTT

    // 至多容纳 2 ^ K 个字(word)作 NTT 变换
    if (word_count_bits >= NUT_NTT_K)
        return false;

    // When convolving two vectors of length n where each input value is at most
    // m, the upper bound on each output value is (m**2)*n. Choosing a minimum
    // working modulus of M=(m**2)*n+1 is sufficient to always avoid overflow in
    // the worst case.
    // See https://www.nayuki.io/page/number-theoretic-transform-integer-dft
    if (NUT_NTT_BASE_MAX * NUT_NTT_BASE_MAX * word_siglen + 1 > NUT_NTT_M)
        return false;

    return true;
}

 // omega[k]、inv_omega[K] 是模 M 的 2^k 次单位根，其中 omega[] 用于正变换,
// inv_omega[] 用于逆变换
static ntt_word_type omega[NUT_NTT_K] = {0};
static ntt_word_type inv_omega[NUT_NTT_K] = {0};
static std::once_flag amega_init_flag;

static void init_omega()
{
    std::call_once(
        amega_init_flag,
        [&] {
            // => w[k-1] = pow_mod(g, 2 * r, m)
            omega[NUT_NTT_K - 1] = pow_mod<ntt_word_type>(NUT_NTT_G, 2 * NUT_NTT_R, NUT_NTT_M);

            // => inv_w[k-1] = w[k-1] ** -1 (mod m)
            inv_omega[NUT_NTT_K - 1] = inverse_of_prime_mod<ntt_word_type>(omega[NUT_NTT_K - 1], NUT_NTT_M);

            assert(mult_mod<ntt_word_type>(omega[NUT_NTT_K - 1], inv_omega[NUT_NTT_K - 1], NUT_NTT_M) == 1);

            for (int i = NUT_NTT_K - 2; i >= 0; --i)
            {
                omega[i] = mult_mod<ntt_word_type>(omega[i + 1], omega[i + 1], NUT_NTT_M);
                inv_omega[i] = mult_mod<ntt_word_type>(inv_omega[i + 1], inv_omega[i + 1], NUT_NTT_M);
            }
            assert(1 == omega[0] && 1 == inv_omega[0]);
        });
}

// DIF-FFT: 输入为自然顺序，输出为二进制倒序
static void NTT_N2R(ntt_word_type *a, unsigned bits)
{
    const size_t len = 1ULL << bits;

    // butterfly operation
    for (int layer = bits; layer > 0; --layer)
    {
        const size_t group = 1ULL << layer, brother = group >> 1;
        assert(layer < NUT_NTT_K);
        const ntt_word_type kernel = omega[layer];

        for (size_t k = 0; k < len; k += group)
        {
            // 遍历每一组
            ntt_word_type w = 1;

            for (size_t j = 0; j < brother; ++j)
            {
                // 对每一个元素，找到它的兄弟（和它进行蝴蝶操作的那个元素）
                const size_t cur = k + j, next = cur + brother;
                const ntt_word_type u = a[cur], v = a[next];

                // => a[cur] = (u + v) % M;
                a[cur] = u + v;
                if (a[cur] >= NUT_NTT_M)
                    a[cur] -= NUT_NTT_M;

                // => a[next] = (u + M - v) * w % M;
                if (u >= v)
                {
                    a[next] = mult_mod<ntt_word_type>(u - v, w, NUT_NTT_M);
                }
                else
                {
                    a[next] = mult_mod<ntt_word_type>(NUT_NTT_M - v + u, w, NUT_NTT_M);
                }

                // => w = w * kernel % M;
                w = mult_mod<ntt_word_type>(w, kernel, NUT_NTT_M);
            }
        }
    }
}

// DIT-FFT: 输入为二进制倒序，输出为自然顺序的FFT
static void NTT_R2N(ntt_word_type* a, unsigned bits)
{
    const size_t len = 1 << bits;

    // butterfly operation
    for (unsigned layer = 1; layer <= bits; ++layer)
    {
        const size_t group = 1 << layer, brother = group >> 1;
        assert(layer < NUT_NTT_K);
        const ntt_word_type kernel = inv_omega[layer];

        for (size_t k = 0; k < len; k += group)
        {
            // 遍历每一组
            ntt_word_type w = 1;

            for (size_t j = 0; j < brother; ++j)
            {
                // 对每一个元素，找到它的兄弟（和它进行蝴蝶操作的那个元素）
                size_t cur = k + j, next = cur + brother;
                const ntt_word_type u = a[cur],
                    t = mult_mod<ntt_word_type>(w, a[next], NUT_NTT_M);

                // => a[cur] = (u + t) % M_M;
                a[cur] = u + t;
                if (a[cur] >= NUT_NTT_M)
                    a[cur] -= NUT_NTT_M;

                // => a[next] = (u + M_M - t) % M_M;
                if (u >= t)
                    a[next] = u - t;
                else
                    a[next] = NUT_NTT_M - t + u;

                // => w = w * kernel % M_M;
                w = mult_mod<ntt_word_type>(w, kernel, NUT_NTT_M);
            }
        }
    }

    // 除以 len 操作变成乘以逆元
    const uint64_t inv_len = inverse_of_prime_mod<ntt_word_type>(len, NUT_NTT_M);
    for (size_t i = 0; i < len; ++i)
        a[i] = mult_mod<ntt_word_type>(a[i], inv_len, NUT_NTT_M);
}

NUT_API void ntt_convolution(ntt_word_type *a, ntt_word_type *b, unsigned bit_len,
                             ntt_word_type *rs)
{
    assert(nullptr != a && nullptr != b && bit_len > 0 && nullptr != rs);

    init_omega();

    NTT_N2R(a, bit_len);
    NTT_N2R(b, bit_len);

    const size_t len = 1 << bit_len;
    for (size_t i = 0; i < len; ++i)
        rs[i] = mult_mod<ntt_word_type>(a[i], b[i], NUT_NTT_M);

    NTT_R2N(rs, bit_len);
}

}
