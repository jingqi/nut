/**
 * See:
 *    https://gist.github.com/panzi/6856583
 */

#ifndef ___HEADFILE_BAE94271_DA1F_4411_AC7D_7734CBCA0672_
#define ___HEADFILE_BAE94271_DA1F_4411_AC7D_7734CBCA0672_

#include <stdint.h>

#include "platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   if NUT_PLATFORM_CC_MINGW
#       include <endian.h>
#   else

        static_assert(sizeof(short) == sizeof(int16_t), "整数长度问题");
        static_assert(sizeof(long) == sizeof(int32_t), "整数长度问题");
        static_assert(sizeof(long long) == sizeof(int64_t), "整数长度问题");

#       define LITTLE_ENDIAN    0x41424344UL
#       define BIG_ENDIAN       0x44434241UL
#       define PDP_ENDIAN       0x42414443UL
#       define ENDIAN_ORDER     ('ABCD')

#       if ENDIAN_ORDER == LITTLE_ENDIAN

#           define htole16(x) (x)
#           define htole32(x) (x)
#           define htole64(x) (x)

#           define le16toh(x) (x)
#           define le32toh(x) (x)
#           define le64toh(x) (x)

#           define htobe16(x) bswap_uint16((uint16_t) (x))
#           define htobe32(x) bswap_uint32((uint32_t) (x))
#           define htobe64(x) bswap_uint64((uint64_t) (x))

#           define be16toh(x) bswap_uint16((uint16_t) (x))
#           define be32toh(x) bswap_uint32((uint32_t) (x))
#           define be64toh(x) bswap_uint64((uint64_t) (x))

#       elif ENDIAN_ORDER == BIG_ENDIAN

            /* That would be xbox 360 */
#           define htole16(x) __builtin_bswap16(x)
#           define htole32(x) __builtin_bswap32(x)
#           define htole64(x) __builtin_bswap64(x)

#           define le16toh(x) __builtin_bswap16(x)
#           define le32toh(x) __builtin_bswap32(x)
#           define le64toh(x) __builtin_bswap64(x)

#           define htobe16(x) (x)
#           define htobe32(x) (x)
#           define htobe64(x) (x)

#           define be16toh(x) (x)
#           define be32toh(x) (x)
#           define be64toh(x) (x)

#       endif
#   endif
#elif NUT_PLATFORM_OS_MAC
#   include <libkern/OSByteOrder.h>

#   define htole16(x) OSSwapHostToLittleInt16(x)
#   define htole32(x) OSSwapHostToLittleInt32(x)
#   define htole64(x) OSSwapHostToLittleInt64(x)

#   define le16toh(x) OSSwapLittleToHostInt16(x)
#   define le32toh(x) OSSwapLittleToHostInt32(x)
#   define le64toh(x) OSSwapLittleToHostInt64(x)

#   define htobe16(x) OSSwapHostToBigInt16(x)
#   define htobe32(x) OSSwapHostToBigInt32(x)
#   define htobe64(x) OSSwapHostToBigInt64(x)

#   define be16toh(x) OSSwapBigToHostInt16(x)
#   define be32toh(x) OSSwapBigToHostInt32(x)
#   define be64toh(x) OSSwapBigToHostInt64(x)
#else
#   include <endian.h>
#endif

#if defined(betoh16) && !defined(be16toh)
#   define be16toh(x) betoh16(x)
#endif

#if defined(letoh16) && !defined(le16toh)
#   define le16toh(x) letoh16(x)
#endif

#if defined(betoh32) && !defined(be32toh)
#   define be32toh(x) betoh32(x)
#endif

#if defined(letoh32) && !defined(le32toh)
#   define le32toh(x) letoh32(x)
#endif

#if defined(betoh64) && !defined(be64toh)
#   define be64toh(x) betoh64(x)
#endif

#if defined(letoh64) && !defined(le64toh)
#   define le64toh(x) letoh64(x)
#endif


inline uint16_t bswap_uint16(uint16_t val)
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap16(val);
#else
    return (val << 8) | (val >> 8);
#endif
}

inline int16_t bswap_int16(int16_t val)
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap16(val);
#else
    return (val << 8) | ((val >> 8) & 0xFF);
#endif
}

inline uint32_t bswap_uint32(uint32_t val)
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap32(val);
#else
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
#endif
}

inline int32_t bswap_int32(int32_t val)
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap32(val);
#else
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | ((val >> 16) & 0xFFFF);
#endif
}

inline uint64_t bswap_uint64(uint64_t val)
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap64(val);
#else
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | (val >> 32);
#endif
}

inline int64_t bswap_int64(int64_t val)
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap64(val);
#else
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);
#endif
}


#endif
