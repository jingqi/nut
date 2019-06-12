
#ifndef ___HEADFILE_2B908A00_62CA_4D3C_ADB3_8CB4BAB97554_
#define ___HEADFILE_2B908A00_62CA_4D3C_ADB3_8CB4BAB97554_

#include <algorithm> // for std::min()

#include "../../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <malloc.h> // for ::alloca()
#else
#   include <alloca.h>
#endif

#include "../word_array_integer/word_array_integer.h"
#include "ntt.h"


/**
 * 规模较小时 karatsub 退化使用一般算法
 *
 * NOTE 这个边界值是一个经验值, 并且与系统性能相关。在 VC 上, 如果设置的过小, 性
 *      能甚至会急剧下降!
 */
#if NUT_PLATFORM_CC_VC
#   define NUT_KARATSUBA_FALLBACK_THRESHOLD 256
#else
#   define NUT_KARATSUBA_FALLBACK_THRESHOLD 128
#endif

namespace nut
{

/**
 * karatsuba 乘法，时间复杂度为 O(n^(log2 3)) = O(n^1.58)
 *
 * a = A * base + B (其中 base = 2 ^ (n/2) )
 * b = C * base + D
 * a * b = AC * (base^2) + ((A+B)(C+D) - AC - BD) * base + BD
 * 该算式中乘法总体规模变小了
 */
template <typename T>
void unsigned_karatsuba_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P) noexcept
{
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);

    // 去除无效位长
    M = std::min(unsigned_significant_size(a, M), P);
    N = std::min(unsigned_significant_size(b, N), P);

    // 退化
    if (M < NUT_KARATSUBA_FALLBACK_THRESHOLD || N < NUT_KARATSUBA_FALLBACK_THRESHOLD)
    {
        unsigned_multiply(a, M, b, N, x, P);
        // unsigned_ntt_multiply(a, M, b, N, x, P);
        return;
    }

    // 准备变量 A、B、C、D
    const size_t base_len = (std::max(M, N) + 1) / 2;
    const T ZERO = 0;

    // a_len = max(0, min(M - base_len, P - base_len))
    //       = max(0, M - base_len)                    [since, M <= P]
    const T *A = a + base_len;
    size_t a_len = M - base_len;
    if (M <= base_len)
    {
        A = &ZERO;
        a_len = 1;
    }

    // b_len = min(base_len, M)
    const T *B = a;
    const size_t b_len = std::min(base_len, M);

    // c_len = max(0, min(N - base_len, P - base_len))
    //       = max(0, N - base_len)                    [since, N <= P]
    const T *C = b + base_len;
    size_t c_len = N - base_len;
    if (N <= base_len)
    {
        C = &ZERO;
        c_len = 1;
    }

    // d_len = min(base_len, N)
    const T *D = b;
    const size_t d_len = std::min(base_len, N);

    // 计算中间结果
    //
    // AB = A + B
    // ab_len = min(max(a_len, b_len) + 1, P - base_len)
    //        = min(b_len + 1, P - base_len)           [since, a_len <= b_len]
    //
    // CD = C + D
    // cd_len = min(max(c_len, d_len) + 1, P - base_len)
    //        = min(d_len + 1, P - base_len)           [since, c_len <= d_len]
    //
    // ABCD = (A + B) * (C + D)
    // abcd_len = min(ab_len + cd_len, P - base_len)
    //          = min(b_len + d_len + 1, P - base_len)
    //
    // AC = A * C
    // ac_len = min(a_len + c_len, max(0, P - base_len))
    //        = min(a_len + c_len, P - base_len)       [since, P - base_len >= max(M,N) - base_len > 0]
    //        < ab_len + cd_len
    //
    // BD = B * D
    // bd_len = min(b_len + d_len, P)
    //
    const size_t ab_len = std::min(b_len + 1, P - base_len);
    const size_t cd_len = std::min(d_len + 1, P - base_len);
    const size_t abcd_len = std::min(b_len + d_len + 1, P - base_len);
    const size_t ac_len = std::min(a_len + c_len, P - base_len);
    assert(ac_len <= ab_len + cd_len);
    const size_t bd_len = std::min(b_len + d_len, P);
    T *const AB = (T*) ::alloca(sizeof(T) * (ab_len + cd_len + abcd_len));
    T *const CD = AB + ab_len;
    T *const ABCD = CD + cd_len;
    T *const AC = AB; // 复用 AB + CD 的存储空间
    T *const BD = x; // 复用 x 的存储空间

    // AB = A + B
    unsigned_add(A, a_len, B, b_len, AB, ab_len);

    // CD = C + D
    unsigned_add(C, c_len, D, d_len, CD, cd_len);

    // ABCD = AB * CD
    unsigned_karatsuba_multiply(AB, ab_len, CD, cd_len, ABCD, abcd_len);

    // AC = A * C
    // NOTE 因为 AC 复用了 AB CD 的存储空间, 需要在 ABCD 之后计算
    unsigned_karatsuba_multiply(A, a_len, C, c_len, AC, ac_len);

    // BD = B * D
    // NOTE 因为 BD 复用了 x 的存储空间, 为了避免传入的参数 a, b 被破坏(a、b、x
    //      可能有交叉区域), BD 要放在 AB、CD、AC 后面计算
    unsigned_karatsuba_multiply(B, b_len, D, d_len, BD, bd_len);

    // ABCD -= AC + BD
    unsigned_sub(ABCD, abcd_len, AC, ac_len, ABCD, abcd_len);
    unsigned_sub(ABCD, abcd_len, BD, bd_len, ABCD, abcd_len);

    // 生成最终结果
    if (bd_len < P)
        ::memset(x + bd_len, 0, sizeof(T) * (P - bd_len));
    unsigned_add(x + base_len, P - base_len, ABCD, abcd_len, x + base_len, P - base_len);
    if (P > base_len * 2)
        unsigned_add(x + base_len * 2, P - base_len * 2, AC, ac_len, x + base_len * 2, P - base_len * 2);
}

/**
 * karatsuba 乘法，时间复杂度为 O(n**(log2 3)) = O(n**1.58)
 *
 * a = A * base + B (其中 base = 2 ^ (n/2) )
 * b = C * base + D
 * a * b = AC * (base^2) + ((A+B)(C+D) - AC - BD) * base + BD
 * 该算式中乘法总体规模变小了
 */
template <typename T>
void signed_karatsuba_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P) noexcept
{
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);

    // 退化
    if (M < NUT_KARATSUBA_FALLBACK_THRESHOLD ||
        N < NUT_KARATSUBA_FALLBACK_THRESHOLD ||
        P < NUT_KARATSUBA_FALLBACK_THRESHOLD)
    {
        signed_multiply(a, M, b, N, x, P);
        // signed_ntt_multiply(a, M, b, N, x, P);
        return;
    }

    // karatsuba 算法不能处理负数的补数形式
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

    // 调用 karatsuba 算法
    unsigned_karatsuba_multiply((nullptr != aa ? aa : a), MM,
                                (nullptr != bb ? bb : b), NN, x, P);

    // 还原符号
    if (a_neg != b_neg)
        signed_negate(x, P, x, P);
}

}

#endif
