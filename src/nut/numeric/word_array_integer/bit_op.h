
#ifndef ___HEADFILE_080FFB26_370A_4C5A_A93C_0BEE7E80BAFC_
#define ___HEADFILE_080FFB26_370A_4C5A_A93C_0BEE7E80BAFC_

#include <assert.h>
#include <string.h> // for memset(), memcpy(), memmove()
#include <type_traits>

#include "../../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <malloc.h> // for ::alloca()
#else
#   include <alloca.h>
#endif

#include "../../nut_config.h"
#include "../../platform/int_type.h"


namespace nut
{

/**
 * 按位与
 * x<N> = a<N> & b<N>
 */
template <typename T>
void bit_and(const T *a, const T *b, T *x, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && nullptr != b && nullptr != x && N > 0);

#if 0 // unoptimized
    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (T*) ::alloca(sizeof(T) * N);

    for (size_t i = 0; i < N; ++i)
        retx[i] = a[i] & b[i];

    // 回写数据
    if (retx != x)
        ::memcpy(x, retx, sizeof(T) * N);
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
        T *const retx = (T*) ::alloca(sizeof(T) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = a[i] & b[i];

        // 回写数据
        ::memcpy(x, retx, sizeof(T) * N);
    }
#endif
}

/**
 * 按位或
 * x<N> = a<N> | b<N>
 */
template <typename T>
void bit_or(const T *a, const T *b, T *x, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && nullptr != b && nullptr != x && N > 0);

#if 0 // unoptimized
    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (T*) ::alloca(sizeof(T) * N);

    for (size_t i = 0; i < N; ++i)
        retx[i] = a[i] | b[i];

    // 回写数据
    if (retx != x)
        ::memcpy(x, retx, sizeof(T) * N);
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
        T *const retx = (T*) ::alloca(sizeof(T) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = a[i] | b[i];

        // 回写数据
        ::memcpy(x, retx, sizeof(T) * N);
    }
#endif
}

/**
 * 按位异或
 * x<N> = a<N> ^ b<N>
 */
template <typename T>
void bit_xor(const T *a, const T *b, T *x, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && nullptr != b && nullptr != x && N > 0);

#if 0 // unoptimized
    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (T*) ::alloca(sizeof(T) * N);

    for (size_t i = 0; i < N; ++i)
        retx[i] = a[i] ^ b[i];

    // 回写数据
    if (retx != x)
        ::memcpy(x, retx, sizeof(T) * N);
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
        T *const retx = (T*) ::alloca(sizeof(T) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = a[i] ^ b[i];

        // 回写数据
        ::memcpy(x, retx, sizeof(T) * N);
    }
#endif
}

/**
 * 按位同或
 * x<N> = ~(a<N> ^ b<N>)
 */
template <typename T>
void bit_nxor(const T *a, const T *b, T *x, size_t N) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    assert(nullptr != a && nullptr != b && nullptr != x && N > 0);

#if 0 // unoptimized
    // 避免区域交叉覆盖
    T *retx = x;
    if ((a < x && x < a + N) || (b < x && x < b + N))
        retx = (T*) ::alloca(sizeof(T) * N);

    for (size_t i = 0; i < N; ++i)
        retx[i] = ~(a[i] ^ b[i]);

    // 回写数据
    if (retx != x)
        ::memcpy(x, retx, sizeof(T) * N);
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
        T *const retx = (T*) ::alloca(sizeof(T) * N);

        for (size_t i = 0; i < N; ++i)
            retx[i] = ~(a[i] ^ b[i]);

        // 回写数据
        ::memcpy(x, retx, sizeof(T) * N);
    }
#endif
}

/**
 * 按位取反
 * x<N> = ~a<N>
 */
template <typename T>
void bit_not(const T *a, T *x, size_t N) noexcept
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
NUT_API unsigned bit1_count(uint8_t a) noexcept;
NUT_API unsigned bit1_count(uint16_t a) noexcept;
NUT_API unsigned bit1_count(uint32_t a) noexcept;
NUT_API unsigned bit1_count(uint64_t a) noexcept;
#if NUT_HAS_INT128
NUT_API unsigned bit1_count(uint128_t a) noexcept;
#endif

NUT_API size_t bit1_count(const uint8_t *a, size_t N) noexcept;

/**
 * 统计 bit 0 数目
 */
NUT_API size_t bit0_count(const uint8_t *a, size_t N) noexcept;

/**
 * bit 逆序化，eg. 10110101 -> 10101101
 */
NUT_API uint8_t reverse_bits(uint8_t v) noexcept;
NUT_API uint16_t reverse_bits(uint16_t v) noexcept;
NUT_API uint32_t reverse_bits(uint32_t v) noexcept;
NUT_API uint64_t reverse_bits(uint64_t v) noexcept;
#if NUT_HAS_INT128
NUT_API uint128_t reverse_bits(uint128_t v) noexcept;
#endif

/**
 * 返回从低位到高位第一个 bit 1 的位置
 *
 * @return -1 if not found
 *         >=0 if found
 */
NUT_API int lowest_bit1(uint8_t a) noexcept;
NUT_API int lowest_bit1(uint16_t a) noexcept;
NUT_API int lowest_bit1(uint32_t a) noexcept;
NUT_API int lowest_bit1(uint64_t a) noexcept;
#if NUT_HAS_INT128
NUT_API int lowest_bit1(uint128_t a) noexcept;
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
ssize_t lowest_bit1(const T *a, size_t N) noexcept
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
ssize_t lowest_bit0(const T *a, size_t N) noexcept
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
NUT_API int highest_bit1(uint8_t a) noexcept;
NUT_API int highest_bit1(uint16_t a) noexcept;
NUT_API int highest_bit1(uint32_t a) noexcept;
NUT_API int highest_bit1(uint64_t a) noexcept;
#if NUT_HAS_INT128
NUT_API int highest_bit1(uint128_t a) noexcept;
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
ssize_t highest_bit1(const T *a, size_t N) noexcept
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
ssize_t highest_bit0(const T *a, size_t N) noexcept
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
size_t bit1_length(const T *a,  size_t N) noexcept
{
    return highest_bit1<T>(a, N) + 1;
}

/**
 * 负数 bit0 length, eg 11101000 -> 5
 */
template <typename T>
size_t bit0_length(const T *a, size_t N) noexcept
{
    return highest_bit0<T>(a, N) + 1;
}

}

#endif
