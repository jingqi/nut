/**
 * See:
 *    https://gist.github.com/panzi/6856583
 */

#ifndef ___HEADFILE_BAE94271_DA1F_4411_AC7D_7734CBCA0672_
#define ___HEADFILE_BAE94271_DA1F_4411_AC7D_7734CBCA0672_

#include <stdint.h>
#include <stdlib.h> // for Windows _byteswap_ushort() and so on

#include "platform.h"


/**
 * 字节序交换
 *    bswap_uint16()
 *    bswap_int16()
 *    bswap_uint32()
 *    bswap_int32()
 *    bswap_uint64()
 *    bswap_int64()
 */
namespace nut
{

// swap endian
inline uint16_t bswap_uint16(uint16_t val)
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap16(val);
#elif NUT_PLATFORM_CC_VC
    static_assert(sizeof(unsigned short) == sizeof(uint16_t), "Unexpected 'unsigned short' size");
    return _byteswap_ushort(val);
#else
    return (val << 8) | (val >> 8);
#endif
}

// swap endian
inline int16_t bswap_int16(int16_t val)
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap16(val);
#elif NUT_PLATFORM_CC_VC
    static_assert(sizeof(unsigned short) == sizeof(int16_t), "Unexpected 'unsigned short' size");
    return (short) _byteswap_ushort((unsigned short) val);
#else
    return (val << 8) | ((val >> 8) & 0xFF);
#endif
}

// swap endian
inline uint32_t bswap_uint32(uint32_t val)
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap32(val);
#elif NUT_PLATFORM_CC_VC
    static_assert(sizeof(unsigned long) == sizeof(uint32_t), "Unexpected 'unsigned long' size");
    return _byteswap_ulong(val);
#else
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
#endif
}

// swap endian
inline int32_t bswap_int32(int32_t val)
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap32(val);
#elif NUT_PLATFORM_CC_VC
    static_assert(sizeof(unsigned long) == sizeof(int32_t), "Unexpected 'unsigned long' size");
    return (long) _byteswap_ulong((unsigned long) val);
#else
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | ((val >> 16) & 0xFFFF);
#endif
}

// swap endian
inline uint64_t bswap_uint64(uint64_t val)
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap64(val);
#elif NUT_PLATFORM_CC_VC
    return _byteswap_uint64(val);
#else
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | (val >> 32);
#endif
}

// swap endian
inline int64_t bswap_int64(int64_t val)
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap64(val);
#elif NUT_PLATFORM_CC_VC
    return (int64_t) _byteswap_uint64((uint64_t) val);
#else
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);
#endif
}

}


/**
 * 判断主机字节序
 */
namespace nut
{

inline bool is_little_endian()
{
    const uint16_t word = 0x0001;
    return 0 != ((const uint8_t*) &word)[0];
}

inline bool is_big_endian()
{
    return !is_little_endian();
}

}


/**
 * 字节序转换
 *
 * host-endian to little-endian:
 *    htole16()
 *    htole32()
 *    htole64()
 *
 * little-endian to host-endian:
 *    le16toh()
 *    le32toh()
 *    le64toh()
 *
 * host-endian to big-endian:
 *    htobe16()
 *    htobe32()
 *    htobe64()
 *
 * big-endian to host-endian:
 *    be16toh()
 *    be32toh()
 *    be64toh()
 */
#if NUT_PLATFORM_OS_WINDOWS

namespace nut
{

// htole16() / le16toh()
inline uint16_t _htole16(uint16_t x)
{
    if (is_little_endian())
        return x;
    else
        return nut::bswap_uint16(x);
}

// htole32() / le32toh()
inline uint32_t _htole32(uint32_t x)
{
    if (is_little_endian())
        return x;
    else
        return nut::bswap_uint32(x);
}

// htole64() / le64toh()
inline uint64_t _htole64(uint64_t x)
{
    if (is_little_endian())
        return x;
    else
        return nut::bswap_uint64(x);
}

// htobe16() / be16toh()
inline uint16_t _htobe16(uint16_t x)
{
    if (is_little_endian())
        return nut::bswap_uint16(x);
    else
        return x;
}

// htobe32() / be32toh()
inline uint32_t _htobe32(uint32_t x)
{
    if (is_little_endian())
        return nut::bswap_uint32(x);
    else
        return x;
}

// htobe64() / be64toh()
inline uint64_t _htobe64(uint64_t x)
{
    if (is_little_endian())
        return nut::bswap_uint64(x);
    else
        return x;
}

}

#   define htole16(x) nut::_htole16((uint16_t) (x))
#   define htole32(x) nut::_htole32((uint32_t) (x))
#   define htole64(x) nut::_htole64((uint64_t) (x))

#   define le16toh(x) nut::_htole16((uint16_t) (x))
#   define le32toh(x) nut::_htole32((uint32_t) (x))
#   define le64toh(x) nut::_htole64((uint64_t) (x))

#   define htobe16(x) nut::_htobe16((uint16_t) (x))
#   define htobe32(x) nut::_htobe32((uint32_t) (x))
#   define htobe64(x) nut::_htobe64((uint64_t) (x))

#   define be16toh(x) nut::_htobe16((uint16_t) (x))
#   define be32toh(x) nut::_htobe32((uint32_t) (x))
#   define be64toh(x) nut::_htobe64((uint64_t) (x))

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

#endif
