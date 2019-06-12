
#ifndef ___HEADFILE_5ACD2973_2216_4813_AA66_5269FE866B87_
#define ___HEADFILE_5ACD2973_2216_4813_AA66_5269FE866B87_

#include <assert.h>
#include <string.h> // for memset(), memcpy(), memmove()
#include <type_traits>


namespace nut
{

template <typename T>
constexpr bool is_positive(const T *a, size_t N) noexcept;

template <typename T>
void _signed_shift_left_word(const T *a, size_t M, T *x, size_t N, size_t word_count) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

    if (x + word_count == a)
    {
        ::memset(x, 0, sizeof(T) * word_count);
        if (x + N > a + M)
        {
            const int fill = (is_positive(a, M) ? 0 : 0xff);
            ::memset(x + word_count + M, fill, sizeof(T) * (N - M - word_count));
        }
        return;
    }

    const T fill = (is_positive(a, M) ? 0 : ~(T)0);
    if (x + word_count < a)
    {
        for (size_t i = 0; i < N; ++i)
        {
            const ssize_t off = (ssize_t) i - (ssize_t) word_count;
            x[i] = (off < 0 ? 0 : (off >= (ssize_t) M ? fill : a[off]));
        }
    }
    else
    {
        for (ssize_t i = N - 1; i >= 0; --i)
        {
            const ssize_t off = i - (ssize_t) word_count;
            x[i] = (off < 0 ? 0 : (off >= (ssize_t) M ? fill : a[off]));
        }
    }
}

/**
 * (有符号数)左移
 * x<N> = a<N> << count
 */
template <typename T>
void signed_shift_left(const T *a, size_t M, T *x, size_t N, size_t bit_count) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

    const size_t words_off = bit_count / (8 * sizeof(T));
    const unsigned bits_off = bit_count % (8 * sizeof(T));
    if (0 == bits_off)
    {
        _signed_shift_left_word(a, M, x, N, words_off);
        return;
    }

    const unsigned inv_bits_off = 8 * sizeof(T) - bits_off;
    const T fill = (is_positive(a, M) ? 0 : ~(T)0);
    if (x + words_off < a)
    {
        for (size_t i = 0; i < N; ++i)
        {
            const ssize_t high_off = (ssize_t) i - (ssize_t) words_off,
                low_off = high_off - 1;
            const T high = (high_off < 0 ? 0 : (high_off >= (ssize_t) M ? fill :
                    a[high_off])) << bits_off;
            const T low = (low_off < 0 ? 0 : (low_off >= (ssize_t) M ? fill :
                    a[low_off])) >> inv_bits_off;
            x[i] = high | low;
        }
    }
    else
    {
        for (ssize_t i = N - 1; i >= 0; --i)
        {
            const ssize_t high_off = i - (ssize_t) words_off,
                low_off = high_off - 1;
            const T high = (high_off < 0 ? 0 : (high_off >= (ssize_t) M ? fill :
                    a[high_off])) << bits_off;
            const T low = (low_off < 0 ? 0 : (low_off >= (ssize_t) M ? fill :
                    a[low_off])) >> inv_bits_off;
            x[i] = high | low;
        }
    }
}

template <typename T>
void _unsigned_shift_left_word(const T *a, size_t M, T *x, size_t N, size_t word_count) noexcept
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
        {
            const ssize_t off = (ssize_t) i - (ssize_t) word_count;
            x[i] = (off < 0 ? 0 : (off >= (ssize_t) M ? 0 : a[off]));
        }
    }
    else
    {
        for (ssize_t i = N - 1; i >= 0; --i)
        {
            const ssize_t off = i - (ssize_t) word_count;
            x[i] = (off < 0 ? 0 : (off >= (ssize_t) M ? 0 : a[off]));
        }
    }
}

/**
 * (无符号数)左移
 * x<N> = a<N> << count
 */
template <typename T>
void unsigned_shift_left(const T *a, size_t M, T *x, size_t N, size_t bit_count) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

    const size_t words_off = bit_count / (8 * sizeof(T));
    const unsigned bits_off = bit_count % (8 * sizeof(T));
    if (0 == bits_off)
    {
        _unsigned_shift_left_word(a, M, x, N, words_off);
        return;
    }

    const unsigned inv_bits_off = 8 * sizeof(T) - bits_off;
    if (x + words_off < a)
    {
        for (size_t i = 0; i < N; ++i)
        {
            const ssize_t high_off = (ssize_t) i - (ssize_t) words_off,
                low_off = high_off - 1;
            const T high = (high_off < 0 ? 0 : (high_off >= (ssize_t) M ? 0 :
                    a[high_off]) << bits_off);
            const T low = (low_off < 0 ? 0 : (low_off >= (ssize_t) M ? 0 :
                    a[low_off]) >> inv_bits_off);
            x[i] = high | low;
        }
    }
    else
    {
        for (ssize_t i = N - 1; i >= 0; --i)
        {
            const ssize_t high_off = i - (ssize_t) words_off,
                low_off = high_off - 1;
            const T high = (high_off < 0 ? 0 : (high_off >= (ssize_t) M ? 0 :
                    a[high_off]) << bits_off);
            const T low = (low_off < 0 ? 0 : (low_off >= (ssize_t) M ? 0 :
                    a[low_off]) >> inv_bits_off);
            x[i] = high | low;
        }
    }
}

template <typename T>
void _signed_shift_right_word(const T *a, size_t M, T *x, size_t N, size_t word_count) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

    if (x == a + word_count)
    {
        if (x + N > a + M)
        {
            const int fill = (is_positive(a, M) ? 0 : 0xff);
            ::memset(x - word_count + M, fill, sizeof(T) * (word_count + N - M));
        }
        return;
    }

    const T fill = (is_positive(a, M) ? 0 : ~(T)0);
    if (x < a + word_count)
    {
        for (size_t i = 0; i < N; ++i)
        {
            const size_t off = i + word_count;
            x[i] = (off >= M ? fill : a[off]);
        }
    }
    else
    {
        for (ssize_t i = N - 1; i >= 0; --i)
        {
            const size_t off = i + word_count;
            x[i] = (off >= M ? fill : a[off]);
        }
    }
}

/**
 * (有符号数)右移
 * x<N> = a<N> >> count
 */
template <typename T>
void signed_shift_right(const T *a, size_t M, T *x, size_t N, size_t bit_count) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

    const size_t words_off = bit_count / (8 * sizeof(T));
    const unsigned bits_off = bit_count % (8 * sizeof(T));
    if (0 == bits_off)
    {
        _signed_shift_right_word(a, M, x, N, words_off);
        return;
    }

    const unsigned inv_bits_off = 8 * sizeof(T) - bits_off;
    const T fill = (is_positive(a, M) ? 0 : ~(T)0);
    if (x <= a + words_off)
    {
        for (size_t i = 0; i < N; ++i)
        {
            const size_t low_off = i + words_off, high_off = low_off + 1;
            const T low = (low_off >= M ? fill : a[low_off]) >> bits_off;
            const T high = (high_off >= M ? fill : a[high_off]) << inv_bits_off;
            x[i] = high | low;
        }
    }
    else
    {
        for (ssize_t i = N - 1; i >= 0; --i)
        {
            const size_t low_off = i + words_off, high_off = low_off + 1;
            const T low = (low_off >= M ? fill : a[low_off]) >> bits_off;
            const T high = (high_off >= M ? fill : a[high_off]) << inv_bits_off;
            x[i] = high | low;
        }
    }
}

template <typename T>
void _unsigned_shift_right_word(const T *a, size_t M, T *x, size_t N, size_t word_count) noexcept
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
        {
            const size_t off = i + word_count;
            x[i] = (off >= M ? 0 : a[off]);
        }
    }
    else
    {
        for (ssize_t i = N - 1; i >= 0; --i)
        {
            const size_t off = i + word_count;
            x[i] = (off >= M ? 0 : a[off]);
        }
    }
}

/**
 * (无符号数)右移
 * x<N> = a<N> >> count
 */
template <typename T>
void unsigned_shift_right(const T *a, size_t M, T *x, size_t N, size_t bit_count) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && M > 0 && nullptr != x && N > 0);

    const size_t words_off = bit_count / (8 * sizeof(T));
    const unsigned bits_off = bit_count % (8 * sizeof(T));
    if (0 == bits_off)
    {
        _unsigned_shift_right_word(a, M, x, N, words_off);
        return;
    }

    const unsigned inv_bits_off = 8 * sizeof(T) - bits_off;
    if (x <= a + words_off)
    {
        for (size_t i = 0; i < N; ++i)
        {
            const size_t low_off = i + words_off, high_off = low_off + 1;
            const T low = (low_off >= M ? 0 : a[low_off] >> bits_off);
            const T high = (high_off >= M ? 0 : a[high_off] << inv_bits_off);
            x[i] = high | low;
        }
    }
    else
    {
        for (ssize_t i = N - 1; i >= 0; --i)
        {
            const size_t low_off = i + words_off, high_off = low_off + 1;
            const T low = (low_off >= M ? 0 : a[low_off] >> bits_off);
            const T high = (high_off >= M ? 0 : a[high_off] << inv_bits_off);
            x[i] = high | low;
        }
    }
}

}

#endif
