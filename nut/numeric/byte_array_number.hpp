/**
 * @file -
 * @author jingqi
 * @date 2011-12-17
 * @last-edit 2013-01-31 09:12:48 jingqi
 * @brief
 *
 * 关于函数命名后缀：
 *     "signed"后缀标记了处理有符号数的函数；"unsigned"后缀标记了处理无符号数的函数；其他函数则通用于有符号数和无符号数。
 */

#ifndef ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_
#define ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_

#include <assert.h>
#include <string.h> // for memset(), memcpy(), memmove()
#include <stdlib.h> // for malloc()

#include <nut/platform/platform.hpp>
#include <nut/platform/stdint.hpp>

// 是否极限优化
#define OPTIMIZE

// 定义半字、字、双字类型数
#if defined(NUT_PLATFORM_BITS_64)
#   define hword_type uint32_t
#   define word_type uint64_t
#   define dword_type uint128_t
#elif defined(NUT_PLATFORM_BITS_32)
#   define hword_type uint16_t
#   define word_type uint32_t
#   define dword_type uint64_t
#else
#   define hword_type uint8_t
#   define word_type uint16_t
#   define dword_type uint32_t
#endif

namespace nut
{

/**
 * (有符号数、无符号数)是否为0
 *
 * @return a<N> == 0
 */
inline bool is_zero(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);

#if !defined(OPTIMIZE)
    for (register size_t i = 0; i < N; ++i)
        if (0 != a[i])
            return false;
    return true;
#else
    const size_t word_count = N / sizeof(word_type);
    for (register size_t i = 0; i < word_count; ++i)
        if (0 != reinterpret_cast<const word_type*>(a)[i])
            return false;
    for (register size_t i = sizeof(word_type) * word_count; i < N; ++i)
        if (0 != a[i])
            return false;
    return true;
#endif
}

/**
 * (有符号数)是否为正数或者0
 *
 * @return true, 参数 >= 0
 *      false, 参数 < 0
 */
inline bool is_positive_signed(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);
    return 0 == (a[N - 1] & 0x80);
}

/**
 * (有符号数)有效字节数
 *
 * @return 返回值>=1
 */
inline size_t significant_size_signed(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);

    const bool positive = is_positive_signed(a, N);
    const uint8_t skip_value = (positive ? 0 : 0xFF);
    register size_t ret = N;
    while (ret > 1 && a[ret - 1] == skip_value && is_positive_signed(a, ret - 1) == positive)
        --ret;
    return ret;
}

/**
 * (无符号数)有效字节数
 *
 * @return 返回值>=1
 */
inline size_t significant_size_unsigned(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);

    register size_t ret = N;
    while (ret > 1 && a[ret - 1] == 0)
        --ret;
    return ret;
}

inline bool equal_unsigned(const uint8_t *a, size_t M, const uint8_t *b, size_t N)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);

#if !defined(OPTIMIZE)
    const size_t limit = (M > N ? M : N);
    for (register size_t i = 0; i < limit; ++i)
        if ((i < M ? a[i] : 0) != (i < N ? b[i] : 0))
            return false;
    return true;
#else
    const size_t word_count = (M < N ? M : N) / sizeof(word_type);
    for (register size_t i = 0; i < word_count; ++i)
        if (reinterpret_cast<const word_type*>(a)[i] != reinterpret_cast<const word_type*>(b)[i])
            return false;
    const size_t limit = (M > N ? M : N);
    for (register size_t i = word_count * sizeof(word_type); i < limit; ++i)
        if ((i < M ? a[i] : 0) != (i < N ? b[i] : 0))
            return false;
    return true;
#endif
}

inline bool equal_signed(const uint8_t *a, size_t M, const uint8_t *b, size_t N)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);

    const bool positive1 = is_positive_signed(a, M), positive2 = is_positive_signed(b, N);
    if (positive1 != positive2)
        return false;

#if !defined(OPTIMIZE)
    const uint8_t fill = (positive1 ? 0 : 0xFF);
    const size_t limit = (M > N ? M : N);
    for (register size_t i = 0; i < limit; ++i)
        if ((i < M ? a[i] : fill) != (i < N ? b[i] : fill))
            return false;
    return true;
#else
    const uint8_t fill = (positive1 ? 0 : 0xFF);
    const size_t word_count = (M < N ? M : N) / sizeof(word_type);
    for (register size_t i = 0; i < word_count; ++i)
        if (reinterpret_cast<const word_type*>(a)[i] != reinterpret_cast<const word_type*>(b)[i])
            return false;
    const size_t limit = (M > N ? M : N);
    for (register size_t i = word_count * sizeof(word_type); i < limit; ++i)
        if ((i < M ? a[i] : fill) != (i < N ? b[i] : fill))
            return false;
    return true;
#endif
}

/**
 * (无符号数)小于
 *
 * @return a<N> < b<N>
 */
inline bool less_than_unsigned(const uint8_t *a, const uint8_t *b, size_t N)
{
    assert(NULL != a && NULL != b && N > 0);

#if !defined(OPTIMIZE)
    for (register int i = N - 1; i >= 0; --i)
        if (a[i] != b[i])
            return a[i] < b[i];
    return false; // 相等
#else
    const size_t word_count = N / sizeof(word_type);
    for (register int i = N - 1, bound = sizeof(word_type) * word_count; i >= bound; --i)
        if (a[i] != b[i])
            return a[i] < b[i];
    for (register int i = word_count - 1; i >= 0; --i)
        if (reinterpret_cast<const word_type*>(a)[i] != reinterpret_cast<const word_type*>(b)[i])
            return reinterpret_cast<const word_type*>(a)[i] < reinterpret_cast<const word_type*>(b)[i];
    return false; // 相等
#endif
}

/**
 * (无符号数)小于
 *
 * @return a<M> < b<N>
 */
inline bool less_than_unsigned(const uint8_t *a, size_t M, const uint8_t *b, size_t N)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);

#if !defined(OPTIMIZE)
    for (register int i = (M > N ? M : N) - 1; i >= 0; --i)
    {
        const uint8_t op1 = (i < (int)M ? a[i] : 0);
        const uint8_t op2 = (i < (int)N ? b[i] : 0);
        if (op1 != op2)
            return op1 < op2;
    }
    return false; // 相等
#else
    const size_t word_count = (M < N ? M : N) / sizeof(word_type);
    for (register int i = (M > N ? M : N) - 1, limit = sizeof(word_type) * word_count;
        i >= limit; --i)
    {
        const uint8_t op1 = (i < (int) M ? a[i] : 0);
        const uint8_t op2 = (i < (int) N ? b[i] : 0);
        if (op1 != op2)
            return op1 < op2;
    }
    for (register int i = word_count - 1; i >= 0; --i)
        if (reinterpret_cast<const word_type*>(a)[i] != reinterpret_cast<const word_type*>(b)[i])
            return reinterpret_cast<const word_type*>(a)[i] < reinterpret_cast<const word_type*>(b)[i];
    return false; // 相等
#endif
}

/**
 * (有符号数)小于
 *
 * @return a<N> < b<N>
 */
inline bool less_than_signed(const uint8_t *a, const uint8_t *b, size_t N)
{
    assert(NULL != a && NULL != b && N > 0);

    const bool positive1 = is_positive_signed(a, N), positive2 = is_positive_signed(b, N);
    if (positive1 != positive2)
        return positive2;

    return less_than_unsigned(a, b, N);
}

/**
 * (有符号数)小于
 *
 * @return a<M> < b<N>
 */
inline bool less_than_signed(const uint8_t *a, size_t M, const uint8_t *b, size_t N)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);

    const bool positive1 = is_positive_signed(a, M), positive2 = is_positive_signed(b, N);
    if (positive1 != positive2)
        return positive2;

#if !defined(OPTIMIZE)
    const uint8_t fill = (positive1 ? 0 : 0xFF);
    for (register int i = (M > N ? M : N) - 1; i >= 0; --i)
    {
        const uint8_t op1 = (i < (int)M ? a[i] : fill);
        const uint8_t op2 = (i < (int)N ? b[i] : fill);
        if (op1 != op2)
            return op1 < op2;
    }
    return false; // 相等
#else
    const uint8_t fill = (positive1 ? 0 : 0xFF);
    const size_t word_count = (M < N ? M : N) / sizeof(word_type);
    for (register int i = (M > N ? M : N) - 1, bound = sizeof(word_type) * word_count;
        i >= bound; --i)
    {
        const uint8_t op1 = (i < (int) M ? a[i] : fill);
        const uint8_t op2 = (i < (int) N ? b[i] : fill);
        if (op1 != op2)
            return op1 < op2;
    }
    for (register int i = word_count - 1; i >= 0; --i)
        if (reinterpret_cast<const word_type*>(a)[i] != reinterpret_cast<const word_type*>(b)[i])
            return reinterpret_cast<const word_type*>(a)[i] < reinterpret_cast<const word_type*>(b)[i];
    return false; // 相等
#endif
}

/**
 * (有符号数)带符号扩展(截断)
 * a<M> -> x<N>
 */
inline void expand_signed(const uint8_t *a, size_t M, uint8_t *x, size_t N)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);

    if (x != a)
        ::memmove(x, a, (M < N ? M : N));
    if (M < N)
        ::memset(x + M, (is_positive_signed(a, M) ? 0 : 0xFF), N - M);
}

/**
 * (无符号数)无符号扩展(截断)
 * a<M> -> x<N>
 */
inline void expand_unsigned(const uint8_t *a, size_t M, uint8_t *x, size_t N)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);

    if (x != a)
        ::memmove(x, a, (M < N ? M : N));
    if (M < N)
        ::memset(x + M, 0, N - M);
}

/**
 * (有符号数、无符号数)相加
 * x<N> = a<N> + b<N>
 *
 * @return 进位
 */
inline uint8_t add(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * N);

#if !defined(OPTIMIZE)
    register uint8_t carry = 0;
    for (register size_t i = 0; i < N; ++i)
    {
        const uint16_t pluser1 = a[i];
        uint16_t pluser2 = b[i];
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
#else
    register uint8_t carry = 0;
    const size_t word_count = N / sizeof(word_type);
    for (register size_t i = 0; i < word_count; ++i)
    {
        const dword_type pluser1 = reinterpret_cast<const word_type*>(a)[i];
        dword_type pluser2 = reinterpret_cast<const word_type*>(b)[i];
        pluser2 += pluser1 + carry;

        reinterpret_cast<word_type*>(retx)[i] = static_cast<word_type>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
    {
        const uint16_t pluser1 = a[i];
        uint16_t pluser2 = b[i];
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
#endif

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, N);
        ::free(retx);
    }
    return carry;
}

/**
 * (有符号数)相加
 * x<P> = a<M> + b<N>
 *
 * @return 进位
 */
inline uint8_t add_signed(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * P);

#if !defined(OPTIMIZE)
    register uint8_t carry = 0;
    const uint16_t filla = (is_positive_signed(a, M) ? 0 : 0x00FF), fillb = (is_positive_signed(b, N) ? 0 : 0x00FF);
    for (register size_t i = 0; i < P; ++i)
    {
        const uint16_t pluser1 = (i < M ? a[i] : filla);
        uint16_t pluser2 = (i < N ? b[i] : fillb);
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
#else
    register uint8_t carry = 0;
    const size_t word_count = (M < N ? (M < P ? M : P) : (N < P ? N : P)) / sizeof(word_type);
    for (register size_t i = 0; i < word_count; ++i)
    {
        const dword_type pluser1 = reinterpret_cast<const word_type*>(a)[i];
        dword_type pluser2 = reinterpret_cast<const word_type*>(b)[i];
        pluser2 += pluser1 + carry;

        reinterpret_cast<word_type*>(retx)[i] = static_cast<word_type>(pluser2);
        carry = static_cast<word_type>(pluser2 >> (sizeof(word_type) * 8));
    }
    const uint16_t filla = (is_positive_signed(a, M) ? 0 : 0x00FF), fillb = (is_positive_signed(b, N) ? 0 : 0x00FF);
    for (register size_t i = sizeof(word_type) * word_count; i < P; ++i)
    {
        const uint16_t pluser1 = (i < M ? a[i] : filla);
        uint16_t pluser2 = (i < N ? b[i] : fillb);
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
#endif

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, P);
        ::free(retx);
    }
    return carry;
}

/**
 * (无符号数)相加
 * x<P> = a<M> + b<N>
 *
 * @return 进位
 */
inline uint8_t add_unsigned(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * P);

#if !defined(OPTIMIZE)
    register uint8_t carry = 0;
    for (register size_t i = 0; i < P; ++i)
    {
        const uint16_t pluser1 = (i < M ? a[i] : 0);
        uint16_t pluser2 = (i < N ? b[i] : 0);
        pluser2 += pluser1 + carry;
        
        retx[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
#else
    register uint8_t carry = 0;
    const size_t word_count = (M < N ? (M < P ? M : P) : (N < P ? N : P)) / sizeof(word_type);
    for (register size_t i = 0; i < word_count; ++i)
    {
        const dword_type pluser1 = reinterpret_cast<const word_type*>(a)[i];
        dword_type pluser2 = reinterpret_cast<const word_type*>(b)[i];
        pluser2 += pluser1 + carry;

        reinterpret_cast<word_type*>(retx)[i] = static_cast<word_type>(pluser2);
        carry = static_cast<word_type>(pluser2 >> (sizeof(word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(word_type); i < P; ++i)
    {
        const uint16_t pluser1 = (i < M ? a[i] : 0);
        uint16_t pluser2 = (i < N ? b[i] : 0);
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
#endif

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, P);
        ::free(retx);
    }
    return carry;
}

/**
 * (有符号数、无符号数)加1
 * x<N> += 1
 *
 * @return 进位
 */
inline uint8_t increase(uint8_t *x, size_t N)
{
    assert(NULL != x && N > 0);

#if !defined(OPTIMIZE)
    register uint8_t carry = 1;
    for (register size_t i = 0; i < N && 0 != carry; ++i)
    {
        uint16_t pluser = x[i];
        pluser += carry;
        
        x[i] = static_cast<uint8_t>(pluser);
        carry = static_cast<uint8_t>(pluser >> (sizeof(uint8_t) * 8));
    }
    return carry;
#else
    const size_t word_count = N / sizeof(word_type);
    register uint8_t carry = 1;
    for (register size_t i = 0; i < word_count && 0 != carry; ++i)
    {
        dword_type pluser = reinterpret_cast<const word_type*>(x)[i];
        pluser += carry;

        reinterpret_cast<word_type*>(x)[i] = static_cast<word_type>(pluser);
        carry = static_cast<uint8_t>(pluser >> (sizeof(word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(word_type); i < N && 0 != carry; ++i)
    {
        uint16_t pluser = x[i];
        pluser += carry;

        x[i] = static_cast<uint8_t>(pluser);
        carry = static_cast<uint8_t>(pluser >> (sizeof(uint8_t) * 8));
    }
    return carry;
#endif
}

/**
 * (有符号数、无符号数)相减
 * x<N> = a<N> - b<N>
 *
 * @return 进位
 */
inline uint8_t sub(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * N);

#if !defined(OPTIMIZE)
    register uint8_t carry = 1;
    for (register size_t i = 0; i < N; ++i)
    {
        const uint16_t pluser1 = a[i];
        uint16_t pluser2 = static_cast<uint8_t>(~b[i]);
        pluser2 += pluser1 + carry;
        
        retx[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
#else
    const size_t word_count = N / sizeof(word_type);
    register uint8_t carry = 1;
    for (register size_t i = 0; i < word_count; ++i)
    {
        const dword_type pluser1 = reinterpret_cast<const word_type*>(a)[i];
        dword_type pluser2 = static_cast<word_type>(~reinterpret_cast<const word_type*>(b)[i]);
        pluser2 += pluser1 + carry;

        reinterpret_cast<word_type*>(retx)[i] = static_cast<word_type>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
    {
        const uint16_t pluser1 = a[i];
        uint16_t pluser2 = static_cast<uint8_t>(~b[i]);
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
#endif

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, N);
        ::free(retx);
    }
    return carry;
}

/**
 * (有符号数)相减
 * x<P> = a<M> - b<N>
 *
 * @return 进位
 */
inline uint8_t sub_signed(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * P);

#if !defined(OPTIMIZE)
    const uint16_t filla = (is_positive_signed(a, M) ? 0 : 0x00FF), fillb = (is_positive_signed(b, N) ? 0 : 0x00FF);
    register uint8_t carry = 1;
    for (register size_t i = 0; i < P; ++i)
    {
        const uint16_t pluser1 = (i < M ? a[i] : filla);
        uint16_t pluser2 = static_cast<uint8_t>(~(i < N ? b[i] : fillb));
        pluser2 += pluser1 + carry;
        
        retx[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
#else
    const size_t word_count = (M < N ? (M < P ? M : P) : (N < P ? N : P)) / sizeof(word_type);
    register uint8_t carry = 1;
    for (register size_t i = 0; i < word_count; ++i)
    {
        const dword_type pluser1 = reinterpret_cast<const word_type*>(a)[i];
        dword_type pluser2 = static_cast<word_type>(~reinterpret_cast<const word_type*>(b)[i]);
        pluser2 += pluser1 + carry;

        reinterpret_cast<word_type*>(retx)[i] = static_cast<word_type>(pluser2);
        carry = static_cast<word_type>(pluser2 >> (sizeof(word_type) * 8));
    }
    const uint16_t filla = (is_positive_signed(a, M) ? 0 : 0x00FF), fillb = (is_positive_signed(b, N) ? 0 : 0x00FF);
    for (register size_t i = word_count * sizeof(word_type); i < P; ++i)
    {
        const uint16_t pluser1 = (i < M ? a[i] : filla);
        uint16_t pluser2 = static_cast<uint8_t>(~(i < N ? b[i] : fillb));
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
#endif

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, P);
        ::free(retx);
    }
    return carry;
}

/**
 * (无符号数)相减
 * x<P> = a<M> - b<N>
 *
 * @return 进位
 */
inline uint8_t sub_unsigned(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * P);

#if !defined(OPTIMIZE)
    register uint8_t carry = 1;
    for (register size_t i = 0; i < P; ++i)
    {
        const uint16_t pluser1 = (i < M ? a[i] : 0);
        uint16_t pluser2 = static_cast<uint8_t>(~(i < N ? b[i] : 0));
        pluser2 += pluser1 + carry;
        
        retx[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
#else
    const size_t word_count = (M < N ? (M < P ? M : P) : (N < P ? N : P)) / sizeof(word_type);
    register uint8_t carry = 1;
    for (register size_t i = 0; i < word_count; ++i)
    {
        const dword_type pluser1 = reinterpret_cast<const word_type*>(a)[i];
        dword_type pluser2 = static_cast<word_type>(~reinterpret_cast<const word_type*>(b)[i]);
        pluser2 += pluser1 + carry;

        reinterpret_cast<word_type*>(retx)[i] = static_cast<word_type>(pluser2);
        carry = static_cast<word_type>(pluser2 >> (sizeof(word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(word_type); i < P; ++i)
    {
        const uint16_t pluser1 = (i < M ? a[i] : 0);
        uint16_t pluser2 = static_cast<uint8_t>(~(i < N ? b[i] : 0));
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
#endif

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, P);
        ::free(retx);
    }
    return carry;
}

/**
 * (有符号数、无符号数)减1
 * x<N> -= 1
 *
 * @return 进位
 */
inline uint8_t decrease(uint8_t *x, size_t N)
{
    assert(NULL != x && N > 0);

#if !defined(OPTIMIZE)
    register uint8_t carry = 0;
    for (register size_t i = 0; i < N && 1 != carry; ++i)
    {
        uint16_t pluser = x[i];
        pluser += carry + 0x00FF;
        
        x[i] = static_cast<uint8_t>(pluser);
        carry = static_cast<uint8_t>(pluser >> (sizeof(uint8_t) * 8));
    }
    return carry;
#else
    const size_t word_count = N / sizeof(word_type);
    register uint8_t carry = 0;
    for (register size_t i = 0; i < word_count && 1 != carry; ++i)
    {
        dword_type pluser = reinterpret_cast<const word_type*>(x)[i];
        pluser += carry + ((dword_type) ~ (word_type) 0);

        reinterpret_cast<word_type*>(x)[i] = static_cast<word_type>(pluser);
        carry = static_cast<uint8_t>(pluser >> (sizeof(word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(word_type); i < N && 1 != carry; ++i)
    {
        uint16_t pluser = x[i];
        pluser += carry + 0x00FF;

        x[i] = static_cast<uint8_t>(pluser);
        carry = static_cast<uint8_t>(pluser >> (sizeof(uint8_t) * 8));
    }
    return carry;
#endif
}

/**
 * (有符号数)取相反数
 * x<N> = -a<N>
 *
 * @return 进位
 */
inline uint8_t negate_signed(const uint8_t *a, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != x && N > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if (a < x && x < a + N)
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * N);

#if !defined(OPTIMIZE)
    register uint8_t carry = 1;
    for (register size_t i = 0; i < N; ++i)
    {
        uint16_t pluser = static_cast<uint8_t>(~a[i]);
        pluser += carry;

        retx[i] = static_cast<uint8_t>(pluser);
        carry = static_cast<uint8_t>(pluser >> (sizeof(uint8_t) * 8));
    }
#else
    const size_t word_count = N / sizeof(word_type);
    register uint8_t carry = 1;
    for (register size_t i = 0; i < word_count; ++i)
    {
        dword_type pluser = static_cast<word_type>(~reinterpret_cast<const word_type*>(a)[i]);
        pluser += carry;

        reinterpret_cast<word_type*>(retx)[i] = static_cast<word_type>(pluser);
        carry = static_cast<uint8_t>(pluser >> (sizeof(word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
    {
        uint16_t pluser = static_cast<uint8_t>(~a[i]);
        pluser += carry;

        retx[i] = static_cast<uint8_t>(pluser);
        carry = static_cast<uint8_t>(pluser >> (sizeof(uint8_t) * 8));
    }
#endif

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, N);
        ::free(retx);
    }
    return carry;
}

inline uint8_t negate_signed(const uint8_t *a, size_t M, uint8_t *x, size_t N)
{
	assert(NULL != a && M > 0 && NULL != x && N > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if (a < x && x < a + M)
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * N);

#if !defined(OPTIMIZE)
    register uint8_t carry = 1;
    const uint8_t fill = (is_positive_signed(a, M) ? 0 : 0xFF);
    for (register size_t i = 0; i < N; ++i)
    {
        uint16_t pluser = static_cast<uint8_t>(~(i < M ? a[i] : fill));
        pluser += carry;

        retx[i] = static_cast<uint8_t>(pluser);
        carry = static_cast<uint8_t>(pluser >> (sizeof(uint8_t) * 8));
    }
#else
    const size_t word_count = (M < N ? M : N) / sizeof(word_type);
    register uint8_t carry = 1;
    for (register size_t i = 0; i < word_count; ++i)
    {
        dword_type pluser = static_cast<word_type>(~reinterpret_cast<const word_type*>(a)[i]);
        pluser += carry;

        reinterpret_cast<word_type*>(retx)[i] = static_cast<word_type>(pluser);
        carry = static_cast<uint8_t>(pluser >> (sizeof(word_type) * 8));
    }
    const uint8_t fill = (is_positive_signed(a, M) ? 0 : 0xFF);
    for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
    {
        uint16_t pluser = static_cast<uint8_t>(~(i < M ? a[i] : fill));
        pluser += carry;

        retx[i] = static_cast<uint8_t>(pluser);
        carry = static_cast<uint8_t>(pluser >> (sizeof(uint8_t) * 8));
    }
#endif

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, N);
        ::free(retx);
    }
    return carry;
}

/**
 * 相乘
 * x<N> = a<N> * b<N>
 */
inline void multiply(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if ((a - N < x && x < a + N) || (b - N < x && x < b + N))
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * N);

    ::memset(retx, 0, N);
    const size_t word_count = N / sizeof(word_type);
    for (register size_t i = 0; i < word_count; ++i)
    {
        word_type carry = 0;
        const dword_type mult1 = reinterpret_cast<const word_type*>(a)[i];
        if (mult1 == 0)
            continue;

        register size_t j = 0;
        for (; i + j < word_count; ++j)
        {
            dword_type mult2 = reinterpret_cast<const word_type*>(b)[j];
            mult2 = mult1 * mult2 + reinterpret_cast<word_type*>(retx)[i + j] + carry;

            reinterpret_cast<word_type*>(retx)[i + j] = static_cast<word_type>(mult2);
            carry = static_cast<word_type>(mult2 >> (sizeof(word_type) * 8));
        }

        size_t bound = i * sizeof(word_type);
        for (register size_t k = j * sizeof(word_type); k + bound < N; ++k)
        {
            dword_type mult2 = b[k];
            mult2 = mult1 * mult2 + retx[k + bound] + carry;

            retx[k + bound] = static_cast<uint8_t>(mult2);
            carry = static_cast<word_type>(mult2 >> (sizeof(uint8_t) * 8));
        }
    }

    for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
    {
        uint8_t carry = 0;
        const uint16_t mult1 = a[i];
        if (mult1 == 0)
            continue;

        for (register size_t j = 0; i + j < N; ++j)
        {
            uint16_t mult2 = b[j];
            mult2 = mult1 * mult2 + retx[i + j] + carry;

            retx[i + j] = static_cast<uint8_t>(mult2);
            carry = static_cast<uint8_t>(mult2 >> (sizeof(uint8_t) * 8));
        }
    }

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, N);
        ::free(retx);
    }
}

/**
 * (有符号数)相乘
 * x<P> = a<M> * b<N>
 */
inline void multiply_signed(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if ((a - P < x && x < a + M) || (b - P < x && x < b + N))
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * P);

    // 乘法
    ::memset(retx, 0, P);
    const uint8_t filla = (is_positive_signed(a,M) ? 0 : 0xFF), fillb = (is_positive_signed(b,N) ? 0 : 0xFF);
    for (register size_t i = 0; i < P; ++i)
    {
        uint8_t carry = 0;
        const uint16_t mult1 = (i < M ? a[i] : filla);
        if (i >= M && 0 == filla)
            break;
        if (mult1 == 0)
            continue;

        for (register size_t j = 0; i + j < P; ++j)
        {
            uint16_t mult2 = (j < N ? b[j] : fillb);
            mult2 = mult1 * mult2 + retx[i + j] + carry;

            retx[i + j] = static_cast<uint8_t>(mult2);
            carry = static_cast<uint8_t>(mult2 >> (sizeof(uint8_t) * 8));
        }
    }

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, P);
        ::free(retx);
    }
}

/**
 * (无符号数)相乘
 * x<P> = a<M> * b<N>
 */
inline void multiply_unsigned(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if ((a - P < x && x < a + M) || (b - P < x && x < b + N))
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * P);

    // 乘法
    ::memset(retx, 0, P);
    for (register size_t i = 0; i < P && i < M; ++i)
    {
        uint8_t carry = 0;
        const uint16_t mult1 = a[i];
        if (mult1 == 0)
            continue;

        for (register size_t j = 0; i + j < P; ++j)
        {
            uint16_t mult2 = (j < N ? b[j] : 0);
            mult2 = mult1 * mult2 + retx[i + j] + carry;

            retx[i + j] = static_cast<uint8_t>(mult2);
            carry = static_cast<uint8_t>(mult2 >> (sizeof(uint8_t) * 8));
        }
    }

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, P);
        ::free(retx);
    }
}

/**
 * 左移
 * x<N> = a<N> << count
 */
inline void shift_left(const uint8_t *a, uint8_t *x, size_t N, size_t count)
{
    assert(NULL != a && NULL != x && N > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if (a - N < x && x < a - count / 8)
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * N);

    const int bytes_off = count / 8, bits_off = count % 8;
    for (register int i = N - 1; i >= 0; --i)
    {
        const uint8_t high = ((i - bytes_off >= 0 ? a[i - bytes_off] : 0) << bits_off);
        const uint8_t low = ((i - bytes_off - 1 >= 0 ? a[i - bytes_off - 1] : 0) >> (8 - bits_off));
        retx[i] = high | low;
    }

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, N);
        ::free(retx);
    }
}

/**
 * (无符号数)右移
 * x<N> = a<N> >> count
 */
inline void shift_right_unsigned(const uint8_t *a, uint8_t *x, size_t N, size_t count)
{
    assert(NULL != a && NULL != x && N > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if (a + count / 8 < x && x < a + N)
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * N);

    const int bytes_off = count / 8, bits_off = count % 8;
    for (register size_t i = 0; i < N; ++i)
    {
        const uint8_t high = ((i + bytes_off + 1 >= N ? 0 : a[i + bytes_off + 1]) << (8 - bits_off));
        const uint8_t low = ((i + bytes_off >= N ? 0 : a[i + bytes_off]) >> bits_off);
        retx[i] = high | low;
    }

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, N);
        ::free(retx);
    }
}

/**
 * (有符号数)右移
 * x<N> = a<N> >> count
 */
inline void shift_right_signed(const uint8_t *a, uint8_t *x, size_t N, size_t count)
{
    assert(NULL != a && NULL != x && N > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if (a + count / 8 < x && x < a + N)
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * N);

    const int bytes_off = count / 8, bits_off = count % 8;
    const uint8_t fill = (is_positive_signed(a, N) ? 0 : 0xFF);
    for (register size_t i = 0; i < N; ++i)
    {
        const uint8_t high = ((i + bytes_off + 1 >= N ? fill : a[i + bytes_off + 1]) << (8 - bits_off));
        const uint8_t low = ((i + bytes_off >= N ? fill : a[i + bytes_off]) >> bits_off);
        retx[i] = high | low;
    }

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, N);
        ::free(retx);
    }
}

/**
 * 循环左移
 * x<N> = a<N> <<< count
 */
inline void circle_shift_left(const uint8_t *a, uint8_t *x, size_t N, size_t count)
{
    assert(NULL != a && NULL != x && N > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if (a - N < x && x < a + N)
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * N);

    // 循环位移
    const int bytes_off = count / 8, bits_off = count % 8;
    for (register size_t i = 0; i < N; ++i)
    {
        const uint8_t high = (a[(i + N - (bytes_off % N)) % N] << bits_off);
        const uint8_t low = (a[(i + N - (bytes_off % N) - 1) % N] >> (8 - bits_off));
        retx[i] = high | low;
    }

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, N);
        ::free(retx);
    }
}

/**
 * 循环右移
 * x<N> = a<N> >>> count
 */
inline void circle_shift_right(const uint8_t *a, uint8_t *x, size_t N, size_t count)
{
    assert(NULL != a && NULL != x && N > 0);

    // 避免区域交叉覆盖
    uint8_t *retx = x;
    if (a - N < x && x < a + N)
        retx = (uint8_t*) ::malloc(sizeof(uint8_t) * N);

    // 循环右移
    const int bytes_off = count / 8, bits_off = count % 8;
    for (register size_t i = 0; i < N; ++i)
    {
        const uint8_t high = (a[(i + bytes_off + 1) % N] << (8 - bits_off));
        const uint8_t low = (a[(i + bytes_off) % N] >> bits_off);
        retx[i] = high | low;
    }

    
    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, N);
        ::free(retx);
    }
}

/**
 * (有符号数)相除
 * x<N> = a<N> / b<N>
 * y<N> = a<N> % b<N>
 *
 * @param x
 *      商
 * @param y
 *      余数
 */
inline void divide_signed(const uint8_t *a, const uint8_t *b, uint8_t *x, uint8_t *y, size_t N)
{
    assert(NULL != a && NULL != b && N > 0);
    assert(NULL != x || NULL != y);
    assert(NULL == x || NULL == y || y >= x + N || x >= y + N); // 避免区域交叉覆盖
    assert(!is_zero(b, N)); // 被除数不能为0

    // 常量
    const size_t dividend_len = significant_size_signed(a, N);
    const size_t divisor_len = significant_size_signed(b, N);
    const bool dividend_positive = is_positive_signed(a, N);
    const bool divisor_positive = is_positive_signed(b, N);

    // 避免数据在计算中途被破坏
    uint8_t *quotient = x; // 商，可以为 NULL
    if ((a - N < x && x < a) || (b - N < x && x < b + N)) // 兼容 x==a 的情况
        quotient = (uint8_t*) ::malloc(sizeof(uint8_t) * dividend_len);
    uint8_t *remainder = y; // 余数，不能为 NULL
    if (NULL == y || (a - N < y && y < a + N) || (b - N < y && y < b + N))
        remainder = (uint8_t*) ::malloc(sizeof(uint8_t) * divisor_len);

    // 逐位试商
    ::memset(remainder, (dividend_positive ? 0 : 0xFF), divisor_len); // 初始化余数
    bool remainder_positive = dividend_positive; // 余数的符号
    for (register size_t i = 0; i < dividend_len; ++i)
    {
        const size_t dividend_byte_pos = dividend_len - i - 1;
        const uint8_t next_dividend_byte = a[dividend_byte_pos]; // 余数左移时的低位补位部分
        if (NULL != quotient)
            quotient[dividend_byte_pos] = 0; // 初始化商，注意，兼容 x==a 的情况

        for (register size_t j = 0; j < 8; ++j)
        {
            // 余数左移1位
            shift_left(remainder, remainder, divisor_len, 1);
            remainder[0] |= (next_dividend_byte >> (7 - j)) & 0x01;

            // 加上/减去除数
            if (remainder_positive == divisor_positive)
                sub(remainder, b, remainder, divisor_len);
            else
                add(remainder, b, remainder, divisor_len);

            // 试商结果
            remainder_positive = is_positive_signed(remainder, divisor_len);
            if (remainder_positive == divisor_positive && NULL != quotient)
                quotient[dividend_byte_pos] |= (1 << (7 - j));
        }
    }

    /**
        修正补数形式的商:
        如果除尽且除数为负数，则商加1
        如果未除尽且商为负数，则商加1
    */
    const bool remainder_is_zero = is_zero(remainder, divisor_len);
    if (NULL != x)
    {
        assert(NULL != quotient);
        if (remainder_is_zero)
        {
            if (!divisor_positive)
                increase(quotient, dividend_len);
        }
        else
        {
            if (!is_positive_signed(quotient, dividend_len))
                increase(quotient, dividend_len);
        }
        expand_signed(quotient, dividend_len, x, N);
    }

    /**
        恢复余数:
        如果未除尽且余数符号与被除数不一致，余数需加修正
    */
    if (NULL != y)
    {
        if (!remainder_is_zero && remainder_positive != dividend_positive)
        {
            if (divisor_positive == dividend_positive)
                add(remainder, b, remainder, divisor_len);
            else
                sub(remainder, b, remainder, divisor_len);
        }
        expand_signed(remainder, divisor_len, y, N);
    }

    // 释放空间
    if (quotient != x)
        ::free(quotient);
    if (remainder != y)
        ::free(remainder);
}

/**
 * (有符号数)相除
 * x<P> = a<M> / b<N>
 * y<Q> = a<M> % b<N>
 *
 * @param x
 *      商
 * @param y
 *      余数
 */
inline void divide_signed(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P, uint8_t *y, size_t Q)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);
    assert((NULL != x && P > 0) || (NULL != y && Q > 0));
    assert(NULL == x || P <= 0 || NULL == y || Q <= 0 || y >= x + P || x >= y + Q); // 避免区域交叉覆盖
    assert(!is_zero(b, N)); // 被除数不能为0

    // 常量
    const size_t dividend_len = significant_size_signed(a, M);
    const size_t divisor_len = significant_size_signed(b, N);
    const bool dividend_positive = is_positive_signed(a, M);
    const bool divisor_positive = is_positive_signed(b, N);
    const size_t quotient_len = (P < dividend_len ? P : dividend_len);

    // 避免数据在计算中途被破坏
    uint8_t *quotient = x; // 商，可以为 NULL
    if ((a - P < x && x < a) || (b - P < x && x < b + N)) // 兼容 x==a 的情况
        quotient = (uint8_t*) ::malloc(sizeof(uint8_t) * quotient_len);
    uint8_t *remainder = y; // 余数，不能为 NULL
    if (NULL == y || Q < divisor_len || (a - Q < y && y < a + M) || (b - Q < y && y < b + N))
        remainder = (uint8_t*) ::malloc(sizeof(uint8_t) * divisor_len);

    // 逐位试商
    ::memset(remainder, (dividend_positive ? 0 : 0xFF), divisor_len); // 初始化余数
    bool remainder_positive = dividend_positive; // 余数的符号
    bool quotient_positive = true; // 商的符号
    for (register size_t i = 0; i < dividend_len; ++i)
    {
        const size_t dividend_byte_pos = dividend_len - i - 1;
        const uint8_t next_dividend_byte = a[dividend_byte_pos]; // 余数左移时的低位补位部分
        if (NULL != quotient && dividend_byte_pos < P)
            quotient[dividend_byte_pos] = 0; // 初始化商，注意，兼容 x==a 的情况

        for (register size_t j = 0; j < 8; ++j)
        {
            // 余数左移1位
            shift_left(remainder, remainder, divisor_len, 1);
            remainder[0] |= (next_dividend_byte >> (7 - j)) & 0x01;

            // 加上/减去除数
            if (remainder_positive == divisor_positive)
                sub(remainder, b, remainder, divisor_len);
            else
                add(remainder, b, remainder, divisor_len);

            // 试商结果
            remainder_positive = is_positive_signed(remainder, divisor_len);
            if (remainder_positive == divisor_positive)
            {
                if (NULL != quotient && dividend_byte_pos < P)
                    quotient[dividend_byte_pos] |= (1 << (7 - j));
                if (0 == i && 0 == j)
                    quotient_positive = false;
            }
        }
    }

    /**
        修正补数形式的商:
        如果除尽且除数为负数，则商加1
        如果未除尽且商为负数，则商加1
    */
    const bool remainder_is_zero = is_zero(remainder, divisor_len);
    if (NULL != x)
    {
        assert(NULL != quotient);
        if (remainder_is_zero)
        {
            if (!divisor_positive)
                increase(quotient, quotient_len);
        }
        else
        {
            if (!quotient_positive)
                increase(quotient, quotient_len);
        }
        expand_signed(quotient, quotient_len, x, P);
    }

    /**
        恢复余数:
        如果未除尽且余数符号与被除数不一致，余数需加修正
    */
    if (NULL != y)
    {
        if (!remainder_is_zero && remainder_positive != dividend_positive)
        {
            if (divisor_positive == dividend_positive)
                add(remainder, b, remainder, divisor_len);
            else
                sub(remainder, b, remainder, divisor_len);
        }
        expand_signed(remainder, divisor_len, y, Q);
    }

    // 释放空间
    if (quotient != x)
        ::free(quotient);
    if (remainder != y)
        ::free(remainder);
}

/**
 * (无符号数)相除
 * x<N> = a<N> / b<N>
 * y<N> = a<N> % b<N>
 *
 * @param x
 *    商
 * @param y
 *    余数
 */
inline void divide_unsigned(const uint8_t *a, const uint8_t *b, uint8_t *x, uint8_t *y, size_t N)
{
    assert(NULL != a && NULL != b && N > 0);
    assert(NULL != x || NULL != y);
    assert(NULL == x || NULL == y || y >= x + N || x >= y + N); // 避免区域交叉覆盖
    assert(!is_zero(b, N)); // 被除数不能为0

    // 常量
    const size_t dividend_len = significant_size_unsigned(a, N);
    const size_t divisor_len = significant_size_unsigned(b, N);

    // 避免数据在计算中途被破坏
    uint8_t *quotient = x; // 商，可以为 NULL
    if ((a - N < x && x < a) || (b - N < x && x < b + N)) // 兼容 x==a 的情况
        quotient = (uint8_t*) ::malloc(sizeof(uint8_t) * dividend_len);
    uint8_t *remainder = y; // 余数，不能为 NULL
    if (NULL == y || (a - N < y && y < a + N) || (b - N < y && y < b + N))
        remainder = (uint8_t*) ::malloc(sizeof(uint8_t) * divisor_len);

    // 逐位试商
    ::memset(remainder, 0, divisor_len); // 初始化余数
    bool remainder_positive = true;
    for (register size_t i = 0; i < dividend_len; ++i)
    {
        const size_t dividend_byte_pos = dividend_len - i - 1;
        const uint8_t next_dividend_byte = a[dividend_byte_pos]; // 余数左移时的低位补位部分
        if (NULL != quotient)
            quotient[dividend_byte_pos] = 0; // 初始化商，注意，兼容 x==a 的情况

        for (register size_t j = 0; j < 8; ++j)
        {
            // 余数左移1位
            shift_left(remainder, remainder, divisor_len, 1);
            remainder[0] |= (next_dividend_byte >> (7 - j)) & 0x01;

            // 加上/减去除数
            if (remainder_positive)
                sub(remainder, b, remainder, divisor_len);
            else
                add(remainder, b, remainder, divisor_len);

            // 试商结果
            remainder_positive = is_positive_signed(remainder, divisor_len);
            if (remainder_positive && NULL != quotient)
                quotient[dividend_byte_pos] |= (1 << (7 - j));
        }
    }

    // 商
    if (NULL != x)
    {
        assert(NULL != quotient);
        expand_unsigned(quotient, dividend_len, x, N);
    }

    /**
        恢复余数:
        如果未除尽且余数符号与被除数不一致，余数需加上除数
    */
    if (NULL != y)
    {
        if (!is_zero(remainder, divisor_len) && !remainder_positive)
            add(remainder, b, remainder, divisor_len);
        expand_unsigned(remainder, divisor_len, y, N);
    }

    // 释放空间
    if (quotient != x)
        ::free(quotient);
    if (remainder != y)
        ::free(remainder);
}

/**
 * (无符号数)相除
 * x<P> = a<M> / b<N>
 * y<Q> = a<M> % b<N>
 *
 * @param x
 *    返回商
 * @param y
 *    返回余数
 */
inline void divide_unsigned(const uint8_t *a, size_t M, const uint8_t *b, size_t N, uint8_t *x, size_t P, uint8_t *y, size_t Q)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);
    assert((NULL != x && P > 0) || (NULL != y && Q > 0));
    assert(NULL == x || P <= 0 || NULL == y || Q <= 0 || y >= x + P || x >= y + Q); // 避免区域交叉覆盖
    assert(!is_zero(b, N)); // 被除数不能为0

    // 常量
    const size_t dividend_len = significant_size_unsigned(a, M);
    const size_t divisor_len = significant_size_unsigned(b, N);
    const size_t quotient_len = (P < dividend_len ? P : dividend_len);

    // 避免数据在计算中途被破坏
    uint8_t *quotient = x; // 商，可以为 NULL
    if ((a - P < x && x < a) || (b - P < x && x < b + N)) // 兼容 x==a 的情况
        quotient = (uint8_t*) ::malloc(sizeof(uint8_t) * quotient_len);
    uint8_t *remainder = y; // 余数，不能为 NULL
    if (NULL == y || Q < divisor_len || (a - Q < y && y < a + M) || (b - Q < y && y < b + N))
        remainder = (uint8_t*) ::malloc(sizeof(uint8_t) * divisor_len);

    // 逐位试商
    ::memset(remainder, 0, divisor_len); // 初始化余数
    bool remainder_positive = true;
    for (register size_t i = 0; i < dividend_len; ++i)
    {
        const size_t dividend_byte_pos = dividend_len - i - 1;
        const uint8_t next_dividend_byte = a[dividend_byte_pos]; // 余数左移时的低位补位部分
        if (NULL != quotient && dividend_byte_pos < P)
            quotient[dividend_byte_pos] = 0; // 初始化商，注意，兼容 x==a 的情况

        for (register size_t j = 0; j < 8; ++j)
        {
            // 余数左移1位
            shift_left(remainder, remainder, divisor_len, 1);
            remainder[0] |= (next_dividend_byte >> (7 - j)) & 0x01;

            // 加上/减去除数
            if (remainder_positive)
                sub(remainder, b, remainder, divisor_len);
            else
                add(remainder, b, remainder, divisor_len);

            // 试商结果
            remainder_positive = is_positive_signed(remainder, divisor_len);
            if (remainder_positive && NULL != quotient && dividend_byte_pos < P)
                quotient[dividend_byte_pos] |= (1 << (7 - j));
        }
    }
    
    // 商
    if (NULL != x)
        expand_unsigned(quotient, quotient_len, x, P);

    /**
        恢复余数:
        如果未除尽且余数符号与被除数不一致，余数需加上除数
    */
    if (NULL != y)
    {
        if (!is_zero(remainder, divisor_len) && !remainder_positive)
            add(remainder, b, remainder, divisor_len);
        expand_unsigned(remainder, divisor_len, y, Q);
    }

    // 释放空间
    if (quotient != x)
        ::free(quotient);
    if (remainder != y)
        ::free(remainder);
}

/**
 * 按位与
 * x<N> = a<N> & b<N>
 */
inline void bit_and(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);

    if ((x <= a || x >= a + N) && (x <= b || x >= b + N))
    {
#if !defined(OPTIMIZE)
        for (register size_t i = 0; i < N; ++i)
            x[i] = a[i] & b[i];
#else
        const size_t word_count = N / sizeof(word_type);
        for (register size_t i = 0; i < word_count; ++i)
            reinterpret_cast<word_type*>(x)[i] = reinterpret_cast<const word_type*>(a)[i] & reinterpret_cast<const word_type*>(b)[i];
        for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
            x[i] = a[i] & b[i];
#endif
    }
    else if ((x <= a - N || x >= a) && (x <= b - N || x >= b))
    {
#if !defined(OPTIMIZE)
        for (register size_t i = N - 1; i >= 0; --i)
            x[i] = a[i] & b[i];
#else
        const size_t word_count = N / sizeof(word_type);
        for (register int i = N - 1, limit = word_count * sizeof(word_type); i >= limit; --i)
            x[i] = a[i] & b[i];
        for (register int i = word_count - 1; i >= 0; --i)
            reinterpret_cast<word_type*>(x)[i] = reinterpret_cast<const word_type*>(a)[i] & reinterpret_cast<const word_type*>(b)[i];
#endif
    }
    else
    {
        // 避免区域交叉覆盖
        uint8_t *retx = (uint8_t*) ::malloc(sizeof(uint8_t) * N);

#if !defined(OPTIMIZE)
        for (register size_t i = 0; i < N; ++i)
            retx[i] = a[i] & b[i];
#else
        const size_t word_count = N / sizeof(word_type);
        for (register size_t i = 0; i < word_count; ++i)
            reinterpret_cast<word_type*>(retx)[i] = reinterpret_cast<const word_type*>(a)[i] & reinterpret_cast<const word_type*>(b)[i];
        for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
            retx[i] = a[i] & b[i];
#endif

        // 回写数据
        ::memcpy(x, retx, N);
        ::free(retx);
    }
}

/**
 * 按位或
 * x<N> = a<N> | b<N>
 */
inline void bit_or(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);

    if ((x <= a || x >= a + N) && (x <= b || x >= b + N))
    {
#if !defined(OPTIMIZE)
        for (register size_t i = 0; i < N; ++i)
            x[i] = a[i] | b[i];
#else
        const size_t word_count = N / sizeof(word_type);
        for (register size_t i = 0; i < word_count; ++i)
            reinterpret_cast<word_type*>(x)[i] = reinterpret_cast<const word_type*>(a)[i] | reinterpret_cast<const word_type*>(b)[i];
        for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
            x[i] = a[i] | b[i];
#endif
    }
    else if ((x <= a - N || x >= a) && (x <= b - N || x >= b))
    {
#if !defined(OPTIMIZE)
        for (register int i = N - 1; i >= 0; --i)
            x[i] = a[i] | b[i];
#else
        const size_t word_count = N / sizeof(word_type);
        for (register int i = N - 1, limit = word_count * sizeof(word_type); i >= limit; --i)
            x[i] = a[i] | b[i];
        for (register int i = word_count - 1; i >= 0; --i)
            reinterpret_cast<word_type*>(x)[i] = reinterpret_cast<const word_type*>(a)[i] | reinterpret_cast<const word_type*>(b)[i];
#endif
    }
    else
    {
        // 避免区域交叉覆盖
        uint8_t *retx = (uint8_t*) ::malloc(sizeof(uint8_t) * N);

#if !defined(OPTIMIZE)
        for (register size_t i = 0; i < N; ++i)
            retx[i] = a[i] | b[i];
#else
        const size_t word_count = N / sizeof(word_type);
        for (register size_t i = 0; i < word_count; ++i)
            reinterpret_cast<word_type*>(retx)[i] = reinterpret_cast<const word_type*>(a)[i] | reinterpret_cast<const word_type*>(b)[i];
        for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
            retx[i] = a[i] | b[i];
#endif

        // 回写数据
        ::memcpy(x, retx, N);
        ::free(retx);
    }
}

/**
 * 按位异或
 * x<N> = a<N> ^ b<N>
 */
inline void bit_xor(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);

    if ((x <= a || x >= a + N) && (x <= b || x >= b + N))
    {
#if !defined(OPTIMIZE)
        for (register size_t i = 0; i < N; ++i)
            x[i] = a[i] ^ b[i];
#else
        const size_t word_count = N / sizeof(word_type);
        for (register size_t i = 0; i < word_count; ++i)
            reinterpret_cast<word_type*>(x)[i] = reinterpret_cast<const word_type*>(a)[i] ^ reinterpret_cast<const word_type*>(b)[i];
        for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
            x[i] = a[i] ^ b[i];
#endif
    }
    else if ((x <= a - N || x >= a) && (x <= b - N || x >= b))
    {
#if !defined(OPTIMIZE)
        for (register int i = N - 1; i >= 0; --i)
            x[i] = a[i] ^ b[i];
#else
        const size_t word_count = N / sizeof(word_type);
        for (register int i = N - 1, limit = word_count * sizeof(word_type); i >= limit; --i)
            x[i] = a[i] ^ b[i];
        for (register int i = word_count - 1; i >= 0; --i)
            reinterpret_cast<word_type*>(x)[i] = reinterpret_cast<const word_type*>(a)[i] ^ reinterpret_cast<const word_type*>(b)[i];
#endif
    }
    else
    {
        // 避免区域交叉覆盖
        uint8_t *retx = (uint8_t*) ::malloc(sizeof(uint8_t) * N);

#if !defined(OPTIMIZE)
        for (register size_t i = 0; i < N; ++i)
            retx[i] = a[i] ^ b[i];
#else
        const size_t word_count = N / sizeof(word_type);
        for (register size_t i = 0; i < word_count; ++i)
            reinterpret_cast<word_type*>(retx)[i] = reinterpret_cast<const word_type*>(a)[i] ^ reinterpret_cast<const word_type*>(b)[i];
        for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
            retx[i] = a[i] ^ b[i];
#endif

        // 回写数据
        ::memcpy(x, retx, N);
        ::free(retx);
    }
}

/**
 * 按位同或
 * x<N> = ~(a<N> ^ b<N>)
 */
inline void bit_nxor(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);

    if ((x <= a || x >= a + N) && (x <= b || x >= b + N))
    {
#if !defined(OPTIMIZE)
        for (register size_t i = 0; i < N; ++i)
            x[i] = ~(a[i] ^ b[i]);
#else
        const size_t word_count = N / sizeof(word_type);
        for (register size_t i = 0; i < word_count; ++i)
            reinterpret_cast<word_type*>(x)[i] = ~(reinterpret_cast<const word_type*>(a)[i] ^ reinterpret_cast<const word_type*>(b)[i]);
        for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
            x[i] = ~(a[i] ^ b[i]);
#endif
    }
    else if ((x <= a - N || x >= a) && (x <= b - N || x >= b))
    {
#if !defined(OPTIMIZE)
        for (register int i = N - 1; i >= 0; --i)
            x[i] = ~(a[i] ^ b[i]);
#else
        const size_t word_count = N / sizeof(word_type);
        for (register int i = N - 1, limit = word_count * sizeof(word_type); i >= limit; --i)
            x[i] = ~(a[i] ^ b[i]);
        for (register int i = word_count - 1; i >= 0; --i)
            reinterpret_cast<word_type*>(x)[i] = ~(reinterpret_cast<const word_type*>(a)[i] ^ reinterpret_cast<const word_type*>(b)[i]);
#endif
    }
    else
    {
        // 避免区域交叉覆盖
        uint8_t *retx = (uint8_t*) ::malloc(sizeof(uint8_t) * N);

#if !defined(OPTIMIZE)
        for (register size_t i = 0; i < N; ++i)
            retx[i] = ~(a[i] ^ b[i]);
#else
        const size_t word_count = N / sizeof(word_type);
        for (register size_t i = 0; i < word_count; ++i)
            reinterpret_cast<word_type*>(retx)[i] = ~(reinterpret_cast<const word_type*>(a)[i] ^ reinterpret_cast<const word_type*>(b)[i]);
        for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
            retx[i] = ~(a[i] ^ b[i]);
#endif

        // 回写数据
        ::memcpy(x, retx, N);
        ::free(retx);
    }
}

/**
 * 按位取反
 * x<N> = ~a<N>
 */
inline void bit_not(const uint8_t *a, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != x && N > 0);

    if (x < a)
    {
#if !defined(OPTIMIZE)
        for (register size_t i = 0; i < N; ++i)
            x[i] = ~a[i];
#else
        const size_t word_count = N / sizeof(word_type);
        for (register size_t i = 0; i < word_count; ++i)
            reinterpret_cast<word_type*>(x)[i] = ~reinterpret_cast<const word_type*>(a)[i];
        for (register size_t i = word_count * sizeof(word_type); i < N; ++i)
            x[i] = ~a[i];
#endif
    }
    else
    {
#if !defined(OPTIMIZE)
        for (register int i = N - 1; i >= 0; --i)
            x[i] = ~a[i];
#else
        const size_t word_count = N / sizeof(word_type);
        for (register int i = N - 1, limit = word_count * sizeof(word_type); i >= limit; --i)
            x[i] = ~a[i];
        for (register int i = word_count - 1; i >= 0; --i)
            reinterpret_cast<word_type*>(x)[i] = ~reinterpret_cast<const word_type*>(a)[i];
#endif
    }
}

inline size_t _bit_length(uint32_t a)
{
    if (a == 0)
        return 0;

    size_t ret = 31;
    if (a >> 16 == 0)
    {
        ret -= 16;
        a <<= 16;
    }
    if (a >> 24 == 0)
    {
        ret -= 8;
        a <<= 8;
    }
    if (a >> 28 == 0)
    {
        ret -= 4;
        a <<= 4;
    }
    if (a >> 30 == 0)
    {
        ret -= 2;
        a <<= 2;
    }
    ret += a >> 31;
    return ret;
}

inline size_t _bit_length(uint8_t a)
{
    if (a == 0)
        return 0;

    size_t ret = 7;
    if (a >> 4 == 0)
    {
        ret -= 4;
        a <<= 4;
    }
    if (a >> 6 == 0)
    {
        ret -= 2;
        a <<= 2;
    }
    ret += a >> 7;
    return ret;
}

inline size_t bit_length(const uint8_t *a,  size_t N)
{
    assert(NULL != a && N > 0);
#if !defined(OPTIMIZE)
    for (register int i = N - 1; i >= 0; --i)
        if (0 != a[i])
            return i * 8 + _bit_length(a[i]);
    return 0;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    for (register int i = N - 1, limit = bits32_count * sizeof(uint32_t); i >= limit; --i)
        if (0 != a[i])
            return i * 8 + _bit_length(a[i]);
    for (register int i = bits32_count - 1; i >= 0; --i)
        if (0 != reinterpret_cast<const uint32_t*>(a)[i])
            return i * 32 + _bit_length(reinterpret_cast<const uint32_t*>(a)[i]);
    return 0;
#endif
}


inline size_t _bit0_length(uint32_t a)
{
    if (a == 0xFFFFFFFF)
        return 0;

    size_t ret = 31;
    if (a >> 16 == 0xFFFF)
    {
        ret -= 16;
        a <<= 16;
    }
    if (a >> 24 == 0xFF)
    {
        ret -= 8;
        a <<= 8;
    }
    if (a >> 28 == 0x0F)
    {
        ret -= 4;
        a <<= 4;
    }
    if (a >> 30 == 0x03)
    {
        ret -= 2;
        a <<= 2;
    }
    ret += 1 - (a >> 31);
    return ret;
}

inline size_t _bit0_length(uint8_t a)
{
    if (a == 0xFF)
        return 0;

    size_t ret = 7;
    if (a >> 4 == 0x0F)
    {
        ret -= 4;
        a <<= 4;
    }
    if (a >> 6 == 0x03)
    {
        ret -= 2;
        a <<= 2;
    }
    ret += 1 - (a >> 7);
    return ret;
}

inline size_t bit0_length(const uint8_t *a,  size_t N)
{
    assert(NULL != a && N > 0);
#if !defined(OPTIMIZE)
    for (register int i = N - 1; i >= 0; --i)
        if (0xFF != a[i])
            return i * 8 + _bit_length(a[i]);
    return 0;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    for (register int i = N - 1, limit = bits32_count * sizeof(uint32_t); i >= limit; --i)
        if (0xFF != a[i])
            return i * 8 + _bit_length(a[i]);
    for (register int i = bits32_count - 1; i >= 0; --i)
        if (0xFFFFFFFF != reinterpret_cast<const uint32_t*>(a)[i])
            return i * 32 + _bit_length(reinterpret_cast<const uint32_t*>(a)[i]);
    return 0;
#endif
}

inline size_t _bit_count(uint32_t a)
{
	a = a - ((a >> 1) & 0x55555555);
	a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
	a = (a + (a >> 4)) & 0x0f0f0f0f;
	a = a + (a >> 8);
	a = a + (a >> 16);
	return a & 0x3f;
}

inline size_t _bit_count(uint8_t a)
{
    a = a - ((a >> 1) & 0x55);
	a = (a & 0x33) + ((a >> 2) & 0x33);
	a = (a + (a >> 4)) & 0x0f;
    return a & 0x0f;
}

inline size_t bit_count(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);
#if !defined(OPTIMIZE)
    size_t ret = 0;
    for (register size_t i = 0; i < N; ++i)
        ret += _bit_count(a[i]);
    return ret;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    size_t ret = 0;
    for (register size_t i = 0; i < bits32_count; ++i)
        ret += _bit_count(reinterpret_cast<const uint32_t*>(a)[i]);
    for (register size_t i = bits32_count * sizeof(uint32_t); i < N; ++i)
        ret += _bit_count(a[i]);
    return ret;
#endif
}

}

#undef OPTIMIZE
#undef hword_type
#undef word_type
#undef dword_type

#endif /* head file guarder */

