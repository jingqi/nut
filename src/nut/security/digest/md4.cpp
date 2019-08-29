
#include <assert.h>
#include <string.h> /* for memcpy() */

#include "../../util/string/string_utils.h"
#include "md4.h"


#define __S11__ 3
#define __S12__ 7
#define __S13__ 11
#define __S14__ 19
#define __S21__ 3
#define __S22__ 5
#define __S23__ 9
#define __S24__ 13
#define __S31__ 3
#define __S32__ 9
#define __S33__ 11
#define __S34__ 15

#define __F__(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define __G__(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define __H__(x, y, z) ((x) ^ (y) ^ (z))

#define __ROTATE_LEFT__(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define __FF__(a, b, c, d, x, s)                   \
    do {                                           \
        (a) += __F__((b), (c), (d)) + (x);         \
        (a) = __ROTATE_LEFT__((a), (s));           \
    } while (false)

#define __GG__(a, b, c, d, x, s)                                   \
    do {                                                           \
        (a) += __G__((b), (c), (d)) + (x) + (uint32_t) 0x5a827999; \
        (a) = __ROTATE_LEFT__((a), (s));                           \
    } while (false)

#define __HH__(a, b, c, d, x, s)                                    \
    do {                                                            \
        (a) += __H__((b), (c), (d)) + (x) + (uint32_t) 0x6ed9eba1;  \
        (a) = __ROTATE_LEFT__((a), (s));                            \
    } while (false)

namespace nut
{

MD4::MD4() noexcept
{
    reset();
}

void MD4::reset() noexcept
{
    _bit_len = 0;

    /* Load magic initialization constants */
    _state[0] = 0x67452301;
    _state[1] = 0xefcdab89;
    _state[2] = 0x98badcfe;
    _state[3] = 0x10325476;

#if !NUT_ENDIAN_LITTLE_BYTE
    ::memset(_result, 0, DIGEST_SIZE);
#endif
}

void MD4::update(uint8_t byte) noexcept
{
    update(&byte, 1);
}

void MD4::update(const void *data, size_t cb) noexcept
{
    assert(nullptr != data || 0 == cb);

    /* Compute number of bytes mod 64 */
    unsigned index = (_bit_len >> 3) & 0x3f;
    const unsigned partlen = 64 - index;

    /* Update number of bits */
    _bit_len += cb << 3;

    /* Transform as many times as possible */
    size_t i = 0;
    if (cb >= partlen)
    {
        if (0 == index)
        {
            transform512bits(data);
        }
        else
        {
            ::memcpy(_buffer + index, data, partlen);
            transform512bits(_buffer);
        }

        for (i = partlen; i + 64 <= cb; i += 64)
            transform512bits(((const uint8_t*) data) + i);

        index = 0;
    }

    /* Buffer remaining input */
    ::memcpy(_buffer + index, ((const uint8_t*) data) + i, cb - i);
}

void MD4::digest() noexcept
{
    /* Pad out to 56 mod 64 */
    unsigned index = (_bit_len >> 3) & 0x3f;
    _buffer[index++] = 0x80;
    if (index < 56)
    {
        ::memset(_buffer + index, 0, 56 - index);
    }
    else if (index > 56)
    {
        if (index < 64)
            ::memset(_buffer + index, 0, 64 - index);
        transform512bits(_buffer);
        ::memset(_buffer, 0, 56);
    }

    /* Append bit length */
#if NUT_ENDIAN_LITTLE_BYTE
    *(uint64_t*)(_buffer + 56) = _bit_len;
#else
    *(uint64_t*)(_buffer + 56) = htole64(_bit_len);
#endif

    transform512bits(_buffer);

    /* Collect result */
#if !NUT_ENDIAN_LITTLE_BYTE
    for (int i = 0; i < 4; ++i)
        ((uint32_t*) _result)[i] = htole32(_state[i]);
#endif
}

const uint8_t* MD4::get_result() const noexcept
{
#if NUT_ENDIAN_LITTLE_BYTE
    return (const uint8_t*) _state;
#else
    return _result;
#endif
}

std::string MD4::get_hex_result() const noexcept
{
#if NUT_ENDIAN_LITTLE_BYTE
    return hex_encode(_state, DIGEST_SIZE, false);
#else
    return hex_encode(_result, DIGEST_SIZE, false);
#endif
}

void MD4::transform512bits(const void *block)
{
    assert(nullptr != block);

    uint32_t a = _state[0], b = _state[1], c = _state[2], d = _state[3];

#if NUT_ENDIAN_LITTLE_BYTE
    const uint32_t *x = (const uint32_t*) block;
#else
    uint32_t x[16];
    for (int i = 0; i < 16; ++i)
        x[i] = le32toh(((const uint32_t*) block)[i]);
#endif

    /* Round 1 */
    __FF__(a, b, c, d, x[ 0], __S11__); /* 1 */
    __FF__(d, a, b, c, x[ 1], __S12__); /* 2 */
    __FF__(c, d, a, b, x[ 2], __S13__); /* 3 */
    __FF__(b, c, d, a, x[ 3], __S14__); /* 4 */
    __FF__(a, b, c, d, x[ 4], __S11__); /* 5 */
    __FF__(d, a, b, c, x[ 5], __S12__); /* 6 */
    __FF__(c, d, a, b, x[ 6], __S13__); /* 7 */
    __FF__(b, c, d, a, x[ 7], __S14__); /* 8 */
    __FF__(a, b, c, d, x[ 8], __S11__); /* 9 */
    __FF__(d, a, b, c, x[ 9], __S12__); /* 10 */
    __FF__(c, d, a, b, x[10], __S13__); /* 11 */
    __FF__(b, c, d, a, x[11], __S14__); /* 12 */
    __FF__(a, b, c, d, x[12], __S11__); /* 13 */
    __FF__(d, a, b, c, x[13], __S12__); /* 14 */
    __FF__(c, d, a, b, x[14], __S13__); /* 15 */
    __FF__(b, c, d, a, x[15], __S14__); /* 16 */

    /* Round 2 */
    __GG__(a, b, c, d, x[ 0], __S21__); /* 17 */
    __GG__(d, a, b, c, x[ 4], __S22__); /* 18 */
    __GG__(c, d, a, b, x[ 8], __S23__); /* 19 */
    __GG__(b, c, d, a, x[12], __S24__); /* 20 */
    __GG__(a, b, c, d, x[ 1], __S21__); /* 21 */
    __GG__(d, a, b, c, x[ 5], __S22__); /* 22 */
    __GG__(c, d, a, b, x[ 9], __S23__); /* 23 */
    __GG__(b, c, d, a, x[13], __S24__); /* 24 */
    __GG__(a, b, c, d, x[ 2], __S21__); /* 25 */
    __GG__(d, a, b, c, x[ 6], __S22__); /* 26 */
    __GG__(c, d, a, b, x[10], __S23__); /* 27 */
    __GG__(b, c, d, a, x[14], __S24__); /* 28 */
    __GG__(a, b, c, d, x[ 3], __S21__); /* 29 */
    __GG__(d, a, b, c, x[ 7], __S22__); /* 30 */
    __GG__(c, d, a, b, x[11], __S23__); /* 31 */
    __GG__(b, c, d, a, x[15], __S24__); /* 32 */

    /* Round 3 */
    __HH__(a, b, c, d, x[ 0], __S31__); /* 33 */
    __HH__(d, a, b, c, x[ 8], __S32__); /* 34 */
    __HH__(c, d, a, b, x[ 4], __S33__); /* 35 */
    __HH__(b, c, d, a, x[12], __S34__); /* 36 */
    __HH__(a, b, c, d, x[ 2], __S31__); /* 37 */
    __HH__(d, a, b, c, x[10], __S32__); /* 38 */
    __HH__(c, d, a, b, x[ 6], __S33__); /* 39 */
    __HH__(b, c, d, a, x[14], __S34__); /* 40 */
    __HH__(a, b, c, d, x[ 1], __S31__); /* 41 */
    __HH__(d, a, b, c, x[ 9], __S32__); /* 42 */
    __HH__(c, d, a, b, x[ 5], __S33__); /* 43 */
    __HH__(b, c, d, a, x[13], __S34__); /* 44 */
    __HH__(a, b, c, d, x[ 3], __S31__); /* 45 */
    __HH__(d, a, b, c, x[11], __S32__); /* 46 */
    __HH__(c, d, a, b, x[ 7], __S33__); /* 47 */
    __HH__(b, c, d, a, x[15], __S34__); /* 48 */

    _state[0] += a;
    _state[1] += b;
    _state[2] += c;
    _state[3] += d;
}

}
