
#include <assert.h>
#include <string.h> /* for ::memcpy() */

#include <nut/platform/endian.h>
#include <nut/util/string/string_utils.h>

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
    _bits_len = 0;

    /* Load magic initialization constants */
    _state[0] = 0x67452301;
    _state[1] = 0xefcdab89;
    _state[2] = 0x98badcfe;
    _state[3] = 0x10325476;
    _state[4] = 0xc3d2e1f0;

    ::memset(_result, 0, DIGEST_SIZE);
}

void SHA1::update(uint8_t byte)
{
    update(&byte, 1);
}

void SHA1::update(const void *buf, size_t cb)
{
    assert(nullptr != buf || 0 == cb);

    /* Calculate number of bytes mod 64 */
    unsigned index = (_bits_len >> 3) & 0x3F;
    const unsigned partlen = 64 - index;

    /* Update number of bits */
    _bits_len += cb << 3;

    /* Transform as many times as possible */
    size_t i = 0;
    if (cb >= partlen)
    {
        if (0 == index)
        {
            transform512bits(buf);
        }
        else
        {
            ::memcpy(_block + index, buf, partlen);
            transform512bits(_block);
        }

        for (i = partlen; i + 64 <= cb; i += 64)
            transform512bits(((const uint8_t*) buf) + i);

        index = 0;
    }

    /* Buffer remaining input */
    ::memcpy(_block + index, ((const uint8_t*) buf) + i, cb - i);
}

void SHA1::digest()
{
    /* Save bits length */
    const uint64_t data_bits_len = htobe64(_bits_len);

    /* Pad out to 56 mod 64. */
    const unsigned index = (_bits_len >> 3) & 0x3f;
    const unsigned pad_len = (index < 56) ? (56 - index) : (120 - index);
    const uint8_t PADDING[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    update(PADDING, pad_len);

    /* Append bits length */
    update(&data_bits_len, 8);
    assert(0 == ((_bits_len >> 3) & 0x3f));

    /* Collect result */
    for (int i = 0; i < 5; ++i)
        ((uint32_t*) _result)[i] = htobe32(_state[i]);
}

const uint8_t* SHA1::get_result() const
{
    return _result;
}

std::string SHA1::get_hex_result() const
{
    return hex_encode(_result, DIGEST_SIZE, false);
}

void SHA1::transform512bits(const void *block)
{
    assert(nullptr != block);

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
