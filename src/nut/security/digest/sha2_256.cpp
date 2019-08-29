
#include <assert.h>
#include <memory.h> // for memset()

#include "../../util/string/string_utils.h"
#include "sha2_256.h"


#define __ROTLEFT__(value, bits) (((value) >> (bits)) | ((value) << (32 - (bits))))

#define __CH__(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define __MAJ__(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define __EP0__(x) (__ROTLEFT__((x), 2) ^ __ROTLEFT__((x), 13) ^ __ROTLEFT__((x), 22))
#define __EP1__(x) (__ROTLEFT__((x), 6) ^ __ROTLEFT__((x), 11) ^ __ROTLEFT__((x), 25))
#define __SIG0__(x) (__ROTLEFT__((x), 7) ^ __ROTLEFT__((x), 18) ^ ((x) >> 3))
#define __SIG1__(x) (__ROTLEFT__((x), 17) ^ __ROTLEFT__((x), 19) ^ ((x) >> 10))

namespace nut
{

static const uint32_t SHA256_K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

SHA2_256::SHA2_256() noexcept
{
    reset();
}

void SHA2_256::reset() noexcept
{
    _bit_len = 0;

    _state[0] = 0x6a09e667;
    _state[1] = 0xbb67ae85;
    _state[2] = 0x3c6ef372;
    _state[3] = 0xa54ff53a;
    _state[4] = 0x510e527f;
    _state[5] = 0x9b05688c;
    _state[6] = 0x1f83d9ab;
    _state[7] = 0x5be0cd19;

#if !NUT_ENDIAN_BIG_BYTE
    ::memset(_result, 0, DIGEST_SIZE);
#endif
}

void SHA2_256::update(uint8_t byte) noexcept
{
    update(&byte, 1);
}

void SHA2_256::update(const void *data, size_t cb) noexcept
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

void SHA2_256::digest() noexcept
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
    for (int i = 0; i < 8; ++i)
        ((uint32_t*) _result)[i] = htobe32(_state[i]);
#endif
}

const uint8_t* SHA2_256::get_result() const noexcept
{
#if NUT_ENDIAN_BIG_BYTE
    return (const uint8_t*) _state;
#else
    return _result;
#endif
}

std::string SHA2_256::get_hex_result() const noexcept
{
#if NUT_ENDIAN_BIG_BYTE
    return hex_encode(_state, DIGEST_SIZE, false);
#else
    return hex_encode(_result, DIGEST_SIZE, false);
#endif
}

void SHA2_256::transform512bits(const void *block) noexcept
{
    uint32_t a, b, c, d, e, f, g, h;
    a = _state[0];
    b = _state[1];
    c = _state[2];
    d = _state[3];
    e = _state[4];
    f = _state[5];
    g = _state[6];
    h = _state[7];

    uint32_t m[64];
    for (int i = 0; i < 16; ++i)
        m[i] = bswap_uint32(((const uint32_t*) block)[i]);
    for (int i = 16; i < 64; ++i)
        m[i] = __SIG1__(m[i - 2]) + m[i - 7] + __SIG0__(m[i - 15]) + m[i - 16];

    uint32_t t1, t2;
    for (int i = 0; i < 64; ++i)
    {
        t1 = h + __EP1__(e) + __CH__(e,f,g) + SHA256_K[i] + m[i];
        t2 = __EP0__(a) + __MAJ__(a,b,c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    _state[0] += a;
    _state[1] += b;
    _state[2] += c;
    _state[3] += d;
    _state[4] += e;
    _state[5] += f;
    _state[6] += g;
    _state[7] += h;
}

}
