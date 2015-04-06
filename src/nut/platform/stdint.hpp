/**
 * @file -
 * @author jingqi
 * @date 2012-03-25
 * @last-edit 2012-03-25 19:46:52 jingqi
 */

#ifndef ___HEADFILE_784AE107_1B8B_4833_ADE0_1C516A19FBE7_
#define ___HEADFILE_784AE107_1B8B_4833_ADE0_1C516A19FBE7_

#include "platform.h"

#include <stdint.h>

#if defined(NUT_PLATFORM_BITS_64)
#   if defined(NUT_PLATFORM_OS_WINDOWS)
typedef unsigned __int128 uint128_t;
typedef signed __int128 int128_t;
#   else
typedef __uint128_t uint128_t;
typedef __int128_t int128_t;
#   endif
#endif

namespace nut
{

template <typename T>
struct StdInt
{};

template <>
struct StdInt<uint8_t>
{
    typedef uint8_t type;
    typedef uint8_t unsigned_type;
    typedef int8_t signed_type;

    typedef uint16_t double_type;
    typedef uint16_t double_unsigned_type;
    typedef int16_t double_signed_type;
};

template <>
struct StdInt<int8_t>
{
    typedef int8_t type;
    typedef uint8_t unsigned_type;
    typedef int8_t signed_type;

    typedef int16_t double_type;
    typedef uint16_t double_unsigned_type;
    typedef int16_t double_signed_type;
};

template <>
struct StdInt<uint16_t>
{
    typedef uint8_t half_type;
    typedef uint8_t half_unsigned_type;
    typedef int8_t half_signed_type;

    typedef uint16_t type;
    typedef uint16_t unsigned_type;
    typedef int16_t signed_type;

    typedef uint32_t double_type;
    typedef uint32_t double_unsigned_type;
    typedef int32_t double_signed_type;
};

template <>
struct StdInt<int16_t>
{
    typedef int8_t half_type;
    typedef uint8_t half_unsigned_type;
    typedef int8_t half_signed_type;

    typedef int16_t type;
    typedef uint16_t unsigned_type;
    typedef int16_t signed_type;

    typedef int32_t double_type;
    typedef uint32_t double_unsigned_type;
    typedef int32_t double_signed_type;
};

template <>
struct StdInt<uint32_t>
{
    typedef uint16_t half_type;
    typedef uint16_t half_unsigned_type;
    typedef int16_t half_signed_type;

    typedef uint32_t type;
    typedef uint32_t unsigned_type;
    typedef int32_t signed_type;

    typedef uint64_t double_type;
    typedef uint64_t double_unsigned_type;
    typedef int64_t double_signed_type;
};

template <>
struct StdInt<int32_t>
{
    typedef int16_t half_type;
    typedef uint16_t half_unsigned_type;
    typedef int16_t half_signed_type;

    typedef int32_t type;
    typedef uint32_t unsigned_type;
    typedef int32_t signed_type;

    typedef int64_t double_type;
    typedef uint64_t double_unsigned_type;
    typedef int64_t double_signed_type;
};

template <>
struct StdInt<uint64_t>
{
    typedef uint32_t half_type;
    typedef uint32_t half_unsigned_type;
    typedef int32_t half_signed_type;

    typedef uint64_t type;
    typedef uint64_t unsigned_type;
    typedef int64_t signed_type;

#if defined(NUT_PLATFORM_BITS_64)
    typedef uint128_t double_type;
    typedef uint128_t double_unsigned_type;
    typedef int128_t double_signed_type;
#endif
};

template <>
struct StdInt<int64_t>
{
    typedef int32_t half_type;
    typedef uint32_t half_unsigned_type;
    typedef int32_t half_signed_type;

    typedef int64_t type;
    typedef uint64_t unsigned_type;
    typedef int64_t signed_type;

#if defined(NUT_PLATFORM_BITS_64)
    typedef int128_t double_type;
    typedef uint128_t double_unsigned_type;
    typedef int128_t double_signed_type;
#endif
};

#if defined(NUT_PLATFORM_BITS_64)
template <>
struct StdInt<uint128_t>
{
    typedef uint64_t half_type;
    typedef uint64_t half_unsigned_type;
    typedef int64_t half_signed_type;

    typedef uint128_t type;
    typedef uint128_t unsigned_type;
    typedef int128_t signed_type;
};

template <>
struct StdInt<int128_t>
{
    typedef int64_t half_type;
    typedef uint64_t half_unsigned_type;
    typedef int64_t half_signed_type;

    typedef int128_t type;
    typedef uint128_t unsigned_type;
    typedef int128_t signed_type;
};
#endif

}

#endif /* head file guarder */

