/**
 * See:
 *    https://gist.github.com/panzi/6856583
 *    https://sourceforge.net/p/predef/wiki/Endianness/
 *    https://www.boost.org/doc/libs/1_69_0/boost/predef/other/endian.h
 *    https://docs.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2012/b0084kay(v=vs.110)
 */

#ifndef ___HEADFILE_BAE94271_DA1F_4411_AC7D_7734CBCA0672_
#define ___HEADFILE_BAE94271_DA1F_4411_AC7D_7734CBCA0672_

#include <assert.h>
#include <stdint.h>
#include <stddef.h> // for size_t in Linux

#include "platform.h"

#if NUT_PLATFORM_OS_LINUX
#   include <endian.h> // for htole16() and so on
#elif NUT_PLATFORM_OS_MACOS
#	include <libkern/OSByteOrder.h> // for OSSwapHostToLittleInt16() and so on
#elif NUT_PLATFORM_OS_WINDOWS
#   if  NUT_PLATFORM_CC_MINGW
#       include <sys/param.h>
#   endif
#   include <stdlib.h> // for _byteswap_ushort() and so on in Windows
#elif defined(__OpenBSD__) || defined(__NetBSD__) || defined(__FreeBSD__) || defined(__DragonFly__)
#   include <sys/endian.h>
#endif


/**
 * 编译时字节序检测
 *    NUT_ENDIAN_LITTLE_BYTE
 *    NUT_ENDIAN_BIG_BYTE
 *    NUT_ENDIAN_LITTLE_WORD
 *    NUT_ENDIAN_BIG_WORD
 */
#if (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && (__BYTE_ORDER == __LITTLE_ENDIAN)) || \
    (defined(_BYTE_ORDER) && defined(_LITTLE_ENDIAN) && (_BYTE_ORDER == _LITTLE_ENDIAN)) || \
    (defined(BYTE_ORDER) && defined(LITTLE_ENDIAN) && (BYTE_ORDER == LITTLE_ENDIAN)) || \
    (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)) || \
    (defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)) ||         \
    (defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN)) ||               \
    defined(__ARMEL__) ||                                               \
    defined(__THUMBEL__) ||                                             \
    defined(__AARCH64EL__) ||                                           \
    defined(_MIPSEL) ||                                                 \
    defined(__MIPSEL) ||                                                \
    defined(__MIPSEL__) ||                                              \
    (NUT_PLATFORM_CC_VC && (defined(_M_IX86) || defined(_M_X64) || defined(_M_IA64) || defined(_M_ARM)))
#   define NUT_ENDIAN_LITTLE_BYTE 1
#   define NUT_ENDIAN_BIG_BYTE 0
#   define NUT_ENDIAN_LITTLE_WORD 0
#   define NUT_ENDIAN_BIG_WORD 0
#elif (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && (__BYTE_ORDER == __BIG_ENDIAN)) || \
    (defined(_BYTE_ORDER) && defined(_BIG_ENDIAN) && (_BYTE_ORDER == _BIG_ENDIAN)) || \
    (defined(BYTE_ORDER) && defined(BIG_ENDIAN) && (BYTE_ORDER == BIG_ENDIAN)) || \
    (defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)) || \
    (defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__)) ||         \
    (defined(_BIG_ENDIAN) && !defined(_LITTLE_ENDIAN)) ||               \
    defined(__ARMEB__) ||                                               \
    defined(__THUMBEB__) ||                                             \
    defined(__AARCH64EB__) ||                                           \
    defined(_MIPSEB) ||                                                 \
    defined(__MIPSEB) ||                                                \
    defined(__MIPSEB__) ||                                              \
    (NUT_PLATFORM_CC_VC && defined(_M_PPC))
#   define NUT_ENDIAN_LITTLE_BYTE 0
#   define NUT_ENDIAN_BIG_BYTE 1
#   define NUT_ENDIAN_LITTLE_WORD 0
#   define NUT_ENDIAN_BIG_WORD 0
#elif (defined(__BYTE_ORDER) && defined(__PDP_ENDIAN) && (__BYTE_ORDER == __PDP_ENDIAN)) || \
    (defined(_BYTE_ORDER) && defined(_PDP_ENDIAN) && (_BYTE_ORDER == _PDP_ENDIAN)) || \
    (defined(BYTE_ORDER) && defined(PDP_ENDIAN) && (BYTE_ORDER == PDP_ENDIAN)) || \
    (defined(__FLOAT_WORD_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && (__FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__))
#   define NUT_ENDIAN_LITTLE_BYTE 0
#   define NUT_ENDIAN_BIG_BYTE 0
#   define NUT_ENDIAN_LITTLE_WORD 1
#   define NUT_ENDIAN_BIG_WORD 0
#elif (defined(__FLOAT_WORD_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && (__FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__))
#   define NUT_ENDIAN_LITTLE_BYTE 0
#   define NUT_ENDIAN_BIG_BYTE 0
#   define NUT_ENDIAN_LITTLE_WORD 0
#   define NUT_ENDIAN_BIG_WORD 1
#else
#   error Unknown endian
#endif

/**
 * 字节序交换
 *    bswap_uint16()
 *    bswap_int16()
 *    bswap_uint32()
 *    bswap_int32()
 *    bswap_uint64()
 *    bswap_int64()
 *    bswap()
 *    wswap()
 */
namespace nut
{

// swap endian
inline uint16_t bswap_uint16(uint16_t val) noexcept
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap16(val);
#elif NUT_PLATFORM_CC_VC
    static_assert(sizeof(unsigned short) == sizeof(uint16_t), "Unexpected 'unsigned short' size");
    return _byteswap_ushort(val); // non-constexpr
#else
    return (val << 8) | (val >> 8);
#endif
}

// swap endian
inline int16_t bswap_int16(int16_t val) noexcept
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap16(val);
#elif NUT_PLATFORM_CC_VC
    static_assert(sizeof(unsigned short) == sizeof(int16_t), "Unexpected 'unsigned short' size");
    return (short) _byteswap_ushort((unsigned short) val); // non-constexpr
#else
    return (val << 8) | ((val >> 8) & 0xFF);
#endif
}

// swap endian
inline uint32_t bswap_uint32(uint32_t val) noexcept
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap32(val);
#elif NUT_PLATFORM_CC_VC
    static_assert(sizeof(unsigned long) == sizeof(uint32_t), "Unexpected 'unsigned long' size");
    return _byteswap_ulong(val); // non-constexpr
#else
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
#endif
}

// swap endian
inline int32_t bswap_int32(int32_t val) noexcept
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap32(val);
#elif NUT_PLATFORM_CC_VC
    static_assert(sizeof(unsigned long) == sizeof(int32_t), "Unexpected 'unsigned long' size");
    return (long) _byteswap_ulong((unsigned long) val); // non-constexpr
#else
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | ((val >> 16) & 0xFFFF);
#endif
}

// swap endian
inline uint64_t bswap_uint64(uint64_t val) noexcept
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap64(val);
#elif NUT_PLATFORM_CC_VC
    return _byteswap_uint64(val); // non-constexpr
#else
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | (val >> 32);
#endif
}

// swap endian
inline int64_t bswap_int64(int64_t val) noexcept
{
#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
    return __builtin_bswap64(val);
#elif NUT_PLATFORM_CC_VC
    return (int64_t) _byteswap_uint64((uint64_t) val); // non-constexpr
#else
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);
#endif
}

// Swap word order endian
template <typename T>
void wswap(T* dst, size_t size) noexcept
{
    assert(nullptr != dst || 0 == size);
    for (size_t i = 0, round = size / 2; i < round; ++i)
    {
        const T v = dst[i];
        dst[i] = dst[size - i - 1];
        dst[size - i - 1] = v;
    }
}

// Swap byte order endian
inline void bswap(void *dst, size_t cb) noexcept
{
    assert(nullptr != dst || 0 == cb);
    wswap<uint8_t>((uint8_t*) dst, cb);
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

#   if NUT_ENDIAN_LITTLE_BYTE

#   define htole16(x) (x)
#   define htole32(x) (x)
#   define htole64(x) (x)

#   define le16toh(x) (x)
#   define le32toh(x) (x)
#   define le64toh(x) (x)

#   define htobe16(x) nut::bswap_uint16((uint16_t) (x))
#   define htobe32(x) nut::bswap_uint32((uint32_t) (x))
#   define htobe64(x) nut::bswap_uint64((uint64_t) (x))

#   define be16toh(x) nut::bswap_uint16((uint16_t) (x))
#   define be32toh(x) nut::bswap_uint32((uint32_t) (x))
#   define be64toh(x) nut::bswap_uint64((uint64_t) (x))

#   elif NUT_ENDIAN_BIG_BYTE

#   define htole16(x) nut::bswap_uint16((uint16_t) (x))
#   define htole32(x) nut::bswap_uint32((uint32_t) (x))
#   define htole64(x) nut::bswap_uint64((uint64_t) (x))

#   define le16toh(x) nut::bswap_uint16((uint16_t) (x))
#   define le32toh(x) nut::bswap_uint32((uint32_t) (x))
#   define le64toh(x) nut::bswap_uint64((uint64_t) (x))

#   define htobe16(x) (x)
#   define htobe32(x) (x)
#   define htobe64(x) (x)

#   define be16toh(x) (x)
#   define be32toh(x) (x)
#   define be64toh(x) (x)

#   endif

#elif NUT_PLATFORM_OS_MACOS

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
