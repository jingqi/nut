
#include <assert.h>
#include <string.h> /* for ::memcpy() */

#include "../../util/string/string_utils.h"
#include "sha1.h"


#define __ROTATE_LEFT__(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

namespace nut
{

SHA1::SHA1() noexcept
{
    reset();
}

void SHA1::reset() noexcept
{
    _bit_len = 0;

    /* Load magic initialization constants */
    _state[0] = 0x67452301;
    _state[1] = 0xefcdab89;
    _state[2] = 0x98badcfe;
    _state[3] = 0x10325476;
    _state[4] = 0xc3d2e1f0;

#if !NUT_ENDIAN_BIG_BYTE
    ::memset(_result, 0, DIGEST_SIZE);
#endif
}

void SHA1::update(uint8_t byte) noexcept
{
    update(&byte, 1);
}

void SHA1::update(const void *data, size_t cb) noexcept
{
    assert(nullptr != data || 0 == cb);

    /* Calculate number of bytes mod 64 */
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

void SHA1::digest() noexcept
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
#if NUT_ENDIAN_BIG_BYTE
    *(uint64_t*)(_buffer + 56) = _bit_len;
#else
    *(uint64_t*)(_buffer + 56) = htobe64(_bit_len);
#endif

    transform512bits(_buffer);

    /* Collect result */
#if !NUT_ENDIAN_BIG_BYTE
    for (int i = 0; i < 5; ++i)
        ((uint32_t*) _result)[i] = htobe32(_state[i]);
#endif
}

const uint8_t* SHA1::get_result() const noexcept
{
#if NUT_ENDIAN_BIG_BYTE
    return (const uint8_t*) _state;
#else
    return _result;
#endif
}

std::string SHA1::get_hex_result() const noexcept
{
#if NUT_ENDIAN_BIG_BYTE
    return hex_encode(_state, DIGEST_SIZE, false);
#else
    return hex_encode(_result, DIGEST_SIZE, false);
#endif
}

void SHA1::transform512bits(const void *block) noexcept
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
