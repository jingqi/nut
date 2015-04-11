
#ifndef ___HEADFILE_2B908A00_62CA_4D3C_ADB3_8CB4BAB97554_
#define ___HEADFILE_2B908A00_62CA_4D3C_ADB3_8CB4BAB97554_

#include "../word_array_integer.h"

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
void unsigned_karatsuba_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, memory_allocator *ma)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);

    // 去除无效位长
    M = (std::min)(unsigned_significant_size(a, M), P);
    N = (std::min)(unsigned_significant_size(b, N), P);

    // 规模较小时使用一般算法
    // 这个边界值是一个经验值，并且与平台相关。在 VC 上，如果设置的过小，性能甚至会急剧下降!
#if defined(NUT_PLATFORM_CC_VC)
    const size_t LIMIT = 256;
#else
    const size_t LIMIT = 32;
#endif

    if (M < LIMIT || N < LIMIT || P < LIMIT)
    {
        unsigned_multiply(a, M, b, N, x, P, ma);
        return;
    }

    // 准备变量 A、B、C、D
    const size_t base_len = ((std::max)(M, N) + 1) / 2;
    const T ZERO = 0;

    // a_len = max(0, min(M - base_len, P - base_len))
    //       = max(0, M - base_len)
    // since, M <= P
    const T *A = a + base_len;
    size_t a_len = M - base_len;
    if (M <= base_len)
    {
        A = &ZERO;
        a_len = 1;
    }

    // b_len = min(base_len, M)
    const T *B = a;
    const size_t b_len = (std::min)(base_len, M);

    // c_len = max(0, min(N - base_len, P - base_len))
    //       = max(0, N - base_len)
    // since, N <= P
    const T *C = b + base_len;
    size_t c_len = N - base_len;
    if (N <= base_len)
    {
        C = &ZERO;
        c_len = 1;
    }

    // d_len = min(base_len, N)
    const T *D = b;
    size_t d_len = (std::min)(base_len, N);

    // 计算中间结果

    // ab_len = min(max(a_len, b_len) + 1, P - base_len)
    //        = min(b_len + 1, P - base_len)
    // since, a_len <= b_len
    const size_t ab_len = (std::min)(b_len + 1, P - base_len);
    T *AB = (T*) ma_alloc(ma, sizeof(T) * ab_len);
    unsigned_add(A, a_len, B, b_len, AB, ab_len, ma);

    // cd_len = min(max(c_len, d_len) + 1, P - base_len)
    //        = min(d_len + 1, P - base_len)
    // since, c_len <= d_len
    const size_t cd_len = (std::min)(d_len + 1, P - base_len);
    T *CD = (T*) ma_alloc(ma, sizeof(T) * cd_len);
    unsigned_add(C, c_len, D, d_len, CD, cd_len, ma);

    // abcd_len = min(ab_len + cd_len, P - base_len)
    //          = min(b_len + d_len + 1, P - base_len)
    const size_t abcd_len = (std::min)(b_len + d_len + 1, P - base_len);
    T *ABCD = (T*) ma_alloc(ma, sizeof(T) * abcd_len);
    unsigned_karatsuba_multiply(AB, ab_len, CD, cd_len, ABCD, abcd_len, ma);
    ma_free(ma, AB);
    ma_free(ma, CD);

    // ac_len = min(a_len + c_len, max(0, P - base_len))
    //        = min(a_len + c_len, P - base_len)
    // since, P - base_len >= max(M,N) - base_len > 0
    const size_t ac_len = (std::min)(a_len + c_len, P - base_len);
    T *AC = (T*) ma_alloc(ma, sizeof(T) * ac_len);
    unsigned_karatsuba_multiply(A, a_len, C, c_len, AC, ac_len, ma);

    // bd_len = min(b_len + d_len, P)
    const size_t bd_len = (std::min)(b_len + d_len, P);
    T *BD = x; // 为了避免传入的参数 a, b 被破坏(a、b、x可能有交叉区域)，BD要放在后面算
    unsigned_karatsuba_multiply(B, b_len, D, d_len, BD, bd_len, ma);

    unsigned_sub(ABCD, abcd_len, AC, ac_len, ABCD, abcd_len, ma);
    unsigned_sub(ABCD, abcd_len, BD, bd_len, ABCD, abcd_len, ma);

    // 生成最终结果
    if (bd_len < P)
        ::memset(x + bd_len, 0, sizeof(T) * (P - bd_len));
    unsigned_add(x + base_len, P - base_len, ABCD, abcd_len, x + base_len, P - base_len, ma);
    ma_free(ma, ABCD);
    if (P > base_len * 2)
        unsigned_add(x + base_len * 2, P - base_len * 2, AC, ac_len, x + base_len * 2, P - base_len * 2, ma);
    ma_free(ma, AC);
}

/**
 * karatsuba 乘法，时间复杂度为 O(n^(log2 3)) = O(n^1.58)
 *
 * a = A * base + B (其中 base = 2 ^ (n/2) )
 * b = C * base + D
 * a * b = AC * (base^2) + ((A+B)(C+D) - AC - BD) * base + BD
 * 该算式中乘法总体规模变小了
 */
template <typename T>
void signed_karatsuba_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, memory_allocator *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);

    // karatsuba 算法不能处理负数的补数形式
    T *aa = const_cast<T*>(a), *bb = const_cast<T*>(b);
    size_t MM = M, NN = N;
    bool neg = false;
    if (!is_positive(a, M))
    {
        ++MM;
        aa = (T*) ma_alloc(ma, sizeof(T) * MM);
        signed_negate(a, M, aa, MM, ma);
        neg = !neg;
    }
    if (!is_positive(b, N))
    {
        ++NN;
        bb = (T*) ma_alloc(ma, sizeof(T) * NN);
        signed_negate(b, N, bb, NN, ma);
        neg = !neg;
    }

    // 调用 karatsuba 算法
    unsigned_karatsuba_multiply(aa, MM, bb, NN, x, P, ma);
    if (aa != a)
        ma_free(ma, aa);
    if (bb != b)
        ma_free(ma, bb);

    // 还原符号
    if (neg)
        signed_negate(x, P, x, P, ma);
}

}

#endif
