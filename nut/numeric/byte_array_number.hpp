/**
 * @file -
 * @author jingqi
 * @date 2011-12-17
 * @last-edit 2011-12-17 16:46:28 jingqi
 */

#ifndef ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_
#define ___HEADFILE_1C442178_8186_41B7_ACBC_AB8307B57A5E_

#include <assert.h>
#include <stdint.h>
#include <string.h> // for memset(), memcpy()

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_BITS_64)
#   define machine_hword_type uint32_t
#   define machine_word_type uint64_t
#   define machine_dword_type uint128_t
#elif defined(NUT_PLATFORM_BITS_32)
#   define machine_hword_type uint16_t
#   define machine_word_type uint32_t
#   define machine_dword_type uint64_t
#else
#   define machine_hword_type uint8_t
#   define machine_word_type uint16_t
#   define machine_dword_type uint32_t
#endif

namespace nut
{

/** 是否为0 */
inline bool isZero(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    for (register size_t i = 0; i < word_count; ++i)
        if (0 != reinterpret_cast<const machine_word_type*>(a)[i])
            return false;
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
        if (0 != a[i])
            return false;
    return true;
}

/** 是否为正数 */
inline bool signedIsPositive(const uint8_t *a, size_t N)
{
    assert(NULL != a && N > 0);
    return 0 == (a[N - 1] & 0x80);
}

/** 小于 */
inline bool unsignedLessThen(const uint8_t *a, const uint8_t *b, size_t N)
{
    assert(NULL != a && NULL != b && N > 0);
    const size_t int_count = N / sizeof(machine_word_type);
    for (register int i = N - 1, bound = int_count * sizeof(machine_word_type); i >= bound; --i)
        if (a[i] != b[i])
            return a[i] < b[i];
    for (register int i = int_count - 1; i >= 0; --i)
        if (reinterpret_cast<const machine_word_type*>(a)[i] != reinterpret_cast<const machine_word_type*>(b)[i])
            return reinterpret_cast<const machine_word_type*>(a)[i] < reinterpret_cast<const machine_word_type*>(b)[i];
    return false; // 相等
}

/** 小于 */
inline bool signedLessThen(const uint8_t *a, const uint8_t *b, size_t N)
{
    assert(NULL != a && NULL != b && N > 0);
    const bool positive1 = signedIsPositive(a, N), positive2 = signedIsPositive(b, N);
    if (positive1 != positive2)
        return positive2;

    return unsignedLessThen(a, b, N);
}

/** 带符号扩展(截断) */
inline void signedExpand(const uint8_t *a, size_t M, uint8_t *x, size_t N)
{
    assert(NULL != a && M > 0);
    assert(NULL != x && N > 0);
    ::memcpy(x, a, (M < N ? M : N));
    if (M < N)
        ::memset(x + M, (signedIsPositive(a, M) ? 0 : 0xFF), N - M);
}

/** 无符号扩展(截断) */
inline void unsignedExpand(const uint8_t *a, size_t M, uint8_t *x, size_t N)
{
    assert(NULL != a && M > 0);
    assert(NULL != x && N > 0);
    ::memcpy(x, a, (M < N ? M : N));
    if (M < N)
        ::memset(x + M, 0, N - M);
}

/** 相加 */
inline void add(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    uint8_t carry = 0;
    for (register size_t i = 0; i < word_count; ++i)
    {
        const machine_dword_type pluser1 = reinterpret_cast<const machine_word_type*>(a)[i];
        machine_dword_type pluser2 = reinterpret_cast<const machine_word_type*>(b)[i];
        pluser2 += pluser1 + carry;

        reinterpret_cast<machine_word_type*>(x)[i] = static_cast<machine_word_type>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(machine_word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
    {
        const uint16_t pluser1 = a[i];
        uint16_t pluser2 = b[i];
        pluser2 += pluser1 + carry;

        x[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
}

/** 相减 */
inline void sub(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    uint8_t carry = 1;
    for (register size_t i = 0; i < word_count; ++i)
    {
        const machine_dword_type pluser1 = reinterpret_cast<const machine_word_type*>(a)[i];
        machine_dword_type pluser2 = static_cast<machine_word_type>(~(reinterpret_cast<const machine_word_type*>(b)[i]));
        pluser2 += pluser1 + carry;

        reinterpret_cast<machine_word_type*>(x)[i] = static_cast<machine_word_type>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(machine_word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
    {
        uint16_t pluser1 = a[i];
        uint16_t pluser2 = static_cast<uint8_t>(~(b[i]));
        pluser2 += pluser1 + carry;

        x[i] = static_cast<uint8_t>(pluser2);
        carry = static_cast<uint8_t>(pluser2 >> (sizeof(uint8_t) * 8));
    }
}

/** 取反 */
inline void signedOpposite(const uint8_t *a, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    uint8_t carry = 1;
    for (register size_t i = 0; i < word_count; ++i)
    {
        machine_dword_type pluser1 = static_cast<machine_word_type>(~(reinterpret_cast<const machine_word_type*>(a)[i]));
        pluser1 += carry;

        reinterpret_cast<machine_word_type*>(x)[i] = static_cast<machine_word_type>(pluser1);
        carry = static_cast<uint8_t>(pluser1 >> (sizeof(machine_word_type) * 8));
    }
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
    {
        uint16_t pluser1 = static_cast<uint8_t>(~(a[i]));
        pluser1 += carry;

        x[i] = static_cast<uint8_t>(pluser1);
        carry = static_cast<uint8_t>(pluser1 >> (sizeof(uint8_t) * 8));
    }
}

/** 相乘 */
inline void multiply(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    uint8_t *ret = (uint8_t*)::malloc(N);
    ::memset(ret, 0, N);

    const size_t word_count = N / sizeof(machine_word_type);
    for (register size_t i = 0; i < word_count; ++i)
    {
        machine_word_type carry = 0;
        const machine_dword_type mult1 = reinterpret_cast<const machine_word_type*>(a)[i];
        if (mult1 == 0)
            continue;

        register size_t j = 0;
        for (; i + j < word_count; ++j)
        {
            machine_dword_type mult2 = reinterpret_cast<const machine_word_type*>(b)[j];
            mult2 = mult1 * mult2 + reinterpret_cast<machine_word_type*>(ret)[i + j] + carry;

            reinterpret_cast<machine_word_type*>(ret)[i + j] = static_cast<machine_word_type>(mult2);
            carry = static_cast<machine_word_type>(mult2 >> (sizeof(machine_word_type) * 8));
        }

        size_t bound = i * sizeof(machine_word_type);
        for (register size_t k = j * sizeof(machine_word_type); k + bound < N; ++k)
        {
            machine_dword_type mult2 = b[k];
            mult2 = mult1 * mult2 + ret[k + bound] + carry;

            ret[k + bound] = static_cast<uint8_t>(mult2);
            carry = static_cast<machine_word_type>(mult2 >> (sizeof(uint8_t) * 8));
        }
    }

    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
    {
        uint8_t carry = 0;
        const uint16_t mult1 = a[i];
        if (mult1 == 0)
            continue;

        for (register size_t j = 0; i + j < N; ++j)
        {
            uint16_t mult2 = b[j];
            mult2 = mult1 * mult2 + ret[i + j] + carry;

            ret[i + j] = static_cast<uint8_t>(mult2);
            carry = static_cast<uint8_t>(mult2 >> (sizeof(uint8_t) * 8));
        }
    }

    ::memcpy(x, ret, N);
    ::free(ret);
}

/**
 * 相除
 * @param x
 *      商
 * @param y
 *      余数
 */
inline void divide(const uint8_t *a, const uint8_t *b, uint8_t *x, uint8_t *y, size_t N)
{
    
}

/** 比特与 */
inline void and(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    for (register size_t i = 0; i < word_count; ++i)
        reinterpret_cast<machine_word_type*>(x)[i] = reinterpret_cast<const machine_word_type*>(a)[i] & reinterpret_cast<const machine_word_type*>(b)[i];
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
        x[i] = a[i] & b[i];
}

/** 比特或 */
inline void or(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    for (register size_t i = 0; i < word_count; ++i)
        reinterpret_cast<machine_word_type*>(x)[i] = reinterpret_cast<const machine_word_type*>(a)[i] | reinterpret_cast<const machine_word_type*>(b)[i];
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
        x[i] = a[i] | b[i];
}

/** 比特异或 */
inline void xor(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    for (register size_t i = 0; i < word_count; ++i)
        reinterpret_cast<machine_word_type*>(x)[i] = reinterpret_cast<const machine_word_type*>(a)[i] ^ reinterpret_cast<const machine_word_type*>(b)[i];
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
        x[i] = a[i] ^ b[i];
}

/** 比特同或 */
inline void sameOr(const uint8_t *a, const uint8_t *b, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != b && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    for (register size_t i = 0; i < word_count; ++i)
        reinterpret_cast<machine_word_type*>(x)[i] = ~(reinterpret_cast<const machine_word_type*>(a)[i] ^ reinterpret_cast<const machine_word_type*>(b)[i]);
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
        x[i] = ~(a[i] ^ b[i]);
}

/** 比特否 */
inline void not(const uint8_t *a, uint8_t *x, size_t N)
{
    assert(NULL != a && NULL != x && N > 0);
    const size_t word_count = N / sizeof(machine_word_type);
    for (register size_t i = 0; i < word_count; ++i)
        reinterpret_cast<machine_word_type*>(x)[i] = ~(reinterpret_cast<const machine_word_type*>(a)[i]);
    for (register size_t i = word_count * sizeof(machine_word_type); i < N; ++i)
        x[i] = ~(a[i]);
}

}

#undef machine_hword_type
#undef machine_word_type
#undef machine_dword_type

#endif /* head file guarder */

