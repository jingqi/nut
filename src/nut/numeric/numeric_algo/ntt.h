/**
 * NTT 中可用素数模数原根表:
 * - 模数是满足 m = r * 2 ** k + 1 的素数
 * - g 为 mod m 的原根, g ** (m - 1) = 1 (mod m)
 * - 只能处理 2**k 规模的多项式
 *
 *              m(素数)  m位长     r   k   g
 * ------------------------------------------
 *                    3      2     1   1   2
 *                    5      3     1   2   2
 *                   17      5     1   4   3
 *                   97      7     3   5   5
 *                  193      8     3   6   5
 *                  257      9     1   8   3
 *                 7681     13    15   9  17
 *                12289     14     3  12  11
 *                40961     16     5  13   3
 *                65537     17     1  16   3
 *               786433     20     3  18  10
 *              5767169     23    11  19   3
 *              7340033     23     7  20   3
 *             23068673     25    11  21   3
 *            104857601     27    25  22   3
 *            167772161     28     5  25   3
 *            469762049     29     7  26   3
 *           1004535809     30   479  21   3
 *           1325137921     31  5055  18  11
 *           2013265921     31    15  27  31
 *           2281701377     32    17  27   3
 *           3221225473     32     3  30   5
 *          75161927681     37    35  31   3
 *          77309411329     37     9  33   7
 *         206158430209     38     3  36  22
 *        2061584302081     41    15  37   7
 *        2748779069441     42     5  39   3
 *        6597069766657     43     3  41   5
 *       39582418599937     46     9  42   5
 *       79164837199873     47     9  43   5
 *      263882790666241     48    15  44   7
 *     1231453023109121     51    35  45   3
 *     1337006139375617     51    19  46   3
 *     3799912185593857     52    27  47   5
 *     4222124650659841     52    15  48  19
 *     7881299347898369     53     7  50   6
 *    31525197391593473     55     7  52   3
 *   180143985094819841     58     5  55   6
 *  1945555039024054273     61    27  56   5
 *  4179340454199820289     62    29  57   3
 *
 * 参见 http://blog.miskcoo.com/2014/07/fft-prime-table
 */

#ifndef ___HEADFILE_6CF1357C_7619_4420_A0A6_97F1AF68C994_
#define ___HEADFILE_6CF1357C_7619_4420_A0A6_97F1AF68C994_

#include <stdint.h>
#include <stddef.h> // for size_t
#include <algorithm> // for std::max()

#include "../../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <malloc.h> // for ::alloca()
#else
#   include <alloca.h>
#endif

#include "../../nut_config.h"
#include "../word_array_integer/word_array_integer.h"
#include "../word_array_integer/bit_op.h"


// 模数 M 的相关信息 m = r * 2 ** k + 1
#define NUT_NTT_R 29
#define NUT_NTT_K 57
#define NUT_NTT_G 3
#define NUT_NTT_M (NUT_NTT_R * (1ULL << NUT_NTT_K) + 1)

// - 大整数拆成多项式, = a0 + a1*base + a2*pow(base,2) + a3*pow(base, 3) + ...
// - NUT_NTT_BASE_BITS 越小, 能够处理越大的大整数, 但是多项式越长, 消耗的时间越长
#define NUT_NTT_BASE_BYTES 3
#define NUT_NTT_BASE_BITS  (8 * NUT_NTT_BASE_BYTES)
#define NUT_NTT_BASE       (1ULL << NUT_NTT_BASE_BITS)
#define NUT_NTT_BASE_MAX   (NUT_NTT_BASE - 1)

static_assert(NUT_NTT_BASE_MAX * NUT_NTT_BASE_MAX + 1 <= NUT_NTT_M,
              "NTT base is too big or modulo is too small");

namespace nut
{

typedef uint64_t ntt_word_type;

static_assert(std::is_unsigned<ntt_word_type>::value,
              "Unexpected integer type");
static_assert(NUT_NTT_M <= ((~(ntt_word_type)0) >> 1), // 避免加减法操作溢出
              "NTT word type is too short or modulo is too big");

/**
 * 能否使用 NTT 乘法
 *
 * @param bits 乘数最大 bit 数
 */
NUT_API bool can_use_ntt_multiply(size_t bits) noexcept;

/**
 * 将 2**(8*sizeof(T)) 进制输入, 划分成 NUT_NTT_BASE 进制输入, 且多项式长度为 2
 * 的幂次
 */
template <typename T>
size_t _split_base_bits(const T *a, size_t M, const T *b, size_t N,
                        ntt_word_type **aa, ntt_word_type **bb, ntt_word_type **rs) noexcept
{
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 &&
           nullptr != aa && nullptr != bb && nullptr != rs);

    const size_t word_siglen = (sizeof(T) * std::max(M, N) + NUT_NTT_BASE_BYTES - 1) / NUT_NTT_BASE_BYTES;
    unsigned wc_bits = highest_bit1((uint64_t) word_siglen); // word_count 必须是 2 的幂次
    if ((1ULL << wc_bits) < word_siglen)
        ++wc_bits;
    assert((1ULL << wc_bits) >= word_siglen);

    const size_t word_count = (1ULL << (wc_bits + 2)); // 至少是 4 倍大小多项式
    *aa = (ntt_word_type*) ::malloc(sizeof(ntt_word_type) * word_count * 3);
    *bb = *aa + word_count;
    *rs = *bb + word_count;
    ::memset(*aa, 0, sizeof(ntt_word_type) * word_count * 2);

    for (size_t i = 0; i < sizeof(T) * std::max(M, N); ++i)
    {
        const size_t out_byte_index = i / NUT_NTT_BASE_BYTES * sizeof(ntt_word_type) +
            i % NUT_NTT_BASE_BYTES;
        const uint8_t byte_a = (i < sizeof(T) * M ? get_byte_le(a, i) : 0),
            byte_b = (i < sizeof(T) * N ? get_byte_le(b, i) : 0);
            set_byte_le(*aa, out_byte_index, byte_a);
            set_byte_le(*bb, out_byte_index, byte_b);
    }

    return wc_bits;
}

template <typename T>
void _merge_base_bits(const ntt_word_type *src, size_t M, T *dst, size_t P) noexcept
{
    assert(nullptr != src && M > 0 && nullptr != dst && P > 0);
    for (size_t i = 0; i < sizeof(T) * P; ++i)
    {
        const size_t in_byte_index = i / NUT_NTT_BASE_BYTES * sizeof(ntt_word_type) +
            i % NUT_NTT_BASE_BYTES;
        const uint8_t byte = (in_byte_index < sizeof(ntt_word_type) * M ?
                              get_byte_le(src, in_byte_index) : 0);
        set_byte_le(dst, i, byte);
    }
}

/**
 * @param a 长度为 2**(bit_len+1)
 * @param b 长度为 2**(bit_len+1)
 * @param rs 长度为 2**(bit_len+1)
 */
NUT_API void ntt_convolution(ntt_word_type *a, ntt_word_type *b, unsigned bit_len,
                             ntt_word_type *rs) noexcept;

/**
 * 快速数论变换(NTT) 乘法, 时间复杂度为 O(n(log2 n))
 */
template <typename T>
void unsigned_ntt_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);

    assert(can_use_ntt_multiply(8 * sizeof(T) * std::max(M, N)));

    ntt_word_type *aa, *bb, *rs;
    const size_t wc_bits = _split_base_bits(a, M, b, N, &aa, &bb, &rs);
    ntt_convolution(aa, bb, wc_bits + 1, rs); // 2**(k+1) points NTT

    const size_t high = (1ULL << (wc_bits + 1)) - 1;
    for (size_t i = 0; i < high; ++i)
    {
        rs[i + 1] += rs[i] >> NUT_NTT_BASE_BITS;
        rs[i] &= ~(~(ntt_word_type)0 << NUT_NTT_BASE_BITS);
    }
    _merge_base_bits(rs, high + 1, x, P);
    ::free(aa); // 'bb'、'rs' are in same memory block
}

template <typename T>
void signed_ntt_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P) noexcept
{
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);

    // NTT 算法不能处理负数的补数形式
    const bool a_neg = is_negative(a, M), b_neg = is_negative(b, N);
    T *aa = nullptr, *bb = nullptr;
    size_t MM = M, NN = N;
    if (a_neg && b_neg)
    {
        ++MM;
        ++NN;
        aa = (T*) ::alloca(sizeof(T) * (MM + NN));
        bb = aa + MM;
        signed_negate(a, M, aa, MM);
        signed_negate(b, N, bb, NN);
    }
    else if (a_neg)
    {
        ++MM;
        aa = (T*) ::alloca(sizeof(T) * MM);
        signed_negate(a, M, aa, MM);
    }
    else if (b_neg)
    {
        ++NN;
        bb = (T*) ::alloca(sizeof(T) * NN);
        signed_negate(b, N, bb, NN);
    }

    // 调用 NTT 算法
    unsigned_ntt_multiply((nullptr != aa ? aa : a), MM,
                          (nullptr != bb ? bb : b), NN, x, P);

    // 还原符号
    if (a_neg != b_neg)
        signed_negate(x, P, x, P);
}

}

#endif
