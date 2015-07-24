
#include <assert.h>
#include <string.h> // for memcpy()

#include "md5.h"

#define __S11__ 7
#define __S12__ 12
#define __S13__ 17
#define __S14__ 22
#define __S21__ 5
#define __S22__ 9
#define __S23__ 14
#define __S24__ 20
#define __S31__ 4
#define __S32__ 11
#define __S33__ 16
#define __S34__ 23
#define __S41__ 6
#define __S42__ 10
#define __S43__ 15
#define __S44__ 21

#define __F__(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define __G__(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define __H__(x, y, z) ((x) ^ (y) ^ (z))
#define __I__(x, y, z) ((y) ^ ((x) | (~z)))

#define __ROTATE_LEFT__(x, n) (((x) << (n)) | ((x) >> (32-(n))))

#define __FF__(a, b, c, d, x, s, ac)                        \
    {                                                       \
        (a) += __F__((b), (c), (d)) + (x) + (uint32_t)(ac); \
        (a) = __ROTATE_LEFT__((a), (s));                    \
        (a) += (b);                                         \
    }

#define __GG__(a, b, c, d, x, s, ac)                        \
    {                                                       \
        (a) += __G__((b), (c), (d)) + (x) + (uint32_t)(ac); \
        (a) = __ROTATE_LEFT__((a), (s));                    \
        (a) += (b);                                         \
    }

#define __HH__(a, b, c, d, x, s, ac)                        \
    {                                                       \
        (a) += __H__((b), (c), (d)) + (x) + (uint32_t)(ac); \
        (a) = __ROTATE_LEFT__((a), (s));                    \
        (a) += (b);                                         \
    }

#define __II__(a, b, c, d, x, s, ac)                        \
    {                                                       \
        (a) += __I__((b), (c), (d)) + (x) + (uint32_t)(ac); \
        (a) = __ROTATE_LEFT__((a), (s));                    \
        (a) += (b);                                         \
    }

namespace nut
{

MD5::MD5()
{
    reset();
}

void MD5::reset()
{
    _byteslen = 0;

    // load magic initialization constants
    _state[0] = 0x67452301;
    _state[1] = 0xefcdab89;
    _state[2] = 0x98badcfe;
    _state[3] = 0x10325476;
}

void MD5::update(uint8_t byte)
{
    update(&byte, 1);
}

void MD5::update(const void *buf, size_t cb)
{
    assert(NULL != buf || 0 == cb);

    // compute number of bytes mod 64
    uint32_t index = _byteslen & 0x3F;
    uint32_t partlen = 64 - index;

    // update number of bits
    _byteslen += cb;

    // transform as many times as possible
    size_t i = 0;
    if (cb >= partlen)
    {
        ::memcpy(_buffer + index, buf, partlen);
        transform512bits(_buffer);

        for (i = partlen; i + 63 < cb; i += 64)
            transform512bits(((const uint8_t*)buf) + i);

        index = 0;
    }
    else
    {
        i = 0;
    }

    /* Buffer remaining input */
    ::memcpy(_buffer + index, ((const uint8_t*)buf) + i, cb - i);
}

void MD5::digest()
{
    /* Save number of bits */
    const uint64_t bits = _byteslen << 3;

    /* Pad out to 56 mod 64. */
    const size_t index = (size_t)(_byteslen & 0x3f);
    const size_t pad_len = (index < 56) ? (56 - index) : (120 - index);
    const uint8_t PADDING[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    update(PADDING, pad_len);

    /* Append length (before padding) */
    update(&bits, 8);
}

std::string MD5::get_string_result() const
{
    std::string ret;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            const uint8_t b = ((uint8_t*)(_state + i))[j];
            int n = (0xF0 & b) >> 4;
            if (n < 10)
                ret += (char) ('0' + n);
            else
                ret += (char) ('A' + n - 10);

            n = 0x0F & b;
            if (n < 10)
                ret += (char) ('0' + n);
            else
                ret += (char) ('A' + n - 10);
        }
    }
    return ret;
}

void MD5::get_bytes_result(uint8_t *ret)
{
    assert(NULL != ret);
    ::memcpy(ret, _state, 16);
}

void MD5::transform512bits(const void *block)
{
    assert(NULL != block);

    uint32_t a = _state[0], b = _state[1], c = _state[2], d = _state[3];
    const uint32_t *x = (const uint32_t*) block;

    /* Round 1 */
    __FF__(a, b, c, d, x[ 0], __S11__, 0xd76aa478); /* 1 */
    __FF__(d, a, b, c, x[ 1], __S12__, 0xe8c7b756); /* 2 */
    __FF__(c, d, a, b, x[ 2], __S13__, 0x242070db); /* 3 */
    __FF__(b, c, d, a, x[ 3], __S14__, 0xc1bdceee); /* 4 */
    __FF__(a, b, c, d, x[ 4], __S11__, 0xf57c0faf); /* 5 */
    __FF__(d, a, b, c, x[ 5], __S12__, 0x4787c62a); /* 6 */
    __FF__(c, d, a, b, x[ 6], __S13__, 0xa8304613); /* 7 */
    __FF__(b, c, d, a, x[ 7], __S14__, 0xfd469501); /* 8 */
    __FF__(a, b, c, d, x[ 8], __S11__, 0x698098d8); /* 9 */
    __FF__(d, a, b, c, x[ 9], __S12__, 0x8b44f7af); /* 10 */
    __FF__(c, d, a, b, x[10], __S13__, 0xffff5bb1); /* 11 */
    __FF__(b, c, d, a, x[11], __S14__, 0x895cd7be); /* 12 */
    __FF__(a, b, c, d, x[12], __S11__, 0x6b901122); /* 13 */
    __FF__(d, a, b, c, x[13], __S12__, 0xfd987193); /* 14 */
    __FF__(c, d, a, b, x[14], __S13__, 0xa679438e); /* 15 */
    __FF__(b, c, d, a, x[15], __S14__, 0x49b40821); /* 16 */

    /* Round 2 */
    __GG__(a, b, c, d, x[ 1], __S21__, 0xf61e2562); /* 17 */
    __GG__(d, a, b, c, x[ 6], __S22__, 0xc040b340); /* 18 */
    __GG__(c, d, a, b, x[11], __S23__, 0x265e5a51); /* 19 */
    __GG__(b, c, d, a, x[ 0], __S24__, 0xe9b6c7aa); /* 20 */
    __GG__(a, b, c, d, x[ 5], __S21__, 0xd62f105d); /* 21 */
    __GG__(d, a, b, c, x[10], __S22__, 0x02441453); /* 22 */
    __GG__(c, d, a, b, x[15], __S23__, 0xd8a1e681); /* 23 */
    __GG__(b, c, d, a, x[ 4], __S24__, 0xe7d3fbc8); /* 24 */
    __GG__(a, b, c, d, x[ 9], __S21__, 0x21e1cde6); /* 25 */
    __GG__(d, a, b, c, x[14], __S22__, 0xc33707d6); /* 26 */
    __GG__(c, d, a, b, x[ 3], __S23__, 0xf4d50d87); /* 27 */
    __GG__(b, c, d, a, x[ 8], __S24__, 0x455a14ed); /* 28 */
    __GG__(a, b, c, d, x[13], __S21__, 0xa9e3e905); /* 29 */
    __GG__(d, a, b, c, x[ 2], __S22__, 0xfcefa3f8); /* 30 */
    __GG__(c, d, a, b, x[ 7], __S23__, 0x676f02d9); /* 31 */
    __GG__(b, c, d, a, x[12], __S24__, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    __HH__(a, b, c, d, x[ 5], __S31__, 0xfffa3942); /* 33 */
    __HH__(d, a, b, c, x[ 8], __S32__, 0x8771f681); /* 34 */
    __HH__(c, d, a, b, x[11], __S33__, 0x6d9d6122); /* 35 */
    __HH__(b, c, d, a, x[14], __S34__, 0xfde5380c); /* 36 */
    __HH__(a, b, c, d, x[ 1], __S31__, 0xa4beea44); /* 37 */
    __HH__(d, a, b, c, x[ 4], __S32__, 0x4bdecfa9); /* 38 */
    __HH__(c, d, a, b, x[ 7], __S33__, 0xf6bb4b60); /* 39 */
    __HH__(b, c, d, a, x[10], __S34__, 0xbebfbc70); /* 40 */
    __HH__(a, b, c, d, x[13], __S31__, 0x289b7ec6); /* 41 */
    __HH__(d, a, b, c, x[ 0], __S32__, 0xeaa127fa); /* 42 */
    __HH__(c, d, a, b, x[ 3], __S33__, 0xd4ef3085); /* 43 */
    __HH__(b, c, d, a, x[ 6], __S34__, 0x04881d05); /* 44 */
    __HH__(a, b, c, d, x[ 9], __S31__, 0xd9d4d039); /* 45 */
    __HH__(d, a, b, c, x[12], __S32__, 0xe6db99e5); /* 46 */
    __HH__(c, d, a, b, x[15], __S33__, 0x1fa27cf8); /* 47 */
    __HH__(b, c, d, a, x[ 2], __S34__, 0xc4ac5665); /* 48 */

    /* Round 4 */
    __II__(a, b, c, d, x[ 0], __S41__, 0xf4292244); /* 49 */
    __II__(d, a, b, c, x[ 7], __S42__, 0x432aff97); /* 50 */
    __II__(c, d, a, b, x[14], __S43__, 0xab9423a7); /* 51 */
    __II__(b, c, d, a, x[ 5], __S44__, 0xfc93a039); /* 52 */
    __II__(a, b, c, d, x[12], __S41__, 0x655b59c3); /* 53 */
    __II__(d, a, b, c, x[ 3], __S42__, 0x8f0ccc92); /* 54 */
    __II__(c, d, a, b, x[10], __S43__, 0xffeff47d); /* 55 */
    __II__(b, c, d, a, x[ 1], __S44__, 0x85845dd1); /* 56 */
    __II__(a, b, c, d, x[ 8], __S41__, 0x6fa87e4f); /* 57 */
    __II__(d, a, b, c, x[15], __S42__, 0xfe2ce6e0); /* 58 */
    __II__(c, d, a, b, x[ 6], __S43__, 0xa3014314); /* 59 */
    __II__(b, c, d, a, x[13], __S44__, 0x4e0811a1); /* 60 */
    __II__(a, b, c, d, x[ 4], __S41__, 0xf7537e82); /* 61 */
    __II__(d, a, b, c, x[11], __S42__, 0xbd3af235); /* 62 */
    __II__(c, d, a, b, x[ 2], __S43__, 0x2ad7d2bb); /* 63 */
    __II__(b, c, d, a, x[ 9], __S44__, 0xeb86d391); /* 64 */

    _state[0] += a;
    _state[1] += b;
    _state[2] += c;
    _state[3] += d;
}

}
