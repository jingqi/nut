/**
 * @file -
 * @author jingqi
 * @date 2012-03-25
 * @last-edit 2012-03-25 19:46:52 jingqi
 */

#ifndef ___HEADFILE_784AE107_1B8B_4833_ADE0_1C516A19FBE7_
#define ___HEADFILE_784AE107_1B8B_4833_ADE0_1C516A19FBE7_

#include "platform.hpp"

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

template <typename T>
struct HalfSize
{};

template <>
struct HalfSize<uint16_t>
{
    typedef uint8_t half_type;
    typedef uint8_t half_unsigned_type;
    typedef int8_t half_signed_type;
};

template <>
struct HalfSize<int16_t>
{
    typedef int8_t half_type;
    typedef uint8_t half_unsigned_type;
    typedef int8_t half_signed_type;
};

template <>
struct HalfSize<uint32_t>
{
    typedef uint16_t half_type;
    typedef uint16_t half_unsigned_type;
    typedef int16_t half_signed_type;
};

template <>
struct HalfSize<int32_t>
{
    typedef int16_t half_type;
    typedef uint16_t half_unsigned_type;
    typedef int16_t half_signed_type;
};

template <>
struct HalfSize<uint64_t>
{
    typedef uint32_t half_type;
    typedef uint32_t half_unsigned_type;
    typedef int32_t half_signed_type;
};

template <>
struct HalfSize<int64_t>
{
    typedef int32_t half_type;
    typedef uint32_t half_unsigned_type;
    typedef int32_t half_signed_type;
};

#if defined(NUT_PLATFORM_BITS_64)
template <>
struct HalfSize<uint128_t>
{
    typedef uint64_t half_type;
    typedef uint64_t half_unsigned_type;
    typedef int64_t half_signed_type;
};

template <>
struct HalfSize<int128_t>
{
    typedef int64_t half_type;
    typedef uint64_t half_unsigned_type;
    typedef int64_t half_signed_type;
};
#endif

template <typename T>
struct DoubleSize
{};

template <>
struct DoubleSize<uint8_t>
{
    typedef uint16_t double_type;
    typedef uint16_t double_unsigned_type;
    typedef int16_t double_signed_type;
};

template <>
struct DoubleSize<int8_t>
{
    typedef int16_t double_type;
    typedef uint16_t double_unsigned_type;
    typedef int16_t double_signed_type;
};

template <>
struct DoubleSize<uint16_t>
{
    typedef uint32_t double_type;
    typedef uint32_t double_unsigned_type;
    typedef int32_t double_signed_type;
};

template <>
struct DoubleSize<int16_t>
{
    typedef int32_t double_type;
    typedef uint32_t double_unsigned_type;
    typedef int32_t double_signed_type;
};

template <>
struct DoubleSize<uint32_t>
{
    typedef uint64_t double_type;
    typedef uint64_t double_unsigned_type;
    typedef int64_t double_signed_type;
};

template <>
struct DoubleSize<int32_t>
{
    typedef int64_t double_type;
    typedef uint64_t double_unsigned_type;
    typedef int64_t double_signed_type;
};

#if defined(NUT_PLATFORM_BITS_64)
template <>
struct DoubleSize<uint64_t>
{
    typedef uint128_t double_type;
    typedef uint128_t double_unsigned_type;
    typedef int128_t double_signed_type;
};

template <>
struct DoubleSize<int64_t>
{
    typedef int128_t double_type;
    typedef uint128_t double_unsigned_type;
    typedef int128_t double_signed_type;
};
#endif

#endif /* head file guarder */

