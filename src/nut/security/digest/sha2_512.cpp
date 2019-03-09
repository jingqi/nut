
#include <assert.h>
#include <string.h> /* for ::memcpy() */
#include <limits.h> /* for SIZE_MAX */
#include <algorithm>

#include <nut/platform/endian.h>
#include <nut/util/string/string_utils.h>

#include "sha2_512.h"


// 循环右移(64位)
#define __ROR64__(value, bits) (((value) >> (bits)) | ((value) << (64 - (bits))))

// Ch, Maj 操作
#define __CH__(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define __MAJ__(x, y, z) ((((x) | (y)) & (z)) | ((x) & (y)))

// 循环右移
#define __S__(x, n) __ROR64__((x), (n))

// 同 2**128 除余右移
#define __R__(x, n) (((x) & 0xffffffffffffffffULL) >> ((uint64_t) n))

#define SIGMA0(x) (__S__((x), 28) ^ __S__((x), 34) ^ __S__((x), 39))
#define SIGMA1(x) (__S__((x), 14) ^ __S__((x), 18) ^ __S__((x), 41))
#define GAMMA0(x) (__S__((x), 1) ^ __S__((x), 8) ^ __R__((x), 7))
#define GAMMA1(x) (__S__((x), 19) ^ __S__((x), 61) ^ __R__((x), 6))

#define SHA512_ROUND(a, b, c, d, e, f, g, h, i)                         \
    do {                                                                \
        (t0) = (h) + SIGMA1(e) + __CH__((e), (f), (g)) + SHA512_K[i] + W[i]; \
        (t1) = SIGMA0(a) + __MAJ__((a), (b), (c));                      \
        (d) += (t0);                                                    \
        (h)  = (t0) + (t1);                                             \
    } while (false)

namespace nut
{

// 每次子循环中用到的常量
static const uint64_t SHA512_K[80] = {
    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
    0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
    0xd807aa98a3030242ULL, 0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
    0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
    0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
    0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
    0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
    0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL, 0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
    0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
    0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
    0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
    0xd192e819d6ef5218ULL, 0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
    0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
    0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
    0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
    0xca273eceea26619cULL, 0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
    0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
    0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
    0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};

SHA2_512::SHA2_512()
{
    reset();
}

void SHA2_512::reset()
{
#if NUT_HAS_INT128
    _bits_len = 0;
#else
    _bits_len_low = 0;
    _bits_len_high = 0;
#endif

    _state[0] = 0x6a09e667f3bcc908ULL;
    _state[1] = 0xbb67ae8584caa73bULL;
    _state[2] = 0x3c6ef372fe94f82bULL;
    _state[3] = 0xa54ff53a5f1d36f1ULL;
    _state[4] = 0x510e527fade682d1ULL;
    _state[5] = 0x9b05688c2b3e6c1fULL;
    _state[6] = 0x1f83d9abfb41bd6bULL;
    _state[7] = 0x5be0cd19137e2179ULL;

    ::memset(_result, 0, DIGEST_SIZE);
}

void SHA2_512::update(uint8_t byte)
{
    update(&byte, 1);
}

void SHA2_512::update(const void *buf, size_t cb)
{
    assert(nullptr != buf || 0 == cb);

    /* Calculate number of bytes mod 128 */
#if NUT_HAS_INT128
    unsigned index = (_bits_len >> 3) & 0x7f;
#else
    unsigned index = (_bits_len_low >> 3) & 0x7f;
#endif
    const unsigned partlen = 128 - index;  //partlen:同128相差的长度

    /* Update number of bits */
#if NUT_HAS_INT128
    _bits_len += cb << 3;
#else
    _bits_len_low += cb << 3;
    if (_bits_len_low < (cb << 3))
        ++_bits_len_high; // NOTE '_bits_len_low' 溢出
#   if SIZE_MAX >= 0x2000000000000000ULL // sizeof(size_t) * 8 > 61
    _bits_len_high += cb >> 61;
#   endif
#endif

    /* Transform as many times as possible */
    size_t i = 0;
    if (cb >= partlen)
    {
        if (0 == index)
        {
            transform1024bits(buf);
        }
        else
        {
            ::memcpy(_block + index, buf, partlen);
            transform1024bits(_block);
        }

        for (i = partlen; i + 128 <= cb; i += 128)
            transform1024bits(((const uint8_t*) buf) + i);

        index = 0;
    }

    /* Buffer remaining input */
    ::memcpy(_block + index, ((const uint8_t*) buf) + i, cb - i);
}

void SHA2_512::digest()
{
    /* Save bits length */
    uint8_t data_bits_len[16];
#if NUT_HAS_INT128
    ((uint64_t*) data_bits_len)[0] = htobe64((uint64_t) (_bits_len >> 64));
    ((uint64_t*) data_bits_len)[1] = htobe64((uint64_t) _bits_len);
#else
    ((uint64_t*) data_bits_len)[0] = htobe64(_bits_len_high);
    ((uint64_t*) data_bits_len)[1] = htobe64(_bits_len_low);
#endif

    /* Pad out to 112 mod 128. */
#if NUT_HAS_INT128
    const unsigned index = (_bits_len >> 3) & 0x7f;
#else
    const unsigned index = (_bits_len_low >> 3) & 0x7f;
#endif
    const unsigned pad_len = (index < 112) ? (112 - index) : (240 - index);
    const uint8_t PADDING[128] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    update(PADDING, pad_len);

    /* Append bits length */
    update(data_bits_len, 16);
#if NUT_HAS_INT128
    assert(0 == ((_bits_len >> 3) & 0x7f));
#else
    assert(0 == ((_bits_len_low >> 3) & 0x7f));
#endif

    /* Collect result */
    for (int i = 0; i < 8; ++i)
        ((uint64_t*) _result)[i] = htobe64(_state[i]);
}

const uint8_t* SHA2_512::get_result() const
{
    return _result;
}

std::string SHA2_512::get_hex_result() const
{
    return hex_encode(_result, DIGEST_SIZE, false);
}

void SHA2_512::transform1024bits(const void *block)
{
    // 把 state 的值复制给 S
    uint64_t S[8];
    for (int i = 0; i < 8; ++i)
        S[i] = _state[i];

    // 将字符数组保存的编码转为 uint64_t
    uint64_t W[80];
    for (int i = 0; i < 16; ++i)
        W[i] = bswap_uint64(((const uint64_t*) block)[i]);

    for (int i = 16; i < 80; ++i)
    {
        W[i] = GAMMA1(W[i - 2]) + W[i - 7] + GAMMA0(W[i - 15]) + W[i - 16];
    }

    uint64_t t0, t1;
    for (int i = 0; i < 80; i += 8)
    {
        SHA512_ROUND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], i + 0);
        SHA512_ROUND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], i + 1);
        SHA512_ROUND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], i + 2);
        SHA512_ROUND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], i + 3);
        SHA512_ROUND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], i + 4);
        SHA512_ROUND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], i + 5);
        SHA512_ROUND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], i + 6);
        SHA512_ROUND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], i + 7);
    }

    // Feedback
    for (int i = 0; i < 8; ++i)
        _state[i] += S[i];
}

}
