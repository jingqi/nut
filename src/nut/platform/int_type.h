
#ifndef ___HEADFILE_784AE107_1B8B_4833_ADE0_1C516A19FBE7_
#define ___HEADFILE_784AE107_1B8B_4833_ADE0_1C516A19FBE7_

#include <stdint.h>

#include "platform.h"

#if NUT_PLATFORM_OS_LINUX
#   include <sys/types.h> /* for ssize_t */
#elif NUT_PLATFORM_CC_VC
#   include <basetsd.h>
typedef SSIZE_T ssize_t;
#endif

#if NUT_PLATFORM_BITS_64
#   if NUT_PLATFORM_CC_VC
        /* FIXME 目前 vs2015 有 __int128 关键字的语法高亮，但是并不被编译器支持 */
#       define NUT_HAS_INT128 0
#   else
#       define NUT_HAS_INT128 1
typedef __uint128_t uint128_t;
typedef __int128_t int128_t;
#   endif
#else
#   define NUT_HAS_INT128 0
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

#if NUT_HAS_INT128
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

#if NUT_HAS_INT128
    typedef int128_t double_type;
    typedef uint128_t double_unsigned_type;
    typedef int128_t double_signed_type;
#endif
};

#if NUT_HAS_INT128
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
