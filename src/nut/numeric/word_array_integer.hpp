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
#include <algorithm>

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
bool is_zero(const T *a, size_t N)
{
    assert(NULL != a && N > 0);

    for (size_t i = 0; i < N; ++i)
    {
        if (0 != a[i])
            return false;
    }
    return true;
}

/**
 * 是否为正数或者0
 *
 * @return true, 参数 >= 0
 *      false, 参数 < 0
 */
template <typename T>
bool is_positive(const T *a, size_t N)
{
    assert(NULL != a && N > 0);
    return 0 == (a[N - 1] & (((T) 1) << (8 * sizeof(T) - 1)));
}

/**
 * 是否为负数
 *
 * @return true, 参数 < 0
 *      false, 参数 >= 0
 */
template <typename T>
bool is_negative(const T *a, size_t N)
{
    return !is_positive(a, N);
}

/**
 * (有符号数)有效字数
 *
 * @return 返回值>=1
 */
template <typename T>
size_t signed_significant_size(const T *a, size_t N)
{
    assert(NULL != a && N > 0);

    const bool positive = is_positive(a, N);
    const T skip_value = (positive ? 0 : ~(T)0);
    size_t ret = N;
    while (ret > 1 && a[ret - 1] == skip_value &&
            is_positive(a, ret - 1) == positive)
        --ret;
    return ret;
}

/**
 * (无符号数)有效字数
 *
 * @return 返回值>=1
 */
template <typename T>
size_t unsigned_significant_size(const T *a, size_t N)
{
    assert(NULL != a && N > 0);

    size_t ret = N;
    while (ret > 1 && a[ret - 1] == 0)
        --ret;
    return ret;
}

/**
 * (有符号数)是否相等
 */
template <typename T>
bool signed_equals(const T *a, size_t M, const T *b, size_t N)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);

    const bool positive1 = is_positive(a, M), positive2 = is_positive(b, N);
    if (positive1 != positive2)
        return false;

    const T fill = (positive1 ? 0 : ~(T)0);
    const size_t limit = (std::max)(M, N);
    for (size_t i = 0; i < limit; ++i)
    {
        if ((i < M ? a[i] : fill) != (i < N ? b[i] : fill))
            return false;
    }
    return true;
}

/**
 * (无符号数)是否相等
 */
template <typename T>
bool unsigned_equals(const T *a, size_t M, const T*b, size_t N)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);

    const size_t limit = (std::max)(M, N);
    for (size_t i = 0; i < limit; ++i)
    {
        if ((i < M ? a[i] : 0) != (i < N ? b[i] : 0))
            return false;
    }
    return true;
}

/**
 * (有符号数)小于
 *
 * @return a<M> < b<N>
 */
template <typename T>
bool signed_less_than(const T *a, size_t M, const T *b, size_t N)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    const bool positive1 = is_positive(a, M), positive2 = is_positive(b, N);
    if (positive1 != positive2)
        return positive2;

    // 同号比较
    const word_type fill = (positive1 ? 0 : ~(word_type)0);
    for (int i = (std::max)(M, N) - 1; i >= 0; --i)
    {
        const word_type op1 = (i < (int)M ? reinterpret_cast<const word_type*>(a)[i] : fill);
        const word_type op2 = (i < (int)N ? reinterpret_cast<const word_type*>(b)[i] : fill);
        if (op1 != op2)
            return op1 < op2;
    }
    return false; // 相等
}

/**
 * (无符号数)小于
 *
 * @return a<M> < b<N>
 */
template <typename T>
bool unsigned_less_than(const T *a, size_t M, const T *b, size_t N)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    for (int i = (std::max)(M, N) - 1; i >= 0; --i)
    {
        const word_type op1 = (i < (int)M ? reinterpret_cast<const word_type*>(a)[i] : 0);
        const word_type op2 = (i < (int)N ? reinterpret_cast<const word_type*>(b)[i] : 0);
        if (op1 != op2)
            return op1 < op2;
    }
    return false; // 相等
}

/**
 * (有符号数)带符号扩展(或者截断)
 * a<M> -> x<N>
 */
template <typename T>
void signed_expand(const T *a, size_t M, T *x, size_t N)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);

    const int fill = (is_positive(a, M) ? 0 : 0xFF); /// 先把变量算出来，避免操作数被破坏
    if (x != a)
        ::memmove(x, a, sizeof(T) * (std::min)(M, N));
    if (M < N)
        ::memset(x + M, fill, sizeof(T) * (N - M));
}

/**
 * (无符号数)无符号扩展(或者截断)
 * a<M> -> x<N>
 */
template <typename T>
void unsigned_expand(const T *a, size_t M, T *x, size_t N)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);

    if (x != a)
        ::memmove(x, a, sizeof(T) * (std::min)(M, N));
    if (M < N)
        ::memset(x + M, 0, sizeof(T) * (N - M));
}

template <typename T>
void _signed_shift_left_word(const T *a, size_t M, T *x, size_t N, size_t word_count)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);

    if (x + word_count == a)
    {
        ::memset(x, 0, sizeof(T) * word_count);
        if (x + N > a + M)
        {
            const int fill = (is_positive(a, M) ? 0 : 0xFF);
            ::memset(x + word_count + M, fill, sizeof(T) * (N - M - word_count));
        }
    }
    else if (x + word_count < a)
    {
        const T fill = (is_positive(a, M) ? 0 : ~(T)0);
        for (size_t i = 0; i < N; ++i)
            x[i] = (i < word_count ? 0 : (i - word_count >= M ? fill : a[i - word_count]));
    }
    else
    {
        const T fill = (is_positive(a, M) ? 0 : ~(T)0);
        for (int i = (int) N - 1; i >= 0; --i)
            x[i] = (i < (int) word_count ? 0 : (i - (int) word_count >= (int) M ? fill : a[i - (int) word_count]));
    }
}

/**
 * (有符号数)左移
 * x<N> = a<N> << count
 */
template <typename T>
void signed_shift_left(const T *a, size_t M, T *x, size_t N, size_t bit_count)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    const size_t words_off = bit_count / (8 * sizeof(word_type)), bits_off = bit_count % (8 * sizeof(word_type));
    if (0 == bits_off)
    {
        _signed_shift_left_word(a, M, x, N, words_off);
    }
    else if (x + words_off < a)
    {
        const word_type fill = (is_positive(a, M) ? 0 : ~(word_type)0);
        for (size_t i = 0; i < N; ++i)
        {
            const word_type high = (i < words_off ? 0 : (i - words_off >= M ? fill :
                    reinterpret_cast<const word_type*>(a)[i - words_off])) << bits_off;
            const word_type low = (i < words_off + 1 ? 0 : (i - words_off - 1 >= M ? fill :
                    reinterpret_cast<const word_type*>(a)[i - words_off - 1])) >> (8 * sizeof(word_type) - bits_off);
            x[i] = high | low;
        }
    }
    else
    {
        const word_type fill = (is_positive(a, M) ? 0 : ~(word_type)0);
        for (int i = N - 1; i >= 0; --i)
        {
            const word_type high = (i < (int) words_off ? 0 : (i - (int) words_off >= (int) M ? fill :
                    reinterpret_cast<const word_type*>(a)[i - (int) words_off])) << bits_off;
            const word_type low = (i < (int) words_off + 1 ? 0 : (i - (int) words_off - 1 >= (int) M ? fill :
                    reinterpret_cast<const word_type*>(a)[i - (int) words_off - 1])) >> (8 * sizeof(word_type) - bits_off);
            x[i] = high | low;
        }
    }
}

template <typename T>
void _unsigned_shift_left_word(const T *a, size_t M, T *x, size_t N, size_t word_count)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);

    if (x + word_count == a)
    {
        ::memset(x, 0, sizeof(T) * word_count);
        if (x + N > a + M)
            ::memset(x + word_count + M, 0, sizeof(T) * (N - M - word_count));
    }
    else if (x + word_count < a)
    {
        for (size_t i = 0; i < N; ++i)
            x[i] = (i < word_count ? 0 : (i - word_count >= M ? 0 : a[i - word_count]));
    }
    else
    {
        for (int i = (int) N - 1; i >= 0; --i)
            x[i] = (i < (int) word_count ? 0 : (i - (int) word_count >= (int) M ? 0 : a[i - (int) word_count]));
    }
}

/**
 * (无符号数)左移
 * x<N> = a<N> << count
 */
template <typename T>
void unsigned_shift_left(const T *a, size_t M, T *x, size_t N, size_t bit_count)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    const size_t words_off = bit_count / (8 * sizeof(word_type)), bits_off = bit_count % (8 * sizeof(word_type));
    if (0 == bits_off)
    {
        _unsigned_shift_left_word(a, M, x, N, words_off);
    }
    else if (x + words_off < a)
    {
        for (size_t i = 0; i < N; ++i)
        {
            const word_type high = (i < words_off ? 0 : (i - words_off >= M ? 0 :
                    reinterpret_cast<const word_type*>(a)[i - words_off])) << bits_off;
            const word_type low = (i < words_off + 1 ? 0 : (i - words_off - 1 >= M ? 0 :
                    reinterpret_cast<const word_type*>(a)[i - words_off - 1])) >> (8 * sizeof(word_type) - bits_off);
            x[i] = high | low;
        }
    }
    else
    {
        for (int i = N - 1; i >= 0; --i)
        {
            const word_type high = (i < (int) words_off ? 0 : (i - (int) words_off >= (int) M ? 0 :
                    reinterpret_cast<const word_type*>(a)[i - (int) words_off])) << bits_off;
            const word_type low = (i < (int) words_off + 1 ? 0 : (i - (int) words_off - 1 >= (int) M ? 0 :
                    reinterpret_cast<const word_type*>(a)[i - (int) words_off - 1])) >> (8 * sizeof(word_type) - bits_off);
            x[i] = high | low;
        }
    }
}

template <typename T>
void _signed_shift_right_word(const T *a, size_t M, T *x, size_t N, size_t word_count)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);

    if (x == a + word_count)
    {
        if (x + N > a + M)
        {
            const int fill = (is_positive(a, M) ? 0 : 0xFF);
            ::memset(x - word_count + M, fill, sizeof(T) * (word_count + N - M));
        }
    }
    else if (x < a + word_count)
    {
        const T fill = (is_positive(a, M) ? 0 : ~(T)0);
        for (size_t i = 0; i < N; ++i)
            x[i] = (i + word_count >= M ? fill : a[i + word_count]);
    }
    else
    {
        const T fill = (is_positive(a, M) ? 0 : ~(T)0);
        for (int i = N - 1; i >= 0; --i)
            x[i] = (i + (int) word_count >= (int) M ? fill : a[i + (int) word_count]);
    }
}

/**
 * (有符号数)右移
 * x<N> = a<N> >> count
 */
template <typename T>
void signed_shift_right(const T *a, size_t M, T *x, size_t N, size_t bit_count)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    const size_t words_off = bit_count / (8 * sizeof(word_type)), bits_off = bit_count % (8 * sizeof(word_type));
    if (0 == bits_off)
    {
        _signed_shift_right_word(a, M, x, N, words_off);
    }
    else if (x <= a + words_off)
    {
        const word_type fill = (is_positive(a, M) ? 0 : ~(word_type)0);
        for (size_t i = 0; i < N; ++i)
        {
            const word_type high = (i + words_off + 1 >= M ? fill :
                    reinterpret_cast<const word_type*>(a)[i + words_off + 1]) << (8 * sizeof(word_type) - bits_off);
            const word_type low = (i + words_off >= M ? fill :
                    reinterpret_cast<const word_type*>(a)[i + words_off]) >> bits_off;
            x[i] = high | low;
        }
    }
    else
    {
        const word_type fill = (is_positive(a, M) ? 0 : ~(word_type)0);
        for (int i = N - 1; i >= 0; --i)
        {
            const word_type high = (i + (int) words_off + 1 >= (int) M ? fill :
                    reinterpret_cast<const word_type*>(a)[i + (int) words_off + 1]) << (8 * sizeof(word_type) - bits_off);
            const word_type low = (i + (int) words_off >= (int) M ? fill :
                    reinterpret_cast<const word_type*>(a)[i + (int) words_off]) >> bits_off;
            x[i] = high | low;
        }
    }
}

template <typename T>
void _unsigned_shift_right_word(const T *a, size_t M, T *x, size_t N, size_t word_count)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);

    if (x == a + word_count)
    {
        if (x + N > a + M)
            ::memset(x - word_count + M, 0, sizeof(T) * (word_count + N - M));
    }
    else if (x < a + word_count)
    {
        for (size_t i = 0; i < N; ++i)
            x[i] = (i + word_count >= M ? 0 : a[i + word_count]);
    }
    else
    {
        for (int i = N - 1; i >= 0; --i)
            x[i] = (i + (int) word_count >= M ? 0 : a[i + (int) word_count]);
    }
}

/**
 * (无符号数)右移
 * x<N> = a<N> >> count
 */
template <typename T>
void unsigned_shift_right(const T *a, size_t M, T *x, size_t N, size_t bit_count)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;

    const size_t words_off = bit_count / (8 * sizeof(word_type)), bits_off = bit_count % (8 * sizeof(word_type));
    if (0 == bits_off)
    {
        _unsigned_shift_right_word(a, M, x, N, words_off);
    }
    else if (x <= a + words_off)
    {
        for (size_t i = 0; i < N; ++i)
        {
            const word_type high = (i + words_off + 1 >= M ? 0 :
                    reinterpret_cast<const word_type*>(a)[i + words_off + 1]) << (8 * sizeof(word_type) - bits_off);
            const word_type low = (i + words_off >= M ? 0 :
                    reinterpret_cast<const word_type*>(a)[i + words_off]) >> bits_off;
            x[i] = high | low;
        }
    }
    else
    {
        for (int i = N - 1; i >= 0; --i)
        {
            const word_type high = (i + (int) words_off + 1 >= (int) M ? 0 :
                    reinterpret_cast<const word_type*>(a)[i + (int) words_off + 1]) << (8 * sizeof(word_type) - bits_off);
            const word_type low = (i + (int) words_off >= (int) M ? 0 :
                    reinterpret_cast<const word_type*>(a)[i + (int) words_off]) >> bits_off;
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

#if (OPTIMIZE_LEVEL == 0)
    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (T*) ma_alloc(ma, sizeof(T) * N);

    for (size_t i = 0; i < N; ++i)
        retx[i] = a[i] & b[i];

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(T) * N);
        ma_free(ma, retx);
    }
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
        T *const retx = (T*) ma_alloc(ma, sizeof(T) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = a[i] & b[i];

        // 回写数据
        ::memcpy(x, retx, sizeof(T) * N);
        ma_free(ma, retx);
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

#if (OPTIMIZE_LEVEL == 0)
    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (T*) ma_alloc(ma, sizeof(T) * N);

    for (size_t i = 0; i < N; ++i)
        retx[i] = a[i] | b[i];

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(T) * N);
        ma_free(ma, retx);
    }
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
        T *const retx = (T*) ma_alloc(ma, sizeof(T) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = a[i] | b[i];

        // 回写数据
        ::memcpy(x, retx, sizeof(T) * N);
        ma_free(ma, retx);
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

#if (OPTIMIZE_LEVEL == 0)
    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (T*) ma_alloc(ma, sizeof(T) * N);

    for (size_t i = 0; i < N; ++i)
        retx[i] = a[i] ^ b[i];

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(T) * N);
        ma_free(ma, retx);
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
        T *const retx = (T*) ma_alloc(ma, sizeof(T) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = a[i] ^ b[i];

        // 回写数据
        ::memcpy(x, retx, sizeof(T) * N);
        ma_free(ma, retx);
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

#if (OPTIMIZE_LEVEL == 0)
    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (T*) ma_alloc(ma, sizeof(T) * N);

    for (size_t i = 0; i < N; ++i)
        retx[i] = ~(a[i] ^ b[i]);

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(T) * N);
        ma_free(ma, retx);
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
        T *const retx = (T*) ma_alloc(ma, sizeof(T) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = ~(a[i] ^ b[i]);

        // 回写数据
        ::memcpy(x, retx, sizeof(T) * N);
        ma_free(ma, retx);
    }
#endif
}

/**
 * 按位取反
 * x<N> = ~a<N>
 */
template <typename T, typename MemAlloc>
void bit_not(const T *a, T *x, size_t N)
{
    assert(NULL != a && NULL != x && N > 0);

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
    {
        if (0 != a[i])
            return i * 8 + _bit_length(a[i]);
    }
    return 0;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    for (int i = N - 1, limit = bits32_count * sizeof(uint32_t); i >= limit; --i)
    {
        if (0 != a[i])
            return i * 8 + _bit_length(a[i]);
    }
    for (int i = bits32_count - 1; i >= 0; --i)
    {
        if (0 != reinterpret_cast<const uint32_t*>(a)[i])
            return i * 32 + _bit_length(reinterpret_cast<const uint32_t*>(a)[i]);
    }
    return 0;
#endif
}

/**
 * 负数 bit length
 */
inline size_t bit0_length(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);

#if (OPTIMIZE_LEVEL == 0)
    for (int i = N - 1; i >= 0; --i)
    {
        if (0xFF != a[i])
            return i * 8 + _bit_length((uint8_t)~a[i]);
    }
    return 0;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    for (int i = N - 1, limit = bits32_count * sizeof(uint32_t); i >= limit; --i)
    {
        if (0xFF != a[i])
            return i * 8 + _bit_length((uint8_t)~a[i]);
    }
    for (int i = bits32_count - 1; i >= 0; --i)
    {
        if (0xFFFFFFFF != reinterpret_cast<const uint32_t*>(a)[i])
            return i * 32 + _bit_length((uint32_t)~reinterpret_cast<const uint32_t*>(a)[i]);
    }
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
    {
        if (0 != a[i])
            return i * 8 + _lowest_bit(a[i]);
    }
    return -1;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    for (size_t i = 0; i < bits32_count; ++i)
    {
        if (0 != reinterpret_cast<const uint32_t*>(a)[i])
            return i * 32 + _lowest_bit(reinterpret_cast<const uint32_t*>(a)[i]);
    }
    for (size_t i = bits32_count * sizeof(uint32_t); i < N; ++i)
    {
        if (0 != a[i])
            return i * 8 + _lowest_bit(a[i]);
    }
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
    {
        if (0xff != a[i])
            return i * 8 + _lowest_bit((uint8_t)~a[i]);
    }
    return -1;
#else
    const size_t bits32_count = N / sizeof(uint32_t);
    for (size_t i = 0; i < bits32_count; ++i)
    {
        if (0xffffffff != reinterpret_cast<const uint32_t*>(a)[i])
            return i * 32 + _lowest_bit((uint32_t)~reinterpret_cast<const uint32_t*>(a)[i]);
    }
    for (size_t i = bits32_count * sizeof(uint32_t); i < N; ++i)
    {
        if (0xff != a[i])
            return i * 8 + _lowest_bit((uint8_t)~a[i]);
    }
    return -1;
#endif
}

/**
 * (有符号数)相加
 * x<P> = a<M> + b<N>
 *
 * @return 进位
 */
template <typename T, typename MemAlloc>
uint8_t signed_add(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (word_type*) ma_alloc(ma, sizeof(word_type) * P);

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
        ma_free(ma, retx);
    }
    return carry;
}

/**
 * (无符号数)相加
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
        retx = (word_type*) ma_alloc(ma, sizeof(word_type) * P);

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
        ma_free(ma, retx);
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
 * (有符号数)相减
 * x<P> = a<M> - b<N>
 *
 * @return 进位
 */
template <typename T, typename MemAlloc>
uint8_t signed_sub(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<word_type>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (word_type*) ma_alloc(ma, sizeof(word_type) * P);

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
        ma_free(ma, retx);
    }
    return carry;
}

/**
 * (无符号数)相减
 * x<P> = a<M> - b<N>
 *
 * @return 进位
 */
template <typename T, typename MemAlloc>
uint8_t unsigned_sub(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<word_type>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (word_type*) ma_alloc(ma, sizeof(word_type) * P);

    uint8_t carry = 1;
    for (size_t i = 0; i < P; ++i)
    {
        const dword_type pluser1 = (i < M ? reinterpret_cast<const word_type*>(a)[i] : 0);
        dword_type pluser2 = static_cast<word_type>(~(i < N ? reinterpret_cast<const word_type*>(b)[i] : 0));
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<word_type>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (8 * sizeof(word_type)));
    }

    // 回写数据
    if (retx != reinterpret_cast<word_type*>(x))
    {
        ::memcpy(x, retx, sizeof(word_type) * P);
        ma_free(ma, retx);
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
 * (有符号数)取相反数
 * x<N> = -a<N>
 *
 * @return 进位
 */
template <typename T, typename MemAlloc>
uint8_t signed_negate(const T *a, size_t M, T *x, size_t N, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if (a < x && x < a + M)
        retx = (word_type*) ma_alloc(ma, sizeof(word_type) * N);

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
        ma_free(ma, retx);
    }
    return carry;
}

/**
 * (无符号数)取相反数
 * x<N> = -a<N>
 *
 * @return 进位
 */
template <typename T, typename MemAlloc>
uint8_t unsigned_negate(const T *a, size_t M, T *x, size_t N, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if (a < x && x < a + M)
        retx = (word_type*) ma_alloc(ma, sizeof(word_type) * N);

    uint8_t carry = 1;
    for (size_t i = 0; i < N; ++i)
    {
        dword_type pluser = static_cast<word_type>(~(i < M ? reinterpret_cast<const word_type*>(a)[i] : 0));
        pluser += carry;

        retx[i] = static_cast<word_type>(pluser);
        carry = static_cast<uint8_t>(pluser >> (8 * sizeof(word_type)));
    }

    // 回写数据
    if (retx != reinterpret_cast<word_type*>(x))
    {
        ::memcpy(x, retx, N);
        ma_free(ma, retx);
    }
    return carry;
}

/**
 * (无符号数/正数)平方优化
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
void _unsigned_square(const T *a, size_t M, T *x, size_t N, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != x && N > 0);
    assert(is_positive(a, M));
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if (a - N < x && x < a + M)
        retx = (word_type*) ma_alloc(ma, sizeof(word_type) * N);

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
    const size_t limit = (std::min)(N, M * 2);
    unsigned_shift_left(retx, limit, retx, limit, 1);

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
        ma_free(ma, retx);
    }
}

/**
 * (有符号数)相乘
 * x<P> = a<M> * b<N>
 */
template <typename T, typename MemAlloc>
void signed_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);
    typedef typename StdInt<T>::unsigned_type word_type;
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    if (a == b && M == N && is_positive(a, M))
    {
        _unsigned_square(a, M, x, P, ma);
        return;
    }

    // 避免区域交叉覆盖
    word_type *retx = reinterpret_cast<word_type*>(x);
    if ((a - P < x && x < a + M) || (b - P < x && x < b + N))
        retx = (word_type*) ma_alloc(ma, sizeof(word_type) * P);

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
        ma_free(ma, retx);
    }
}


/**
 * (无符号数)相乘
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
        retx = (word_type*) ma_alloc(ma, sizeof(word_type) * P);

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
        ma_free(ma, retx);
    }
}

template <typename T, typename MemAlloc>
void unsigned_karatsuba_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, MemAlloc *ma = NULL);

/**
 * karatsuba 乘法，时间复杂度为 O(n^(log2 3)) = O(n^1.58)
 *
 * a = A * base + B (其中 base = 2 ^ (n/2) )
 * b = C * base + D
 * a * b = AC * (base^2) + ((A+B)(C+D) - AC - BD) * base + BD
 * 该算式中乘法总体规模变小了
 */
template <typename T, typename MemAlloc>
void signed_karatsuba_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, MemAlloc *ma = NULL)
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

/**
 * karatsuba 乘法，时间复杂度为 O(n^(log2 3)) = O(n^1.58)
 *
 * a = A * base + B (其中 base = 2 ^ (n/2) )
 * b = C * base + D
 * a * b = AC * (base^2) + ((A+B)(C+D) - AC - BD) * base + BD
 * 该算式中乘法总体规模变小了
 */
template <typename T, typename MemAlloc>
void unsigned_karatsuba_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0 && NULL != x && P > 0);

    // 去除无效位长
    M = (std::min)(unsigned_significant_size(a, M), P);
    N = (std::min)(unsigned_significant_size(b, N), P);

    // 规模较小时使用一般算法
    if (M < 5 || N < 5 || P < 5)
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

    // ac_len = min(a_len + c_len, max(0, P - base_len))
    //        = min(a_len + c_len, P - base_len)
    // since, P - base_len >= max(M,N) - base_len > 0
    const size_t ac_len = (std::min)(a_len + c_len, P - base_len);
    T *AC = (T*) ma_alloc(ma, sizeof(T) * ac_len);
    unsigned_karatsuba_multiply(A, a_len, C, c_len, AC, ac_len, ma);

    // bd_len = min(b_len + d_len, P)
    const size_t bd_len = (std::min)(b_len + d_len, P);
    T *BD = x;
    unsigned_karatsuba_multiply(B, b_len, D, d_len, BD, bd_len, ma);

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
 * (有符号数)相除
 * x<P> = a<M> / b<N>
 * y<Q> = a<M> % b<N>
 *
 * @param x
 *      商
 * @param y
 *      余数
 */
template <typename T, typename MemAlloc>
void signed_divide(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, T *y, size_t Q, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);
    assert((NULL != x && P > 0) || (NULL != y && Q > 0));
    assert(NULL == x || P == 0 || NULL == y || Q == 0 || y >= x + P || x >= y + Q); // 避免区域交叉覆盖
    assert(!is_zero(b, N)); // 被除数不能为0

    typedef typename StdInt<T>::unsigned_type word_type;

    // 常量
    const size_t dividend_len = signed_significant_size(a, M);
    const size_t divisor_len = signed_significant_size(b, N);
    const bool dividend_positive = is_positive(a, M); /// 先把变量算出来，避免操作数被破坏
    const bool divisor_positive = is_positive(b, N);
    const size_t quotient_len = (std::min)(P, dividend_len);

    // 避免数据在计算中途被破坏
    word_type *quotient = reinterpret_cast<word_type*>(x); // 商，可以为 NULL
    if ((a - P < x && x < a) || (b - P < x && x < b + N)) // 兼容 x==a 的情况
        quotient = (word_type*) ma_alloc(ma, sizeof(word_type) * quotient_len);
    word_type *remainder = reinterpret_cast<word_type*>(y); // 余数，不能为 NULL
    if (NULL == y || Q < divisor_len || (a - Q < y && y < a + M) || (b - Q < y && y < b + N))
        remainder = (word_type*) ma_alloc(ma, sizeof(word_type) * divisor_len);

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
            signed_shift_left(remainder, divisor_len, remainder, divisor_len, 1);
            remainder[0] |= (next_dividend_word >> (8 * sizeof(word_type) - 1 - j)) & 0x01;

            // 加上/减去除数
            if (remainder_positive == divisor_positive)
                signed_sub(remainder, divisor_len, reinterpret_cast<const word_type*>(b), divisor_len, remainder, divisor_len, ma);
            else
                signed_add(remainder, divisor_len, reinterpret_cast<const word_type*>(b), divisor_len, remainder, divisor_len, ma);

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
        signed_expand(quotient, quotient_len, reinterpret_cast<word_type*>(x), P);
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
                signed_add(remainder, divisor_len, reinterpret_cast<const word_type*>(b), divisor_len, remainder, divisor_len, ma);
            else
                signed_sub(remainder, divisor_len, reinterpret_cast<const word_type*>(b), divisor_len, remainder, divisor_len, ma);
        }
        signed_expand(remainder, divisor_len, reinterpret_cast<word_type*>(y), Q);
    }

    // 释放空间
    if (quotient != reinterpret_cast<word_type*>(x))
        ma_free(ma, quotient);
    if (remainder != reinterpret_cast<word_type*>(y))
        ma_free(ma, remainder);
}

/**
 * (无符号数)相除
 * x<P> = a<M> / b<N>
 * y<Q> = a<M> % b<N>
 *
 * @param x
 *    商
 * @param y
 *    余数
 */
template <typename T, typename MemAlloc>
void unsigned_divide(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, T *y, size_t Q, MemAlloc *ma = NULL)
{
    assert(NULL != a && M > 0 && NULL != b && N > 0);
    assert((NULL != x && P > 0) || (NULL != y && Q > 0));
    assert(NULL == x || P == 0 || NULL == y || Q == 0 || y >= x + P || x >= y + Q); // 避免区域交叉覆盖
    assert(!is_zero(b, N)); // 被除数不能为0

    typedef typename StdInt<T>::unsigned_type word_type;

    // 常量
    const size_t dividend_len = unsigned_significant_size(a, M);
    const size_t divisor_len = unsigned_significant_size(b, N);
    const size_t quotient_len = (std::min)(P, dividend_len);

    // 避免数据在计算中途被破坏
    word_type *quotient = reinterpret_cast<word_type*>(x); // 商，可以为 NULL
    if ((a - P < x && x < a) || (b - P < x && x < b + N)) // 兼容 x==a 的情况
        quotient = (word_type*) ma_alloc(ma, sizeof(word_type) * quotient_len);
    word_type *remainder = reinterpret_cast<word_type*>(y); // 余数，不能为 NULL
    if (NULL == y || Q < divisor_len || (a - Q < y && y < a + M) || (b - Q < y && y < b + N))
        remainder = (word_type*) ma_alloc(ma, sizeof(word_type) * divisor_len);

    // 逐位试商
    ::memset(remainder, 0, sizeof(word_type) * divisor_len); // 初始化余数
    bool remainder_positive = true;
    for (size_t i = 0; i < dividend_len; ++i)
    {
        const size_t dividend_word_pos = dividend_len - i - 1;
        const word_type next_dividend_word = reinterpret_cast<const word_type*>(a)[dividend_word_pos]; // 余数左移时的低位补位部分
        if (NULL != quotient && dividend_word_pos < P)
            quotient[dividend_word_pos] = 0; // 初始化商，注意，兼容 x==a 的情况

        for (size_t j = 0; j < 8 * sizeof(word_type); ++j)
        {
            // 余数左移1位
            unsigned_shift_left(remainder, divisor_len, remainder, divisor_len, 1);
            remainder[0] |= (next_dividend_word >> (8 * sizeof(word_type) - 1 - j)) & 0x01;

            // 加上/减去除数
            if (remainder_positive)
                unsigned_sub(remainder, divisor_len, reinterpret_cast<const word_type*>(b), divisor_len, remainder, divisor_len, ma);
            else
                unsigned_add(remainder, divisor_len, reinterpret_cast<const word_type*>(b), divisor_len, remainder, divisor_len, ma);

            // 试商结果
            remainder_positive = is_positive(remainder, divisor_len);
            if (remainder_positive && NULL != quotient && dividend_word_pos < P)
                quotient[dividend_word_pos] |= (1 << (8 * sizeof(word_type) - 1 - j));
        }
    }

    // 商
    if (NULL != x)
        unsigned_expand(quotient, quotient_len, reinterpret_cast<word_type*>(x), P);

    /**
        恢复余数:
        如果未除尽且余数符号与被除数不一致，余数需加上除数
    */
    if (NULL != y)
    {
        if (!is_zero(remainder, divisor_len) && !remainder_positive)
            unsigned_add(remainder, divisor_len, reinterpret_cast<const word_type*>(b), divisor_len, remainder, divisor_len, ma);
        unsigned_expand(remainder, divisor_len, reinterpret_cast<word_type>(y), Q);
    }

    // 释放空间
    if (quotient != reinterpret_cast<word_type*>(x))
        ma_free(ma, quotient);
    if (remainder != reinterpret_cast<word_type*>(y))
        ma_free(ma, remainder);
}

}

#undef OPTIMIZE_LEVEL

#endif /* head file guarder */
