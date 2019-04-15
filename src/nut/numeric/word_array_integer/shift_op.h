
#ifndef ___HEADFILE_5ACD2973_2216_4813_AA66_5269FE866B87_
#define ___HEADFILE_5ACD2973_2216_4813_AA66_5269FE866B87_

#include <assert.h>
#include <string.h> // for memset(), memcpy(), memmove()
#include <type_traits>


namespace nut
{

template <typename T>
bool is_positive(const T *a, size_t N);

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


}

#endif
