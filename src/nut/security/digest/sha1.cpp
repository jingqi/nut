
#include <assert.h>
#include <string.h> /* for ::memcpy() */

#include "sha1.h"


#define __ROTATE_LEFT__(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

namespace nut
{

SHA1::SHA1()
{
    reset();
}

void SHA1::reset()
{
    _bytes_len = 0;

    /* Load magic initialization constants */
    _state[0] = 0x67452301;
    _state[1] = 0xefcdab89;
    _state[2] = 0x98badcfe;
    _state[3] = 0x10325476;
    _state[4] = 0xc3d2e1f0;
}

void SHA1::update(uint8_t byte)
{
    update(&byte, 1);
}

void SHA1::update(const void *buf, size_t cb)
{
    assert(NULL != buf || 0 == cb);

    /* Calculate number of bytes mod 64 */
    uint32_t index = _bytes_len & 0x3F;
    const uint32_t partlen = 64 - index;

    /* Update number of bits */
    _bytes_len += cb;

    /* Transform as many times as possible */
    size_t i = 0;
    if (cb >= partlen)
    {
        ::memcpy(_buffer + index, buf, partlen);
        transform512bits(_buffer);

        for (i = partlen; i + 63 < cb; i += 64)
            transform512bits(((const uint8_t*) buf) + i);

        index = 0;
    }

    /* Buffer remaining input */
    ::memcpy(_buffer + index, ((const uint8_t*) buf) + i, cb - i);
}

void SHA1::digest()
{
    /* Save number of bits */
    const uint64_t bits = _bytes_len << 3;

    /* Pad out to 56 mod 64. */
    const size_t index = (size_t) (_bytes_len & 0x3f);
    const size_t pad_len = (index < 56) ? (56 - index) : (120 - index);
    const uint8_t PADDING[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    update(PADDING, pad_len);

    /* Append length (before padding) */
    for (int i = 0; i < 8; ++i)
        update(((const uint8_t*) &bits) + 7 - i, 1);
    assert(0 == (_bytes_len & 0x3f));
}

const uint8_t* SHA1::get_bytes_result(void *result) const
{
    if (NULL != result)
        ::memcpy(result, _state, 20);
    return (const uint8_t*) _state;
}

std::string SHA1::get_string_result() const
{
    std::string ret;
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            const uint8_t b = ((uint8_t*)(_state + i))[3 - j];
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

void SHA1::transform512bits(const void *block)
{
    assert(NULL != block);

    uint32_t w[80];
    for (int i = 0; i < 16; ++i)
    {
        for (int j = 0; j < 4; ++j)
            ((uint8_t*) w)[i * 4 + j] = ((const uint8_t*) block)[i * 4 + 3 - j];
    }
    for (int i = 16; i < 80; ++i)
    {
        const uint32_t tmp = w[i - 16] ^ w[i - 14] ^ w[i - 8] ^ w[i - 3];
        w[i] = __ROTATE_LEFT__(tmp, 1);
    }

    uint32_t a = _state[0], b = _state[1], c = _state[2], d = _state[3], e = _state[4];

    for (int i = 0; i < 20; ++i)
    {
        const uint32_t tmp =  __ROTATE_LEFT__(a, 5) +
            ((b & c) | ((~b) & d)) + e + w[i] + 0x5a827999;
        e = d;
        d = c;
        c = __ROTATE_LEFT__(b, 30);
        b = a;
        a = tmp;
    }

    for (int i = 20; i < 40; ++i)
    {
        const uint32_t tmp = __ROTATE_LEFT__(a, 5) +
            (b ^ c ^ d) + e + w[i] + 0x6ed9eba1;
        e = d;
        d = c;
        c = __ROTATE_LEFT__(b, 30);
        b = a;
        a = tmp;
    }

    for (int i = 40; i < 60; ++i)
    {
        const uint32_t tmp = __ROTATE_LEFT__(a, 5) +
            ((b & c) | (b & d) | (c & d)) + e + w[i] + 0x8f1bbcdc;
        e = d;
        d = c;
        c = __ROTATE_LEFT__(b, 30);
        b = a;
        a = tmp;
    }

    for (int i = 60; i < 80; ++i)
    {
        const uint32_t tmp = __ROTATE_LEFT__(a, 5) +
            (b ^ c ^ d) + e + w[i] + 0xca62c1d6;
        e = d;
        d = c;
        c = __ROTATE_LEFT__(b, 30);
        b = a;
        a = tmp;
    }

    _state[0] += a;
    _state[1] += b;
    _state[2] += c;
    _state[3] += d;
    _state[4] += e;
}

}
