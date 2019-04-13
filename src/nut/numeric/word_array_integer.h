/**
 * 由 '字(word)' 的数组来表示的大整数
 *
 * NOTE
 * - word 之间按照 little-endian 顺序存储, 高位 word 在数组末尾.
 * - 对于带符号大整数, 最高位 word 的最高位 bit 表示整个大整数的符号.
 * - word 内部的字节序与本地字节序相同; 即在 big-endian 机器下是 big-endian, 在
 *   little-endian 机器下是 little-endian
 */

#ifndef ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_
#define ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_

#include <assert.h>
#include <string.h> // for memset(), memcpy(), memmove()
#include <stdlib.h> // for malloc()
#include <algorithm>

#include "../nut_config.h"
#include "../platform/platform.h"
#include "../platform/int_type.h"
#include "../platform/endian.h"


namespace nut
{

/**
 * 将 word 内部字节序当成是 little-endian 来获取指定位置的字节
 */
template <typename T>
uint8_t get_byte_le(const T *arr, size_t le_byte_index)
{
#if NUT_ENDIAN_LITTLE_BYTE
    return reinterpret_cast<const uint8_t*>(arr)[le_byte_index];
#else
    const size_t word_index = le_byte_index / sizeof(T);
    const size_t inner_index = sizeof(T) - 1 - (le_byte_index % sizeof(T));
    return reinterpret_cast<const uint8_t*>(word + word_index)[inner_index];
#endif
}

/**
 * 将 word 内部字节序当成是 little-endian 来设置指定位置的字节
 */
template <typename T>
void set_byte_le(T *arr, size_t le_byte_index, uint8_t v)
{
#if NUT_ENDIAN_LITTLE_BYTE
    reinterpret_cast<uint8_t*>(arr)[le_byte_index] = v;
#else
    const size_t word_index = le_byte_index / sizeof(T);
    const size_t inner_index = sizeof(T) - 1 - (le_byte_index % sizeof(T));
    reinterpret_cast<uint8_t*>(word + word_index)[inner_index] = v;
#endif
}

/**
 * 是否为0
 *
 * @return a<N> == 0
 */
template <typename T>
bool is_zero(const T *a, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && N > 0);

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
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && N > 0);
    const T mask = ((T) 1) << (8 * sizeof(T) - 1);
    return 0 == (a[N - 1] & mask);
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
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    return !is_positive(a, N);
}

/**
 * (有符号数)有效'字'数
 *
 * @return 返回值>=1
 */
template <typename T>
size_t signed_significant_size(const T *a, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && N > 0);

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
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && N > 0);

    size_t ret = N;
    while (ret > 1 && a[ret - 1] == 0)
        --ret;
    return ret;
}

/**
 * (有符号数)比较
 *
 * @return <0 if a<M> < b<N>
 *         0 if a<M> == b<N>
 *         >0 if a<M> > b<N>
 */
template <typename T>
int signed_compare(const T *a, size_t M, const T *b, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0);

    const bool positive1 = is_positive(a, M), positive2 = is_positive(b, N);
    if (positive1 != positive2)
        return positive2 ? -1 : 1;

    // 同号比较
    const T fill = (positive1 ? 0 : ~(T)0);
    for (ssize_t i = std::max(M, N) - 1; i >= 0; --i)
    {
        const T op1 = (i < (ssize_t) M ? a[i] : fill);
        const T op2 = (i < (ssize_t) N ? b[i] : fill);
        if (op1 != op2)
            return op1 < op2 ? -1 : 1;
    }
    return 0; // 相等
}

/**
 * (无符号数)比较
 *
 * @return <0 if a<M> < b<N>
 *         0 if a<M> == b<N>
 *         >0 if a<M> > b<N>
 */
template <typename T>
int unsigned_compare(const T *a, size_t M, const T *b, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0);

    for (ssize_t i = std::max(M, N) - 1; i >= 0; --i)
    {
        const T op1 = (i < (ssize_t) M ? a[i] : 0);
        const T op2 = (i < (ssize_t) N ? b[i] : 0);
        if (op1 != op2)
            return op1 < op2 ? -1 : 1;
    }
    return 0; // 相等
}

/**
 * (有符号数)带符号扩展(或者截断)
 * a<M> -> x<N>
 */
template <typename T>
void signed_expand(const T *a, size_t M, T *x, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

    const int fill = (is_positive(a, M) ? 0 : 0xff); // NOTE 先把变量算出来，避免操作数被破坏
    if (x != a)
        ::memmove(x, a, sizeof(T) * std::min(M, N));
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
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

    if (x != a)
        ::memmove(x, a, sizeof(T) * std::min(M, N));
    if (M < N)
        ::memset(x + M, 0, sizeof(T) * (N - M));
}

template <typename T>
void _signed_shift_left_word(const T *a, size_t M, T *x, size_t N, size_t word_count)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

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
        for (ssize_t i = N - 1; i >= 0; --i)
            x[i] = (i < (ssize_t) word_count ? 0 : (i - word_count >= M ? fill : a[i - word_count]));
    }
}

/**
 * (有符号数)左移
 * x<N> = a<N> << count
 */
template <typename T>
void signed_shift_left(const T *a, size_t M, T *x, size_t N, size_t bit_count)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

    const size_t words_off = bit_count / (8 * sizeof(T)),
        bits_off = bit_count % (8 * sizeof(T));
    if (0 == bits_off)
    {
        _signed_shift_left_word(a, M, x, N, words_off);
    }
    else if (x + words_off < a)
    {
        const T fill = (is_positive(a, M) ? 0 : ~(T)0);
        for (size_t i = 0; i < N; ++i)
        {
            const T high = (i < words_off ? 0 : (i - words_off >= M ? fill :
                    a[i - words_off])) << bits_off;
            const T low = (i < words_off + 1 ? 0 : (i - words_off - 1 >= M ? fill :
                    a[i - words_off - 1])) >> (8 * sizeof(T) - bits_off);
            x[i] = high | low;
        }
    }
    else
    {
        const T fill = (is_positive(a, M) ? 0 : ~(T)0);
        for (ssize_t i = N - 1; i >= 0; --i)
        {
            const T high = (i < (ssize_t) words_off ? 0 : (i - words_off >= M ? fill :
                    a[i - words_off])) << bits_off;
            const T low = (i < (ssize_t) words_off + 1 ? 0 : (i - words_off - 1 >= M ? fill :
                    a[i - words_off - 1])) >> (8 * sizeof(T) - bits_off);
            x[i] = high | low;
        }
    }
}

template <typename T>
void _unsigned_shift_left_word(const T *a, size_t M, T *x, size_t N, size_t word_count)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

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
        for (ssize_t i = N - 1; i >= 0; --i)
            x[i] = (i < (ssize_t) word_count ? 0 : (i - word_count >= M ? 0 : a[i - word_count]));
    }
}

/**
 * (无符号数)左移
 * x<N> = a<N> << count
 */
template <typename T>
void unsigned_shift_left(const T *a, size_t M, T *x, size_t N, size_t bit_count)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

    const size_t words_off = bit_count / (8 * sizeof(T)),
        bits_off = bit_count % (8 * sizeof(T));
    if (0 == bits_off)
    {
        _unsigned_shift_left_word(a, M, x, N, words_off);
    }
    else if (x + words_off < a)
    {
        for (size_t i = 0; i < N; ++i)
        {
            const T high = (i < words_off ? 0 : (i - words_off >= M ? 0 :
                    a[i - words_off])) << bits_off;
            const T low = (i < words_off + 1 ? 0 : (i - words_off - 1 >= M ? 0 :
                    a[i - words_off - 1])) >> (8 * sizeof(T) - bits_off);
            x[i] = high | low;
        }
    }
    else
    {
        for (ssize_t i = N - 1; i >= 0; --i)
        {
            const T high = (i < (ssize_t) words_off ? 0 : (i - words_off >= M ? 0 :
                    a[i - words_off])) << bits_off;
            const T low = (i < (ssize_t) words_off + 1 ? 0 : (i - words_off - 1 >= M ? 0 :
                    a[i - words_off - 1])) >> (8 * sizeof(T) - bits_off);
            x[i] = high | low;
        }
    }
}

template <typename T>
void _signed_shift_right_word(const T *a, size_t M, T *x, size_t N, size_t word_count)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

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
        for (ssize_t i = N - 1; i >= 0; --i)
            x[i] = (i + word_count >= M ? fill : a[i + word_count]);
    }
}

/**
 * (有符号数)右移
 * x<N> = a<N> >> count
 */
template <typename T>
void signed_shift_right(const T *a, size_t M, T *x, size_t N, size_t bit_count)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

    const size_t words_off = bit_count / (8 * sizeof(T)),
        bits_off = bit_count % (8 * sizeof(T));
    if (0 == bits_off)
    {
        _signed_shift_right_word(a, M, x, N, words_off);
    }
    else if (x <= a + words_off)
    {
        const T fill = (is_positive(a, M) ? 0 : ~(T)0);
        for (size_t i = 0; i < N; ++i)
        {
            const T high = (i + words_off + 1 >= M ? fill :
                            a[i + words_off + 1]) << (8 * sizeof(T) - bits_off);
            const T low = (i + words_off >= M ? fill :
                           a[i + words_off]) >> bits_off;
            x[i] = high | low;
        }
    }
    else
    {
        const T fill = (is_positive(a, M) ? 0 : ~(T)0);
        for (ssize_t i = N - 1; i >= 0; --i)
        {
            const T high = (i + words_off + 1 >= M ? fill :
                            a[i + words_off + 1]) << (8 * sizeof(T) - bits_off);
            const T low = (i + words_off >= M ? fill :
                           a[i + words_off]) >> bits_off;
            x[i] = high | low;
        }
    }
}

template <typename T>
void _unsigned_shift_right_word(const T *a, size_t M, T *x, size_t N, size_t word_count)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

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
        for (ssize_t i = N - 1; i >= 0; --i)
            x[i] = (i + word_count >= M ? 0 : a[i + word_count]);
    }
}

/**
 * (无符号数)右移
 * x<N> = a<N> >> count
 */
template <typename T>
void unsigned_shift_right(const T *a, size_t M, T *x, size_t N, size_t bit_count)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

    const size_t words_off = bit_count / (8 * sizeof(T)),
        bits_off = bit_count % (8 * sizeof(T));
    if (0 == bits_off)
    {
        _unsigned_shift_right_word(a, M, x, N, words_off);
    }
    else if (x <= a + words_off)
    {
        for (size_t i = 0; i < N; ++i)
        {
            const T high = (i + words_off + 1 >= M ? 0 :
                    a[i + words_off + 1]) << (8 * sizeof(T) - bits_off);
            const T low = (i + words_off >= M ? 0 :
                    a[i + words_off]) >> bits_off;
            x[i] = high | low;
        }
    }
    else
    {
        for (ssize_t i = N - 1; i >= 0; --i)
        {
            const T high = (i + words_off + 1 >= M ? 0 :
                            a[i + words_off + 1]) << (8 * sizeof(T) - bits_off);
            const T low = (i + words_off >= M ? 0 :
                           a[i + words_off]) >> bits_off;
            x[i] = high | low;
        }
    }
}

/**
 * 按位与
 * x<N> = a<N> & b<N>
 */
template <typename T>
void bit_and(const T *a, const T *b, T *x, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && nullptr != b && nullptr != x && N > 0);

#if 0 // unoptimized
    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (T*) ::malloc(sizeof(T) * N);

    for (size_t i = 0; i < N; ++i)
        retx[i] = a[i] & b[i];

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(T) * N);
        ::free(retx);
    }
#else
    if ((x <= a || x >= a + N) && (x <= b || x >= b + N))
    {
        for (size_t i = 0; i < N; ++i)
            x[i] = a[i] & b[i];
    }
    else if ((x <= a - N || x >= a) && (x <= b - N || x >= b))
    {
        for (ssize_t i = N - 1; i >= 0; --i)
            x[i] = a[i] & b[i];
    }
    else
    {
        // 避免区域交叉覆盖
        T *const retx = (T*) ::malloc(sizeof(T) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = a[i] & b[i];

        // 回写数据
        ::memcpy(x, retx, sizeof(T) * N);
        ::free(retx);
    }
#endif
}

/**
 * 按位或
 * x<N> = a<N> | b<N>
 */
template <typename T>
void bit_or(const T *a, const T *b, T *x, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && nullptr != b && nullptr != x && N > 0);

#if 0 // unoptimized
    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (T*) ::malloc(sizeof(T) * N);

    for (size_t i = 0; i < N; ++i)
        retx[i] = a[i] | b[i];

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(T) * N);
        ::free(retx);
    }
#else
    if ((x <= a || x >= a + N) && (x <= b || x >= b + N))
    {
        for (size_t i = 0; i < N; ++i)
            x[i] = a[i] | b[i];
    }
    else if ((x <= a - N || x >= a) && (x <= b - N || x >= b))
    {
        for (ssize_t i = N - 1; i >= 0; --i)
            x[i] = a[i] | b[i];
    }
    else
    {
        // 避免区域交叉覆盖
        T *const retx = (T*) ::malloc(sizeof(T) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = a[i] | b[i];

        // 回写数据
        ::memcpy(x, retx, sizeof(T) * N);
        ::free(retx);
    }
#endif
}

/**
 * 按位异或
 * x<N> = a<N> ^ b<N>
 */
template <typename T>
void bit_xor(const T *a, const T *b, T *x, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && nullptr != b && nullptr != x && N > 0);

#if 0 // unoptimized
    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (T*) ::malloc(sizeof(T) * N);

    for (size_t i = 0; i < N; ++i)
        retx[i] = a[i] ^ b[i];

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(T) * N);
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
        for (ssize_t i = N - 1; i >= 0; --i)
            x[i] = a[i] ^ b[i];
    }
    else
    {
        // 避免区域交叉覆盖
        T *const retx = (T*) ::malloc(sizeof(T) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = a[i] ^ b[i];

        // 回写数据
        ::memcpy(x, retx, sizeof(T) * N);
        ::free(retx);
    }
#endif
}

/**
 * 按位同或
 * x<N> = ~(a<N> ^ b<N>)
 */
template <typename T>
void bit_nxor(const T *a, const T *b, T *x, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && nullptr != b && nullptr != x && N > 0);

#if 0 // unoptimized
    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (T*) ::malloc(sizeof(T) * N);

    for (size_t i = 0; i < N; ++i)
        retx[i] = ~(a[i] ^ b[i]);

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(T) * N);
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
        for (ssize_t i = N - 1; i >= 0; --i)
            x[i] = ~(a[i] ^ b[i]);
    }
    else
    {
        // 避免区域交叉覆盖
        T *const retx = (T*) ::malloc(sizeof(T) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = ~(a[i] ^ b[i]);

        // 回写数据
        ::memcpy(x, retx, sizeof(T) * N);
        ::free(retx);
    }
#endif
}

/**
 * 按位取反
 * x<N> = ~a<N>
 */
template <typename T>
void bit_not(const T *a, T *x, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && nullptr != x && N > 0);

    if (x < a)
    {
        for (size_t i = 0; i < N; ++i)
            x[i] = ~a[i];
    }
    else
    {
        for (ssize_t i = N - 1; i >= 0; --i)
            x[i] = ~a[i];
    }
}

/**
 * 统计 bit 1 数目
 */
NUT_API unsigned bit1_count(uint8_t a);
NUT_API unsigned bit1_count(uint16_t a);
NUT_API unsigned bit1_count(uint32_t a);
NUT_API unsigned bit1_count(uint64_t a);
#if NUT_HAS_INT128
NUT_API unsigned bit1_count(uint128_t a);
#endif

NUT_API size_t bit1_count(const uint8_t *a, size_t N);

/**
 * 统计 bit 0 数目
 */
NUT_API size_t bit0_count(const uint8_t *a, size_t N);

/**
 * bit 逆序化，eg. 10110101 -> 10101101
 */
NUT_API uint8_t reverse_bits(uint8_t v);
NUT_API uint16_t reverse_bits(uint16_t v);
NUT_API uint32_t reverse_bits(uint32_t v);
NUT_API uint64_t reverse_bits(uint64_t v);
#if NUT_HAS_INT128
NUT_API uint128_t reverse_bits(uint128_t v);
#endif

/**
 * 返回从低位到高位第一个 bit 1 的位置
 *
 * @return -1 if not found
 *         >=0 if found
 */
NUT_API int lowest_bit1(uint8_t a);
NUT_API int lowest_bit1(uint16_t a);
NUT_API int lowest_bit1(uint32_t a);
NUT_API int lowest_bit1(uint64_t a);
#if NUT_HAS_INT128
NUT_API int lowest_bit1(uint128_t a);
#endif

/**
 * 返回从低位到高位第一个 bit 1 的位置
 *
 * NOTE 对于 big-endian 机器，不能随便转换指针类型后再调用, 即
 *      lowest_bit1<uint8_t>() != lowest_bit1<uint16_t>() ...
 *
 * @return -1 if not found
 *         >=0 if found
 */
template <typename T>
ssize_t lowest_bit1(const T *a, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && N > 0);
    for (size_t i = 0; i < N; ++i)
    {
        if (0 != a[i])
            return i * 8 * sizeof(T) + lowest_bit1(a[i]);
    }
    return -1;
}

/**
 * 返回从低位到高位第一个 bit 0 的位置
 *
 * NOTE 对于 big-endian 机器，不能随便转换指针类型后再调用, 即
 *      lowest_bit0<uint8_t>() != lowest_bit0<uint16_t>() ...
 *
 * @return -1 if not found
 *         >=0 if found
 */
template <typename T>
ssize_t lowest_bit0(const T *a, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && N > 0);
    const T ignore = ~(T)0;
    for (size_t i = 0; i < N; ++i)
    {
        if (ignore != a[i])
            return i * 8 * sizeof(T) + lowest_bit1((T)~a[i]);
    }
    return -1;
}

/**
 * 返回从高位到低位第一个 bit 1 的位置
 *
 * @return -1 if not found
 *         >=0 if found
 */
NUT_API int highest_bit1(uint8_t a);
NUT_API int highest_bit1(uint16_t a);
NUT_API int highest_bit1(uint32_t a);
NUT_API int highest_bit1(uint64_t a);
#if NUT_HAS_INT128
NUT_API int highest_bit1(uint128_t a);
#endif

/**
 * 返回从高位到低位第一个 bit 1 的位置
 *
 * NOTE 对于 big-endian 机器，不能随便转换指针类型后再调用, 即
 *      highest_bit1<uint8_t>() != highest_bit1<uint16_t>() ...
 *
 * @return -1 if not found
 *         >=0 if found
 */
template <typename T>
ssize_t highest_bit1(const T *a, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && N > 0);
    for (ssize_t i = N - 1; i >= 0; --i)
    {
        if (0 != a[i])
            return i * 8 * sizeof(T) + highest_bit1(a[i]);
    }
    return -1;
}

/**
 * 返回从高位到低位第一个 bit 0 的位置
 *
 * NOTE 对于 big-endian 机器，不能随便转换指针类型后再调用, 即
 *      highest_bit0<uint8_t>() != highest_bit0<uint16_t>() ...
 *
 * @return -1 if not found
 *         >=0 if found
 */
template <typename T>
ssize_t highest_bit0(const T *a, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && N > 0);
    const T ignore = ~(T)0;
    for (ssize_t i = N - 1; i >= 0; --i)
    {
        if (ignore != a[i])
            return i * 8 * sizeof(T) + highest_bit1((T)~a[i]);
    }
    return -1;
}

/**
 * 正数 bit1 length, eg 00110100 -> 6
 */
template <typename T>
size_t bit1_length(const T *a,  size_t N)
{
    return highest_bit1<T>(a, N) + 1;
}

/**
 * 负数 bit0 length, eg 11101000 -> 5
 */
template <typename T>
size_t bit0_length(const T *a, size_t N)
{
    return highest_bit0<T>(a, N) + 1;
}

/**
 * (有符号数)相加
 * x<P> = a<M> + b<N>
 *
 * @return 进位
 */
template <typename T>
uint8_t signed_add(const T *a, size_t M, const T *b, size_t N, T *x, size_t P)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (T*) ::malloc(sizeof(T) * P);

    uint8_t carry = 0;
    const T filla = (is_positive(a, M) ? 0 : ~(T)0),
        fillb = (is_positive(b, N) ? 0 : ~(T)0);
    for (size_t i = 0; i < P; ++i)
    {
        const dword_type pluser1 = (i < M ? a[i] : filla);
        dword_type pluser2 = (i < N ? b[i] : fillb);
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<T>(pluser2);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[0]);
#else
        carry = static_cast<uint8_t>(pluser2 >> (8 * sizeof(T))); // 稍慢
#endif
    }

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(T) * P);
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
template <typename T>
uint8_t unsigned_add(const T *a, size_t M, const T *b, size_t N, T *x, size_t P)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (T*) ::malloc(sizeof(T) * P);

    uint8_t carry = 0;
    for (size_t i = 0; i < P; ++i)
    {
        const dword_type pluser1 = (i < M ? a[i] : 0);
        dword_type pluser2 = (i < N ? b[i] : 0);
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<T>(pluser2);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[0]);
#else
        carry = static_cast<uint8_t>(pluser2 >> (8 * sizeof(T)));
#endif
    }

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(T) * P);
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
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != x && N > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    uint8_t carry = 1;
    for (size_t i = 0; i < N && 0 != carry; ++i)
    {
        dword_type pluser = x[i];
        pluser += carry;

        x[i] = static_cast<T>(pluser);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[0]);
#else
        carry = static_cast<uint8_t>(pluser >> (8 * sizeof(T)));
#endif
    }
    return carry;
}

/**
 * (有符号数)相减
 * x<P> = a<M> - b<N>
 *
 * @return 进位
 */
template <typename T>
uint8_t signed_sub(const T *a, size_t M, const T *b, size_t N, T *x, size_t P)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (T*) ::malloc(sizeof(T) * P);

    const T filla = (is_positive(a, M) ? 0 : ~(T)0),
        fillb = (is_positive(b, N) ? 0 : ~(T)0);
    uint8_t carry = 1;
    for (size_t i = 0; i < P; ++i)
    {
        const dword_type pluser1 = (i < M ? a[i] : filla);
        dword_type pluser2 = static_cast<T>(~(i < N ? b[i] : fillb));
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<T>(pluser2);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[0]);
#else
        carry = static_cast<uint8_t>(pluser2 >> (8 * sizeof(T)));
#endif
    }

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(T) * P);
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
template <typename T>
uint8_t unsigned_sub(const T *a, size_t M, const T *b, size_t N, T *x, size_t P)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + M) || (b < x && x < b + N))
        retx = (T*) ::malloc(sizeof(T) * P);

    uint8_t carry = 1;
    for (size_t i = 0; i < P; ++i)
    {
        const dword_type pluser1 = (i < M ? a[i] : 0);
        dword_type pluser2 = static_cast<T>(~(i < N ? b[i] : 0));
        pluser2 += pluser1 + carry;

        retx[i] = static_cast<T>(pluser2);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser2)[0]);
#else
        carry = static_cast<uint8_t>(pluser2 >> (8 * sizeof(T)));
#endif
    }

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(T) * P);
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
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != x && N > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    uint8_t carry = 0;
    for (size_t i = 0; i < N && 1 != carry; ++i)
    {
        dword_type pluser = x[i];
        pluser += carry + (dword_type)(T) ~(T)0;

        x[i] = static_cast<T>(pluser);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[0]);
#else
        carry = static_cast<uint8_t>(pluser >> (8 * sizeof(T)));
#endif
    }
    return carry;
}

/**
 * (有符号数)取相反数
 * x<N> = -a<N>
 *
 * @return 进位
 */
template <typename T>
uint8_t signed_negate(const T *a, size_t M, T *x, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if (a < x && x < a + M)
        retx = (T*) ::malloc(sizeof(T) * N);

    uint8_t carry = 1;
    const T fill = (is_positive(a, M) ? 0 : ~(T)0);
    for (size_t i = 0; i < N; ++i)
    {
        dword_type pluser = static_cast<T>(~(i < M ? a[i] : fill));
        pluser += carry;

        retx[i] = static_cast<T>(pluser);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[0]);
#else
        carry = static_cast<uint8_t>(pluser >> (8 * sizeof(T)));
#endif
    }

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, N);
        ::free(retx);
    }
    return carry;
}

/**
 * (无符号数)取相反数
 * x<N> = -a<N>
 *
 * @return 进位
 */
template <typename T>
uint8_t unsigned_negate(const T *a, size_t M, T *x, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if (a < x && x < a + M)
        retx = (T*) ::malloc(sizeof(T) * N);

    uint8_t carry = 1;
    for (size_t i = 0; i < N; ++i)
    {
        dword_type pluser = static_cast<T>(~(i < M ? a[i] : 0));
        pluser += carry;

        retx[i] = static_cast<T>(pluser);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[1]);
#elif NUT_ENDIAN_BIG_BYTE
        carry = static_cast<uint8_t>(reinterpret_cast<const T*>(&pluser)[0]);
#else
        carry = static_cast<uint8_t>(pluser >> (8 * sizeof(T)));
#endif
    }

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, N);
        ::free(retx);
    }
    return carry;
}

/**
 * (无符号数/正数)平方优化
 *
 *               a  b  c  d  e
 *          *    a  b  c  d  e
 *          -------------------
 *              ae be ce de ee
 *           ad bd cd dd de
 *        ac bc cc cd ce
 *     ab bb bc bd be
 *  aa ab ac ad ae
 *
 * 可拆成三部分，上下两部分是重复的：
 *
 *              ae be ce de
 *           ad bd cd
 *        ac bc             ee
 *     ab             dd
 *              cc
 *        bb             de
 *  aa             cd ce
 *           bc bd be
 *     ab ac ad ae
 */
template <typename T>
void _unsigned_square(const T *a, size_t M, T *x, size_t N)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);
    assert(is_positive(a, M));
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if (a - N < x && x < a + M)
        retx = (T*) ::malloc(sizeof(T) * N);

    // 先计算一半
    ::memset(retx, 0, sizeof(T) * N);
    for (size_t i = 0; i < M - 1; ++i)
    {
        if (i * 2 + 1 >= N)
            break;

        const dword_type op1 = a[i];
        if (0 == op1)
            continue;

        T carry = 0;
        for (size_t j = i + 1; j < M && i + j < N; ++j)
        {
            dword_type op2 = a[j];
            op2 = op1 * op2 + retx[i + j] + carry;

            retx[i + j] = static_cast<T>(op2);
#if NUT_ENDIAN_LITTLE_BYTE
            carry = reinterpret_cast<const T*>(&op2)[1];
#elif NUT_ENDIAN_BIG_BYTE
            carry = reinterpret_cast<const T*>(&op2)[0];
#else
            carry = static_cast<T>(op2 >> (8 * sizeof(T)));
#endif
        }
        if (i + M < N)
            retx[i + M] = carry;
    }

    // 再加上另一半
    const size_t limit = std::min(N, M * 2);
    unsigned_shift_left(retx, limit, retx, limit, 1);

    // 加上中间对称线
    T carry = 0;
    for (size_t i = 0; i < M; ++i)
    {
        if (i * 2 >= N)
            break;

        dword_type op = a[i];
        op = op * op + retx[i * 2] + carry;

        retx[i * 2] = static_cast<T>(op);
#if NUT_ENDIAN_LITTLE_BYTE
        carry = reinterpret_cast<const T*>(&op)[1];
#elif NUT_ENDIAN_BIG_BYTE
        carry = reinterpret_cast<const T*>(&op)[0];
#else
        carry = static_cast<T>(op >> (8 * sizeof(T)));
#endif

        if (0 != carry && i * 2 + 1 < N)
        {
            op = retx[i * 2 + 1];
            op += carry;

            retx[i * 2 + 1] = static_cast<T>(op);
#if NUT_ENDIAN_LITTLE_BYTE
            carry = reinterpret_cast<const T*>(&op)[1];
#elif NUT_ENDIAN_BIG_BYTE
            carry = reinterpret_cast<const T*>(&op)[0];
#else
            carry = static_cast<T>(op >> (8 * sizeof(T)));
#endif
        }
    }

    // 回写结果
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(T) * N);
        ::free(retx);
    }
}

/**
 * (有符号数)相乘
 * x<P> = a<M> * b<N>
 */
template <typename T>
void signed_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    if (a == b && M == N && is_positive(a, M))
    {
        _unsigned_square(a, M, x, P);
        return;
    }

    // 避免区域交叉覆盖
    T *retx = x;
    if ((a - P < x && x < a + M) || (b - P < x && x < b + N))
        retx = (T*) ::malloc(sizeof(T) * P);

    // 乘法
    const T filla = (is_positive(a,M) ? 0 : ~(T)0),
        fillb = (is_positive(b,N) ? 0 : ~(T)0); // NOTE 先把变量算出来，避免操作数被破坏
    ::memset(retx, 0, sizeof(T) * P);
    for (size_t i = 0; i < P; ++i)
    {
        if (i >= M && 0 == filla)
            break;

        const dword_type mult1 = (i < M ? a[i] : filla);
        if (0 == mult1)
            continue;

        T carry = 0; // 这个进位包括乘法的，故此会大于1
        for (size_t j = 0; i + j < P; ++j)
        {
            if (j >= N && 0 == fillb && 0 == carry)
                break;

            dword_type mult2 = (j < N ? b[j] : fillb);
            mult2 = mult1 * mult2 + retx[i + j] + carry;

            retx[i + j] = static_cast<T>(mult2);
#if NUT_ENDIAN_LITTLE_BYTE
            carry = reinterpret_cast<const T*>(&mult2)[1];
#elif NUT_ENDIAN_BIG_BYTE
            carry = reinterpret_cast<const T*>(&mult2)[0];
#else
            carry = static_cast<T>(mult2 >> (8 * sizeof(T)));
#endif
        }
    }

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(T) * P);
        ::free(retx);
    }
}


/**
 * (无符号数)相乘
 * x<P> = a<M> * b<N>
 */
template <typename T>
void unsigned_multiply(const T *a, size_t M, const T *b, size_t N, T *x, size_t P)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0 && nullptr != x && P > 0);
    typedef typename StdInt<T>::double_unsigned_type dword_type;

    // 避免区域交叉覆盖
    T *retx = x;
    if ((a - P < x && x < a + M) || (b - P < x && x < b + N))
        retx = (T*) ::malloc(sizeof(T) * P);

    // 乘法
    ::memset(retx, 0, sizeof(T) * P);
    for (size_t i = 0; i < P; ++i)
    {
        if (i >= M)
            break;

        const dword_type mult1 = a[i];
        if (0 == mult1)
            continue;

        T carry = 0; // 这个进位包括乘法的，故此会大于1
        for (size_t j = 0; i + j < P; ++j)
        {
            if (j >= N && 0 == carry)
                break;

            dword_type mult2 = (j < N ? b[j] : 0);
            mult2 = mult1 * mult2 + retx[i + j] + carry;

            retx[i + j] = static_cast<T>(mult2);
#if NUT_ENDIAN_LITTLE_BYTE
            carry = reinterpret_cast<const T*>(&mult2)[1];
#elif NUT_ENDIAN_BIG_BYTE
            carry = reinterpret_cast<const T*>(&mult2)[0];
#else
            carry = static_cast<T>(mult2 >> (8 * sizeof(T)));
#endif
        }
    }

    // 回写数据
    if (retx != x)
    {
        ::memcpy(x, retx, sizeof(T) * P);
        ::free(retx);
    }
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
template <typename T>
void signed_divide(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, T *y, size_t Q)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0);
    assert((nullptr != x && P > 0) || (nullptr != y && Q > 0));
    assert(nullptr == x || P == 0 || nullptr == y || Q == 0 || y >= x + P || x >= y + Q); // 避免区域交叉覆盖
    assert(!is_zero(b, N)); // 被除数不能为0

    // 常量
    const size_t dividend_len = signed_significant_size(a, M);
    const size_t divisor_len = signed_significant_size(b, N);
    const bool dividend_positive = is_positive(a, M); /// 先把变量算出来，避免操作数被破坏
    const bool divisor_positive = is_positive(b, N);
    const size_t quotient_len = std::min(P, dividend_len);

    // 避免数据在计算中途被破坏
    T *quotient = x; // 商，可以为 nullptr
    if ((a - P < x && x < a) || (b - P < x && x < b + N)) // 兼容 x==a 的情况
        quotient = (T*) ::malloc(sizeof(T) * quotient_len);
    T *remainder = y; // 余数，不能为 nullptr
    if (nullptr == y || Q < divisor_len || (a - Q < y && y < a + M) || (b - Q < y && y < b + N))
        remainder = (T*) ::malloc(sizeof(T) * divisor_len);

    // 逐位试商
    ::memset(remainder, (dividend_positive ? 0 : 0xFF), sizeof(T) * divisor_len); // 初始化余数
    bool remainder_positive = dividend_positive; // 余数的符号
    bool quotient_positive = true; // 商的符号
    for (size_t i = 0; i < dividend_len; ++i)
    {
        const size_t dividend_word_pos = dividend_len - i - 1;
        const T next_dividend_word = a[dividend_word_pos]; // 余数左移时的低位补位部分
        if (nullptr != quotient && dividend_word_pos < P)
            quotient[dividend_word_pos] = 0; // 初始化商，注意，兼容 x==a 的情况

        for (size_t j = 0; j < 8 * sizeof(T); ++j)
        {
            // 余数左移1位
            signed_shift_left(remainder, divisor_len, remainder, divisor_len, 1);
            remainder[0] |= (next_dividend_word >> (8 * sizeof(T) - 1 - j)) & 0x01;

            // 加上/减去除数
            if (remainder_positive == divisor_positive)
                signed_sub(remainder, divisor_len, b, divisor_len, remainder, divisor_len);
            else
                signed_add(remainder, divisor_len, b, divisor_len, remainder, divisor_len);

            // 试商结果
            remainder_positive = is_positive(remainder, divisor_len);
            if (remainder_positive == divisor_positive)
            {
                if (nullptr != quotient && dividend_word_pos < P)
                    quotient[dividend_word_pos] |= (1 << (8 * sizeof(T) - 1 - j));
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
    if (nullptr != x)
    {
        assert(nullptr != quotient);
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
        signed_expand(quotient, quotient_len, x, P);
    }

    /**
        恢复余数:
        如果未除尽且余数符号与被除数不一致，余数需加修正
    */
    if (nullptr != y)
    {
        if (!remainder_is_zero && remainder_positive != dividend_positive)
        {
            if (divisor_positive == dividend_positive)
                signed_add(remainder, divisor_len, b, divisor_len, remainder, divisor_len);
            else
                signed_sub(remainder, divisor_len, b, divisor_len, remainder, divisor_len);
        }
        signed_expand(remainder, divisor_len, y, Q);
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
 *    商
 * @param y
 *    余数
 */
template <typename T>
void unsigned_divide(const T *a, size_t M, const T *b, size_t N, T *x, size_t P, T *y, size_t Q)
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != b && N > 0);
    assert((nullptr != x && P > 0) || (nullptr != y && Q > 0));
    assert(nullptr == x || P == 0 || nullptr == y || Q == 0 || y >= x + P || x >= y + Q); // 避免区域交叉覆盖
    assert(!is_zero(b, N)); // 被除数不能为0

    // 常量
    const size_t dividend_len = unsigned_significant_size(a, M);
    const size_t divisor_len = unsigned_significant_size(b, N);
    const size_t quotient_len = std::min(P, dividend_len);

    // 避免数据在计算中途被破坏
    T *quotient = x; // 商，可以为 nullptr
    if ((a - P < x && x < a) || (b - P < x && x < b + N)) // 兼容 x==a 的情况
        quotient = (T*) ::malloc(sizeof(T) * quotient_len);
    T *remainder = y; // 余数，不能为 nullptr
    if (nullptr == y || Q < divisor_len || (a - Q < y && y < a + M) || (b - Q < y && y < b + N))
        remainder = (T*) ::malloc(sizeof(T) * divisor_len);

    // 逐位试商
    ::memset(remainder, 0, sizeof(T) * divisor_len); // 初始化余数
    bool remainder_positive = true;
    for (size_t i = 0; i < dividend_len; ++i)
    {
        const size_t dividend_word_pos = dividend_len - i - 1;
        const T next_dividend_word = a[dividend_word_pos]; // 余数左移时的低位补位部分
        if (nullptr != quotient && dividend_word_pos < P)
            quotient[dividend_word_pos] = 0; // 初始化商，注意，兼容 x==a 的情况

        for (size_t j = 0; j < 8 * sizeof(T); ++j)
        {
            // 余数左移1位
            unsigned_shift_left(remainder, divisor_len, remainder, divisor_len, 1);
            remainder[0] |= (next_dividend_word >> (8 * sizeof(T) - 1 - j)) & 0x01;

            // 加上/减去除数
            if (remainder_positive)
                unsigned_sub(remainder, divisor_len, b, divisor_len, remainder, divisor_len);
            else
                unsigned_add(remainder, divisor_len, b, divisor_len, remainder, divisor_len);

            // 试商结果
            remainder_positive = is_positive(remainder, divisor_len);
            if (remainder_positive && nullptr != quotient && dividend_word_pos < P)
                quotient[dividend_word_pos] |= (1 << (8 * sizeof(T) - 1 - j));
        }
    }

    // 商
    if (nullptr != x)
        unsigned_expand(quotient, quotient_len, x, P);

    /**
        恢复余数:
        如果未除尽且余数符号与被除数不一致，余数需加上除数
    */
    if (nullptr != y)
    {
        if (!is_zero(remainder, divisor_len) && !remainder_positive)
            unsigned_add(remainder, divisor_len, b, divisor_len, remainder, divisor_len);
        unsigned_expand(remainder, divisor_len, y, Q);
    }

    // 释放空间
    if (quotient != x)
        ::free(quotient);
    if (remainder != y)
        ::free(remainder);
}

}

#endif /* head file guarder */
