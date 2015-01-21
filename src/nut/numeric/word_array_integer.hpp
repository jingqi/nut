/**
 * @file -
 * @author jingqi
 * @date 2011-12-17
 * @last-edit 2014-11-21 22:53:19 jingqi
 * @brief
 *
 * 有符号不定长大整数：由word_type的数组来表示，字节序为little-endian，最高位为符号位
 */

#ifndef ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_
#define ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_

#include <assert.h>
#include <string.h> // for memset(), memcpy(), memmove()
#include <stdlib.h> // for malloc()

#include <nut/platform/platform.hpp>
#include <nut/platform/stdint.hpp>

// 优化程度，>= 0
#define OPTIMIZE_LEVEL 1000

namespace nut
{

/**
 * 是否为0
 *
 * @return a<N> == 0
 */
template <typename T>
inline bool is_zero(const T *a, size_t N)
{
    assert(NULL != a && N > 0);

    for (size_t i = 0; i < N; ++i)
        if (0 != a[i])
            return false;
    return true;
}

/**
 * 是否为正数或者0
 *
 * @return true, 参数 >= 0
 *      false, 参数 < 0
 */
template <typename T>
inline bool is_positive(const T *a, size_t N)
{
    assert(NULL != a && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    return 0 == (a[N - 1] & (((word_type) 1) << (8 * sizeof(word_type) - 1)));
}

/**
 * 有效字数
 *
 * @return 返回值>=1
 */
template <typename T>
size_t significant_size(const T *a, size_t N)
{
    assert(NULL != a && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    const bool positive = is_positive(a, N);
    const word_type skip_value = (positive ? 0 : ~(word_type)0);
    size_t ret = N;
    while (ret > 1 && reinterpret_cast<const word_type*>(a)[ret - 1] == skip_value && is_positive(a, ret - 1) == positive)
        --ret;
    return ret;
}

/**
 * 是否相等
 */
template <typename T>
bool equals(const T *a, size_t M, const T *b, size_t N)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    const bool positive1 = is_positive(a, M), positive2 = is_positive(b, N);
    if (positive1 != positive2)
        return false;

    const word_type fill = (positive1 ? 0 : ~(word_type)0);
    const word_type limit = (M > N ? M : N);
    for (size_t i = 0; i < limit; ++i)
        if ((i < M ? reinterpret_cast<const word_type*>(a)[i] : fill) !=
            (i < N ? reinterpret_cast<const word_type*>(b)[i] : fill))
            return false;
    return true;
}

/**
 * 小于
 *
 * @return a<M> < b<N>
 */
template <typename T>
bool less_than(const T *a, size_t M, const T *b, size_t N)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    const bool positive1 = is_positive(a, M), positive2 = is_positive(b, N);
    if (positive1 != positive2)
        return positive2;

    // 同号比较
    const word_type fill = (positive1 ? 0 : ~(word_type)0);
    for (int i = (M > N ? M : N) - 1; i >= 0; --i)
    {
        const word_type op1 = (i < (int)M ? reinterpret_cast<const word_type*>(a)[i] : fill);
        const word_type op2 = (i < (int)N ? reinterpret_cast<const word_type*>(b)[i] : fill);
        if (op1 != op2)
            return op1 < op2;
    }
    return false; // 相等
}

/**
 * 带符号扩展(或者截断)
 * a<M> -> x<N>
 */
template <typename T>
void expand(const T *a, size_t M, T *x, size_t N)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    const uint8_t fill = (is_positive(a, M) ? 0 : 0xFF); /// 先把变量算出来，避免操作数被破坏
    if (x != a)
        ::memmove(x, a, sizeof(word_type) * (M < N ? M : N));
    if (M < N)
        ::memset(x + M, fill, sizeof(word_type) * (N - M));
}

template <typename T>
void _shift_left_word(const T *a, size_t M, T *x, size_t N, size_t count)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    if (x + count == a)
    {
        return; // nothing need to do
    }
    else if (x + count < a)
    {
        const word_type fill = (is_positive(a, M) ? 0 : ~(word_type)0);
        for (size_t i = 0; i < N; ++i)
            x[i] = (i < count ? 0 : (i - count >= M ? fill : a[i - count]));
    }
    else
    {
        const word_type fill = (is_positive(a, M) ? 0 : ~(word_type)0);
        for (int i = (int) N - 1; i >= 0; --i)
            x[i] = (i < (int) count ? 0 : (i - (int) count >= (int) M ? fill : a[i - count]));
    }
}

/**
 * 左移
 * x<N> = a<N> << count
 */
template <typename T>
void shift_left(const T *a, size_t M, T *x, size_t N, size_t count)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    const int words_off = count / (8 * sizeof(word_type)), bits_off = count % (8 * sizeof(word_type));
    if (0 == bits_off)
    {
        _shift_left_word(a, M, x, N, words_off);
    }
    else if (x + words_off < a)
    {
        const int fill = (is_positive(a, M) ? 0 : ~(word_type)0);
        for (size_t i = 0; i < N; ++i)
        {
            const word_type high = (i < (size_t)words_off ? 0 : (i - words_off >= M ? fill : reinterpret_cast<const word_type*>(a)[i - words_off]))
                << bits_off;
            const word_type low = (i < (size_t)words_off + 1 ? 0 : (i - words_off - 1 >= M ? fill : reinterpret_cast<const word_type*>(a)[i - words_off - 1]))
                >> (8 * sizeof(word_type) - bits_off);
            x[i] = high | low;
        }
    }
    else
    {
        const int fill = (is_positive(a, M) ? 0 : ~(word_type)0);
        for (int i = N - 1; i >= 0; --i)
        {
            const word_type high = (i < (int) words_off ? 0 : (i - words_off >= (int)M ? fill : reinterpret_cast<const word_type*>(a)[i - words_off]))
                << bits_off;
            const word_type low = (i < (int) words_off + 1 ? 0 : (i - words_off - 1 >= (int)M ? fill : reinterpret_cast<const word_type*>(a)[i - words_off - 1]))
                >> (8 * sizeof(word_type) - bits_off);
            x[i] = high | low;
        }
    }
}

template <typename T>
void _shift_right_word(const T *a, size_t M, T *x, size_t N, size_t count)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    if (x == a + count)
    {
        return; // nothing need to do
    }
    else if (x < a + count)
    {
        const word_type fill = (is_positive(a, M) ? 0 : ~(word_type)0);
        for (size_t i = 0; i < N; ++i)
            x[i] = (i + count >= M ? fill : a[i + count]);
    }
    else
    {
        const word_type fill = (is_positive(a, M) ? 0 : ~(word_type)0);
        for (int i = N - 1; i >= 0; --i)
            x[i] = (i + count >= M ? fill : a[i + count]);
    }
}

/**
 * 右移
 * x<N> = a<N> >> count
 */
template <typename T>
void shift_right(const T *a, size_t M, T *x, size_t N, size_t count)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    const size_t words_off = count / (8 * sizeof(word_type)), bits_off = count % (8 * sizeof(word_type));
    if (0 == bits_off)
    {
        _shift_right_word(a, M, x, N, words_off);
    }
    else if (x <= a + words_off)
    {
        const word_type fill = (is_positive(a, M) ? 0 : ~(word_type)0);
        for (size_t i = 0; i < N; ++i)
        {
            const word_type high = (i + words_off + 1 >= M ? fill : reinterpret_cast<const word_type*>(a)[i + words_off + 1])
                << (8 * sizeof(word_type) - bits_off);
            const word_type low = (i + words_off >= M ? fill : reinterpret_cast<const word_type*>(a)[i + words_off])
                >> bits_off;
            x[i] = high | low;
        }
    }
    else
    {
        const word_type fill = (is_positive(a, M) ? 0 : ~(word_type)0);
        for (int i = N - 1; i >= 0; --i)
        {
            const word_type high = (i + words_off + 1 >= M ? fill : reinterpret_cast<const word_type*>(a)[i + words_off + 1])
                << (8 * sizeof(word_type) - bits_off);
            const word_type low = (i + words_off >= M ? fill : reinterpret_cast<const word_type*>(a)[i + words_off])
                >> bits_off;
            x[i] = high | low;
        }
    }
}

/**
 * 按位与
 * x<N> = a<N> & b<N>
 */
template <typename T, typename MemAlloc>
void bit_and(const T *a, const T *b, T *x, size_t N, MemAlloc *ma = NULL)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

#if (OPTIMIZE_LEVEL == 0)
    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + N) || (b < x && x < b + N))
    {
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * N);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * N);
    }

    for (size_t i = 0; i < N; ++i)
        retx[i] = a[i] & b[i];

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(word_type) * N);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
    return;
#else
    if ((x <= a || x >= a + N) && (x <= b || x >= b + N))
    {
        for (size_t i = 0; i < N; ++i)
            x[i] = a[i] & b[i];
    }
    else if ((x <= a - N || x >= a) && (x <= b - N || x >= b))
    {
        for (int i = (int) N - 1; i >= 0; --i)
            x[i] = a[i] & b[i];
    }
    else
    {
        // 避免区域交叉覆盖
        word_type *retx = NULL;
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * N);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = a[i] & b[i];

        // 回写数据
        ::memcpy(x, retx, sizeof(word_type) * N);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
#endif
}

/**
 * 按位或
 * x<N> = a<N> | b<N>
 */
template <typename T, typename MemAlloc>
void bit_or(const T *a, const T *b, T *x, size_t N, MemAlloc *ma = NULL)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

#if (OPTIMIZE_LEVEL == 0)
    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + N) || (b < x && x < b + N))
    {
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * N);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * N);
    }

    for (size_t i = 0; i < N; ++i)
        retx[i] = a[i] | b[i];

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(word_type) * N);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
    return;
#else
    if ((x <= a || x >= a + N) && (x <= b || x >= b + N))
    {
        for (size_t i = 0; i < N; ++i)
            x[i] = a[i] | b[i];
    }
    else if ((x <= a - N || x >= a) && (x <= b - N || x >= b))
    {
        for (int i = N - 1; i >= 0; --i)
            x[i] = a[i] | b[i];
    }
    else
    {
        // 避免区域交叉覆盖
        word_type *retx = NULL;
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * N);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = a[i] | b[i];

        // 回写数据
        ::memcpy(x, retx, sizeof(word_type) * N);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
#endif
}

/**
 * 按位异或
 * x<N> = a<N> ^ b<N>
 */
template <typename T, typename MemAlloc>
void bit_xor(const T *a, const T *b, T *x, size_t N, MemAlloc *ma = NULL)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

#if (OPTIMIZE_LEVEL == 0)
    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + N) || (b < x && x < b + N))
    {
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * N);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * N);
    }

    for (size_t i = 0; i < N; ++i)
        retx[i] = a[i] ^ b[i];

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(word_type) * N);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
#else
    if ((x <= a || x >= a + N) && (x <= b || x >= b + N))
    {
        for (size_t i = 0; i < N; ++i)
            x[i] = a[i] ^ b[i];
    }
    else if ((x <= a - N || x >= a) && (x <= b - N || x >= b))
    {
        for (int i = N - 1; i >= 0; --i)
            x[i] = a[i] ^ b[i];
    }
    else
    {
        // 避免区域交叉覆盖
        word_type *retx = NULL;
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * N);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = a[i] ^ b[i];

        // 回写数据
        ::memcpy(x, retx, sizeof(word_type) * N);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
#endif
}

/**
 * 按位同或
 * x<N> = ~(a<N> ^ b<N>)
 */
template <typename T, typename MemAlloc>
void bit_nxor(const T *a, const T *b, T *x, size_t N, MemAlloc *ma = NULL)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

#if (OPTIMIZE_LEVEL == 0)
    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + N) || (b < x && x < b + N))
    {
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * N);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * N);
    }

    for (size_t i = 0; i < N; ++i)
        retx[i] = ~(a[i] ^ b[i]);

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(word_type) * N);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
#else
    if ((x <= a || x >= a + N) && (x <= b || x >= b + N))
    {
        for (size_t i = 0; i < N; ++i)
            x[i] = ~(a[i] ^ b[i]);
    }
    else if ((x <= a - N || x >= a) && (x <= b - N || x >= b))
    {
        for (int i = N - 1; i >= 0; --i)
            x[i] = ~(a[i] ^ b[i]);
    }
    else
    {
        // 避免区域交叉覆盖
        word_type *retx = NULL;
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * N);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = ~(a[i] ^ b[i]);

        // 回写数据
        ::memcpy(x, retx, sizeof(word_type) * N);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
#endif
}

/**
 * 按位取反
 * x<N> = ~a<N>
 */
template <typename T, typename MemAlloc>
void bit_not(const T *a, T *x, size_t N, MemAlloc *ma = NULL)
{
    assert(NULL != a && NULL != x && N > 0);

#if (OPTIMIZE_LEVEL == 0)
    typedef typename StdInt<T>::unsigned_type word_type;
    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if (a < x && x < a + N)
    {
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * N);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * N);
    }

    for(size_t i = 0; i < N; ++i)
        retx[i] = ~a[i];

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(word_type) * N);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
#else
    (void)ma; // unused
    if (x < a)
    {
        for (size_t i = 0; i < N; ++i)
            x[i] = ~a[i];
    }
    else
    {
        for (int i = N - 1; i >= 0; --i)
            x[i] = ~a[i];
    }
#endif
}

inline size_t _bit_length(uint64_t a)
{
    if (a == 0)
        return 0;

    size_t ret = 63;
    if (a >> 32 == 0)
    {
        ret -= 32;
        a <<= 32;
    }
    if (a >> 48 == 0)
    {
        ret -= 16;
        a <<= 16;
    }
    if (a >> 56 == 0)
    {
        ret -= 8;
        a <<= 8;
    }
    if (a >> 60 == 0)
    {
        ret -= 4;
        a <<= 4;
    }
    if (a >> 62 == 0)
    {
        ret -= 2;
        a <<= 2;
    }
    ret += a >> 63;
    return ret;
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

inline size_t _bit_length(uint16_t a)
{
    if (a == 0)
        return 0;

    size_t ret = 15;
    if (a >> 8 == 0)
    {
        ret -= 8;
        a <<= 8;
    }
    if (a >> 12 == 0)
    {
        ret -= 4;
        a <<= 4;
    }
    if (a >> 14 == 0)
    {
        ret -= 2;
        a <<= 2;
    }
    ret += a >> 15;
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

/**
 * 正数 bit length
 */
inline size_t bit_length(const uint8_t *a,  size_t N)
{
    assert(NULL != a && N > 0);

#if (OPTIMIZE_LEVEL == 0)
    for (int i = N - 1; i >= 0; --i)
        if (0 != a[i])
            return i * 8 + _bit_length(a[i]);
    return 0;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    for (int i = N - 1, limit = bits32_count * sizeof(uint32_t); i >= limit; --i)
        if (0 != a[i])
            return i * 8 + _bit_length(a[i]);
    for (int i = bits32_count - 1; i >= 0; --i)
        if (0 != reinterpret_cast<const uint32_t*>(a)[i])
            return i * 32 + _bit_length(reinterpret_cast<const uint32_t*>(a)[i]);
    return 0;
#endif
}

/**
 * 负数 bit length
 */
inline size_t bit0_length(const uint8_t *a,  size_t N)
{
    assert(NULL != a && N > 0);

#if (OPTIMIZE_LEVEL == 0)
    for (int i = N - 1; i >= 0; --i)
        if (0xFF != a[i])
            return i * 8 + _bit_length((uint8_t)~a[i]);
    return 0;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    for (int i = N - 1, limit = bits32_count * sizeof(uint32_t); i >= limit; --i)
        if (0xFF != a[i])
            return i * 8 + _bit_length((uint8_t)~a[i]);
    for (int i = bits32_count - 1; i >= 0; --i)
        if (0xFFFFFFFF != reinterpret_cast<const uint32_t*>(a)[i])
            return i * 32 + _bit_length((uint32_t)~reinterpret_cast<const uint32_t*>(a)[i]);
    return 0;
#endif
}

inline size_t _bit_count(uint64_t a)
{
    a -= (a >> 1) & 0x5555555555555555LL;
    a = (a & 0x3333333333333333LL) + ((a >> 2) & 0x3333333333333333LL);
    a = (a + (a >> 4)) & 0x0f0f0f0f0f0f0f0fLL;
    a += a >> 8;
    a += a >> 16;
    a += a >> 32;
    return a & 0x7f;
}

inline size_t _bit_count(uint32_t a)
{
	a -= (a >> 1) & 0x55555555;
	a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
	a = (a + (a >> 4)) & 0x0f0f0f0f;
	a += a >> 8;
	a += a >> 16;
	return a & 0x3f;
}

inline size_t _bit_count(uint16_t a)
{
    a -= (a >> 1) & 0x5555;
    a = (a & 0x3333) + ((a >> 2) & 0x3333);
    a = (a + (a >> 4)) & 0x0f0f;
    a += (a >> 8);
    return a & 0x1f;
}

inline size_t _bit_count(uint8_t a)
{
    a -= (a >> 1) & 0x55;
	a = (a & 0x33) + ((a >> 2) & 0x33);
	a = (a + (a >> 4)) & 0x0f;
    return a & 0x0f;
}

/**
 * 统计 bit 1 数目
 */
inline size_t bit_count(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);

#if (OPTIMIZE_LEVEL == 0)
    size_t ret = 0;
    for (size_t i = 0; i < N; ++i)
        ret += _bit_count(a[i]);
    return ret;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    size_t ret = 0;
    for (size_t i = 0; i < bits32_count; ++i)
        ret += _bit_count(reinterpret_cast<const uint32_t*>(a)[i]);
    for (size_t i = bits32_count * sizeof(uint32_t); i < N; ++i)
        ret += _bit_count(a[i]);
    return ret;
#endif
}

/**
 * 统计 bit 0 数目
 */
inline size_t bit0_count(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);
    return N * 8 - bit_count(a, N);
}

inline int _lowest_bit(uint64_t a)
{
    if (a == 0)
        return -1;

    size_t ret = 1;
    if ((uint64_t)(a << 32) == 0)
    {
        ret += 32;
        a >>= 32;
    }
    if ((uint64_t)(a << 48) == 0)
    {
        ret += 16;
        a >>= 16;
    }
    if ((uint64_t)(a << 56) == 0)
    {
        ret += 8;
        a >>= 8;
    }
    if ((uint64_t)(a << 60) == 0)
    {
        ret += 4;
        a >>= 4;
    }
    if ((uint64_t)(a << 62) == 0)
    {
        ret += 2;
        a >>= 2;
    }
    ret -= a & 1;
    return ret;
}

inline int _lowest_bit(uint32_t a)
{
    if (a == 0)
        return -1;

    size_t ret = 1;
    if ((uint32_t)(a << 16) == 0)
    {
        ret += 16;
        a >>= 16;
    }
    if ((uint32_t)(a << 24) == 0)
    {
        ret += 8;
        a >>= 8;
    }
    if ((uint32_t)(a << 28) == 0)
    {
        ret += 4;
        a >>= 4;
    }
    if ((uint32_t)(a << 30) == 0)
    {
        ret += 2;
        a >>= 2;
    }
    ret -= a & 1;
    return ret;
}

inline int _lowest_bit(uint16_t a)
{
    if (a == 0)
        return -1;

    size_t ret = 1;
    if ((uint16_t)(a << 8) == 0)
    {
        ret += 8;
        a >>= 8;
    }
    if ((uint16_t)(a << 12) == 0)
    {
        ret += 4;
        a >>= 4;
    }
    if ((uint16_t)(a << 14) == 0)
    {
        ret += 2;
        a >>= 2;
    }
    ret -= a & 1;
    return ret;
}

inline int _lowest_bit(uint8_t a)
{
    if (a == 0)
        return -1;

    size_t ret = 1;
    if ((uint32_t)(a << 4) == 0)
    {
        ret += 4;
        a >>= 4;
    }
    if ((uint32_t)(a << 6) == 0)
    {
        ret += 2;
        a >>= 2;
    }
    ret -= a & 1;
    return ret;
}

/**
 * 返回从低位到高位第一个 bit 1 的位置
 *
 * @return -1 if not found
 *      >0 if found
 */
inline int lowest_bit(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);

#if (OPTIMIZE_LEVEL == 0)
    for (size_t i = 0; i < N; ++i)
        if (0 != a[i])
            return i * 8 + _lowest_bit(a[i]);
    return -1;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    for (size_t i = 0; i < bits32_count; ++i)
        if (0 != reinterpret_cast<const uint32_t*>(a)[i])
            return i * 32 + _lowest_bit(reinterpret_cast<const uint32_t*>(a)[i]);
    for (size_t i = bits32_count * sizeof(uint32_t); i < N; ++i)
        if (0 != a[i])
            return i * 8 + _lowest_bit(a[i]);
    return -1;
#endif
}

/**
 * 返回从低位到高位第一个 bit 0 的位置
 *
 * @return -1 if not found
 *      >0 if found
 */
inline int lowest_bit0(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);

#if (OPTIMIZE_LEVEL == 0)
    for (size_t i = 0; i < N; ++i)
        if (0xff != a[i])
            return i * 8 + _lowest_bit((uint8_t)~a[i]);
    return -1;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    for (size_t i = 0; i < bits32_count; ++i)
        if (0xffffffff != reinterpret_cast<const uint32_t*>(a)[i])
            return i * 32 + _lowest_bit((uint32_t)~reinterpret_cast<const uint32_t*>(a)[i]);
    for (size_t i = bits32_count * sizeof(uint32_t); i < N; ++i)
        if (0xff != a[i])
            return i * 8 + _lowest_bit((uint8_t)~a[i]);
    return -1;
#endif
}

/**
 * 相加
 * x<P> = a<M> + b<N>
 *
 * @return 进位
 */
template <typename T, typename MemAlloc>
uint8_t add(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + M) || (b < x && x < b + N))
    {
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * P);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * P);
    }

    uint8_t carry = 0;
    const word_type filla = (is_positive(a, M) ? 0 : ~(word_type)0), fillb = (is_positive(b, N) ? 0 : ~(word_type)0);
    for (size_t i = 0; i < P; ++i)
    {
        const dword_type pluser1 = (i < M ? reinterpret_cast<const word_type*>(a)[i] : filla);
        dword_type pluser2 = (i < N ? reinterpret_cast<const word_type*>(b)[i] : fillb);
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<word_type>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (8 * sizeof(word_type)));
    }

    // 回写数据
    if (retx != reinterpret_cast<word_type*>(x))
    {
        ::memcpy(x, retx, sizeof(word_type) * P);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
    return carry;
}


/**
 * 无符号数相加
 * x<P> = a<M> + b<N>
 *
 * @return 进位
 */
template <typename T, typename MemAlloc>
uint8_t unsigned_add(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + M) || (b < x && x < b + N))
    {
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * P);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * P);
    }

    uint8_t carry = 0;
    for (size_t i = 0; i < P; ++i)
    {
        const dword_type pluser1 = (i < M ? reinterpret_cast<const word_type*>(a)[i] : 0);
        dword_type pluser2 = (i < N ? reinterpret_cast<const word_type*>(b)[i] : 0);
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<word_type>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (8 * sizeof(word_type)));
    }

    // 回写数据
    if (retx != reinterpret_cast<word_type*>(x))
    {
        ::memcpy(x, retx, sizeof(word_type) * P);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
    return carry;
}

/**
 * 加1
 * x<N> += 1
 *
 * @return 进位
 */
template <typename T>
uint8_t increase(T *x, size_t N)
{
    assert(NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    uint8_t carry = 1;
    for (size_t i = 0; i < N && 0 != carry; ++i)
    {
        dword_type pluser = reinterpret_cast<const word_type*>(x)[i];
        pluser += carry;

        x[i] = static_cast<T>(pluser);
        carry = static_cast<uint8_t>(pluser >> (8 * sizeof(word_type)));
    }
    return carry;
}

/**
 * 相减
 * x<P> = a<M> - b<N>
 *
 * @return 进位
 */
template <typename T, typename MemAlloc>
uint8_t sub(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<word_type>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + M) || (b < x && x < b + N))
    {
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * P);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * P);
    }

    const word_type filla = (is_positive(a, M) ? 0 : ~(word_type)0), fillb = (is_positive(b, N) ? 0 : ~(word_type)0);
    uint8_t carry = 1;
    for (size_t i = 0; i < P; ++i)
    {
        const dword_type pluser1 = (i < M ? reinterpret_cast<const word_type*>(a)[i] : filla);
        dword_type pluser2 = static_cast<word_type>(~(i < N ? reinterpret_cast<const word_type*>(b)[i] : fillb));
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<word_type>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (8 * sizeof(word_type)));
    }

    // 回写数据
    if (retx != reinterpret_cast<word_type*>(x))
    {
        ::memcpy(x, retx, sizeof(word_type) * P);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
    return carry;
}

/**
 * 减1
 * x<N> -= 1
 *
 * @return 进位
 */
template <typename T>
uint8_t decrease(T *x, size_t N)
{
    assert(NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    uint8_t carry = 0;
    for (size_t i = 0; i < N && 1 != carry; ++i)
    {
        dword_type pluser = reinterpret_cast<const word_type*>(x)[i];
        pluser += carry + (dword_type)(word_type)~(word_type)0;

        x[i] = static_cast<T>(pluser);
        carry = static_cast<uint8_t>(pluser >> (8 * sizeof(word_type)));
    }
    return carry;
}

/**
 * 取相反数
 * x<N> = -a<N>
 *
 * @return 进位
 */
template <typename T, typename MemAlloc>
uint8_t negate(const T *a, size_t M, T *x, size_t N, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if (a < x && x < a + M)
    {
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * N);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * N);
    }

    uint8_t carry = 1;
    const word_type fill = (is_positive(a, M) ? 0 : ~(word_type)0);
    for (size_t i = 0; i < N; ++i)
    {
        dword_type pluser = static_cast<word_type>(~(i < M ? reinterpret_cast<const word_type*>(a)[i] : fill));
        pluser += carry;

        retx[i] = static_cast<word_type>(pluser);
        carry = static_cast<uint8_t>(pluser >> (8 * sizeof(word_type)));
    }

    // 回写数据
    if (retx != reinterpret_cast<word_type*>(x))
    {
        ::memcpy(x, retx, N);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
    return carry;
}

/**
 * 正数平方优化
 *
 *              a  b  c  d  e
 *           *  a  b  c  d  e
 *         -------------------
 *             ae be ce de ee
 *          ad bd cd dd de
 *       ac bc cc cd ce
 *    ab bb bc bd be
 * aa ab ac ad ae
 *
 * 含有重复的
 *             ae be ce de
 *          ad bd cd
 *       ac bc
 *    ab
 *             +
 *                      de
 *                cd ce
 *          bc bd be
 *    ab ac ad ae
 */
template <typename T, typename MemAlloc>
void _square(const T *a, size_t M, T *x, size_t N, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    assert(is_positive(a, M));
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if (a - N < x && x < a + M)
    {
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * N);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * N);
    }

    // 先计算一半
    ::memset(retx, 0, sizeof(word_type) * N);
    for (size_t i = 0; i < M - 1; ++i)
    {
        if (i * 2 + 1 >= N)
            break;

        const dword_type op1 = reinterpret_cast<const word_type*>(a)[i];
        if (0 == op1)
            continue;

        word_type carry = 0;
        for (size_t j = i + 1; j < M && i + j < N; ++j)
        {
            dword_type op2 = reinterpret_cast<const word_type*>(a)[j];
            op2 = op1 * op2 + retx[i + j] + carry;

            retx[i + j] = static_cast<word_type>(op2);
            carry = static_cast<word_type>(op2 >> (8 * sizeof(word_type)));
        }
        if (i + M < N)
            retx[i + M] = carry;
    }

    // 再加上另一半
    const size_t limit = (N < M * 2 ? N : M * 2);
    shift_left(retx, limit, retx, limit, 1);

    // 加上中间对称线
    word_type carry = 0;
    for (size_t i = 0; i < M; ++i)
    {
        if (i * 2 >= N)
            break;

        dword_type op = reinterpret_cast<const word_type*>(a)[i];
        op = op * op + retx[i * 2] + carry;

        retx[i * 2] = static_cast<word_type>(op);
        carry = static_cast<word_type>(op >> (8 * sizeof(word_type)));

        if (0 != carry && i * 2 + 1 < N)
        {
            op = retx[i * 2 + 1];
            op += carry;

            retx[i * 2 + 1] = static_cast<word_type>(op);
            carry = static_cast<word_type>(op >> (8 * sizeof(word_type)));
        }
    }

    // 回写结果
    if (retx != reinterpret_cast<word_type*>(x))
    {
        ::memcpy(x, retx, sizeof(word_type) * N);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
}

/**
 * 相乘
 * x<P> = a<M> * b<N>
 */
template <typename T, typename MemAlloc>
void multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    if (a == b && M == N && is_positive(a, M))
    {
        _square(a, M, x, P, ma);
        return;
    }

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a - P < x && x < a + M) || (b - P < x && x < b + N))
    {
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * P);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * P);
    }

    // 乘法
    const word_type filla = (is_positive(a,M) ? 0 : ~(word_type)0), fillb = (is_positive(b,N) ? 0 : ~(word_type)0); /// 先把变量算出来，避免操作数被破坏
    ::memset(retx, 0, sizeof(word_type) * P);
    for (size_t i = 0; i < P; ++i)
    {
        if (i >= M && 0 == filla)
            break;

        const dword_type mult1 = (i < M ? reinterpret_cast<const word_type*>(a)[i] : filla);
        if (mult1 == 0)
            continue;

        word_type carry = 0; // 这个进位包括乘法的，故此会大于1
        for (size_t j = 0; i + j < P; ++j)
        {
            if (j >= N && 0 == fillb && 0 == carry)
                break;

            dword_type mult2 = (j < N ? reinterpret_cast<const word_type*>(b)[j] : fillb);
            mult2 = mult1 * mult2 + retx[i + j] + carry;

            retx[i + j] = static_cast<word_type>(mult2);
            carry = static_cast<word_type>(mult2 >> (8 * sizeof(word_type)));
        }
    }

    // 回写数据
    if (retx != reinterpret_cast<word_type*>(x))
    {
        ::memcpy(x, retx, sizeof(word_type) * P);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
}


/**
 * 无符号数相乘
 * x<P> = a<M> * b<N>
 */
template <typename T, typename MemAlloc>
void unsigned_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a - P < x && x < a + M) || (b - P < x && x < b + N))
    {
        if (NULL != ma)
            retx = (word_type*) ma->alloc(sizeof(word_type) * P);
        else
            retx = (word_type*) ::malloc(sizeof(word_type) * P);
    }

    // 乘法
    ::memset(retx, 0, sizeof(word_type) * P);
    for (size_t i = 0; i < P; ++i)
    {
        if (i >= M)
            break;

        const dword_type mult1 = reinterpret_cast<const word_type*>(a)[i];
        if (mult1 == 0)
            continue;

        word_type carry = 0; // 这个进位包括乘法的，故此会大于1
        for (size_t j = 0; i + j < P; ++j)
        {
            if (j >= N && 0 == carry)
                break;

            dword_type mult2 = (j < N ? reinterpret_cast<const word_type*>(b)[j] : 0);
            mult2 = mult1 * mult2 + retx[i + j] + carry;

            retx[i + j] = static_cast<word_type>(mult2);
            carry = static_cast<word_type>(mult2 >> (8 * sizeof(word_type)));
        }
    }

    // 回写数据
    if (retx != reinterpret_cast<word_type*>(x))
    {
        ::memcpy(x, retx, sizeof(word_type) * P);
        if (NULL != ma)
            ma->free(retx);
        else
            ::free(retx);
    }
}

/**
 * karatsuba 乘法，时间复杂度为 O(n^1.59)
 *
 * a = A * base + B (其中 base = 2 ^ (n/2) )
 * b = C * base + D
 * a * b = AC * (base^2) + ((A-B)(D-C) + AC + BD) * base + BD
 * 只需要做 3 次 n/2 规模的乘法，以及一些加减法
 */
template <typename T, typename MemAlloc>
void karatsuba_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    // 规模较小时使用一般算法
    if (M < 5 || N < 5 || P < 5)
    {
        multiply(a, M, b, N, x, P, ma);
        return;
    }

    // 改算法不能处理负数的补数
    T *aa = const_cast<T*>(a), *bb = const_cast<T*>(b);
    size_t MM = M, NN = N;
    bool neg = false;
    if (!is_positive(a, M))
    {
        ++MM;
        if (NULL != ma)
            aa = (T*) ma->alloc(sizeof(T) * MM);
        else
            aa = (T*) ::malloc(sizeof(T) * MM);
        negate(a, M, aa, MM, ma);
        neg = !neg;
    }
    if (!is_positive(b, N))
    {
        ++NN;
        if (NULL != ma)
            bb = (T*) ma->alloc(sizeof(T) * NN);
        else
            bb = (T*) ::malloc(sizeof(T) * NN);
        negate(b, N, bb, NN, ma);
        neg = !neg;
    }

    // 准备变量 A、B、C、D
    const size_t base_len = ((MM > NN ? MM : NN) + 1) / 2;
    const T ZERO = 0;
    const T *A = a + base_len;
    size_t alen = M - base_len;
    if (M <= base_len)
    {
        A = &ZERO;
        alen = 1;
    }

    T *B = const_cast<T*>(a);
    size_t blen = base_len;
    if (!is_positive(B, blen))
    {
        ++blen;
        if (NULL != ma)
            B = (T*) ma->alloc(sizeof(T) * blen);
        else
            B = (T*) ::malloc(sizeof(T) * blen);
        ::memcpy(B, a, sizeof(T) * base_len);
        B[base_len] = 0;
    }

    const T *C = b + base_len;
    size_t clen = N - base_len;
    if (N <= base_len)
    {
        C = &ZERO;
        clen = 1;
    }

    T *D = const_cast<T*>(b);
    size_t dlen = base_len;
    if (!is_positive(D, dlen))
    {
        ++dlen;
        if (NULL != ma)
            D = (T*) ma->alloc(sizeof(T) * dlen);
        else
            D = (T*) ::malloc(sizeof(T) * dlen);
        ::memcpy(D, b, sizeof(T) * base_len);
        D[base_len] = 0;
    }

    T *AC = NULL;
    if (NULL != ma)
        AC = (T*) ma->alloc(sizeof(T) * (base_len * 2));
    else
        AC = (T*) ::malloc(sizeof(T) * (base_len * 2));
    karatsuba_multiply(A, alen, C, clen, AC, base_len * 2, ma);

    T *BD = NULL;
    if (NULL != ma)
        BD = (T*) ma->alloc(sizeof(T) * (base_len * 2 + 1));
    else
        BD = (T*) ::malloc(sizeof(T) * (base_len * 2 + 1));
    karatsuba_multiply(B, blen, D, dlen, BD, base_len * 2 + 1, ma);

    T *D_C = NULL;
    if (NULL != ma)
        D_C = (T*) ma->alloc(sizeof(T) * base_len + 1);
    else
        D_C = (T*) ::malloc(sizeof(T) * base_len + 1);
    sub(D, dlen, C, clen, D_C, base_len + 1, ma);
    if (D != b)
    {
        if (NULL != ma)
            ma->free(D);
        else
            ::free(D);
    }
    D = NULL;
    if (bb != b)
    {
        if (NULL != ma)
            ma->free(bb);
        else
            ::free(bb);
    }
    bb = NULL;

    T *ABCD = NULL;
    if (NULL != ma)
        ABCD = (T*) ma->alloc(sizeof(T) * (base_len * 2 + 1));
    else
        ABCD = (T*) ::malloc(sizeof(T) * (base_len * 2 + 1));
    sub(A, alen, B, blen, ABCD, base_len + 1, ma);
    if (B != a)
    {
        if (NULL != ma)
            ma->free(B);
        else
            ::free(B);
    }
    B = NULL;
    if (aa != a)
    {
        if (NULL != ma)
            ma->free(aa);
        else
            ::free(aa);
    }
    aa = NULL;

    karatsuba_multiply(ABCD, base_len + 1, D_C, base_len + 1, ABCD, base_len * 2 + 1, ma);
    if (NULL != ma)
        ma->free(D_C);
    else
        ::free(D_C);
    D_C = NULL;

    add(ABCD, base_len * 2 + 1, AC, base_len * 2, ABCD, base_len * 2 + 1, ma);
    add(ABCD, base_len * 2 + 1, BD, base_len * 2 + 1, ABCD, base_len * 2 + 1, ma);

    // 并生成结果
    expand(BD, base_len * 2 + 1, x, P);
    if (P > base_len)
        add(x + base_len, P - base_len, ABCD, base_len * 2 + 1, x + base_len, P - base_len, ma);
    if (P > base_len * 2)
        add(x + base_len * 2, P - base_len * 2, AC, base_len * 2, x + base_len * 2, P - base_len * 2, ma);

    // 回收内存
    if (NULL != ma)
    {
        ma->free(AC);
        ma->free(BD);
        ma->free(ABCD);
    }
    else
    {
        ::free(AC);
        ::free(BD);
        ::free(ABCD);
    }
}

/**
 * 相除
 * x<P> = a<M> / b<N>
 * y<Q> = a<M> % b<N>
 *
 * @param x
 *      商
 * @param y
 *      余数
 */
template <typename T, typename MemAlloc>
void divide(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, T *y, size_t Q, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);
    assert((NULL != x && P > 0) || (NULL != y && Q > 0));
    assert(NULL == x || P == 0 || NULL == y || Q == 0 || y >= x + P || x >= y + Q); // 避免区域交叉覆盖
    assert(!is_zero(b, N)); // 被除数不能为0

    typedef typename StdInt<T>::unsigned_type word_type;

    // 常量
    const size_t dividend_len = significant_size(a, M);
    const size_t divisor_len = significant_size(b, N);
    const bool dividend_positive = is_positive(a, M); /// 先把变量算出来，避免操作数被破坏
    const bool divisor_positive = is_positive(b, N);
    const size_t quotient_len = (P < dividend_len ? P : dividend_len);

    // 避免数据在计算中途被破坏
    word_type *quotient = reinterpret_cast<word_type*>(x); // 商，可以为 NULL
    if ((a - P < x && x < a) || (b - P < x && x < b + N)) // 兼容 x==a 的情况
    {
        if (NULL != ma)
            quotient = (word_type*) ma->alloc(sizeof(word_type) * quotient_len);
        else
            quotient = (word_type*) ::malloc(sizeof(word_type) * quotient_len);
    }
    word_type *remainder = reinterpret_cast<word_type*>(y); // 余数，不能为 NULL
    if (NULL == y || Q < divisor_len || (a - Q < y && y < a + M) || (b - Q < y && y < b + N))
    {
        if (NULL != ma)
            remainder = (word_type*) ma->alloc(sizeof(word_type) * divisor_len);
        else
            remainder = (word_type*) ::malloc(sizeof(word_type) * divisor_len);
    }

    // 逐位试商
    ::memset(remainder, (dividend_positive ? 0 : 0xFF), sizeof(word_type) * divisor_len); // 初始化余数
    bool remainder_positive = dividend_positive; // 余数的符号
    bool quotient_positive = true; // 商的符号
    for (size_t i = 0; i < dividend_len; ++i)
    {
        const size_t dividend_word_pos = dividend_len - i - 1;
        const word_type next_dividend_word = reinterpret_cast<const word_type*>(a)[dividend_word_pos]; // 余数左移时的低位补位部分
        if (NULL != quotient && dividend_word_pos < P)
            quotient[dividend_word_pos] = 0; // 初始化商，注意，兼容 x==a 的情况

        for (size_t j = 0; j < 8 * sizeof(word_type); ++j)
        {
            // 余数左移1位
            shift_left(remainder, divisor_len, remainder, divisor_len, 1);
            remainder[0] |= (next_dividend_word >> (8 * sizeof(word_type) - 1 - j)) & 0x01;

            // 加上/减去除数
            if (remainder_positive == divisor_positive)
                sub(remainder, divisor_len, reinterpret_cast<const word_type*>(b), divisor_len, remainder, divisor_len, ma);
            else
                add(remainder, divisor_len, reinterpret_cast<const word_type*>(b), divisor_len, remainder, divisor_len, ma);

            // 试商结果
            remainder_positive = is_positive(remainder, divisor_len);
            if (remainder_positive == divisor_positive)
            {
                if (NULL != quotient && dividend_word_pos < P)
                    quotient[dividend_word_pos] |= (1 << (8 * sizeof(word_type) - 1 - j));
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
        expand(quotient, quotient_len, reinterpret_cast<word_type*>(x), P);
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
                add(remainder, divisor_len, reinterpret_cast<const word_type*>(b), divisor_len, remainder, divisor_len, ma);
            else
                sub(remainder, divisor_len, reinterpret_cast<const word_type*>(b), divisor_len, remainder, divisor_len, ma);
        }
        expand(remainder, divisor_len, reinterpret_cast<word_type*>(y), Q);
    }

    // 释放空间
    if (quotient != reinterpret_cast<word_type*>(x))
    {
        if (NULL != ma)
            ma->free(quotient);
        else
            ::free(quotient);
    }
    if (remainder != reinterpret_cast<word_type*>(y))
    {
        if (NULL != ma)
            ma->free(remainder);
        else
            ::free(remainder);
    }
}

}

#undef OPTIMIZE_LEVEL

#endif /* head file guarder */
