
#include <assert.h>

#include "../../../platform/endian.h"
#include "../../../numeric/numeric_algo/prime.h"
#include "der_writer.h"


namespace nut
{

NUT_API void der_write_len(std::vector<uint8_t> *output, size_t len) noexcept
{
    assert(nullptr != output);

    if (len < 0x80)
    {
        output->push_back((uint8_t) len);
        return;
    }

    uint8_t buffer[sizeof(size_t)];
    *(size_t*)buffer = len;
#if NUT_ENDIAN_LITTLE_BYTE
    bswap(buffer, sizeof(size_t));
#endif

    int start = 0;
    while (0 == buffer[start])
        ++start;
    assert(start < (int) sizeof(size_t));
    output->push_back((uint8_t) (sizeof(size_t) - start) | 0x80);
    for (int i = start; i < (int) sizeof(size_t); ++i)
        output->push_back(buffer[i]);
}

NUT_API void der_write_integer(std::vector<uint8_t> *output, const BigInteger& v) noexcept
{
    assert(nullptr != output);

    output->push_back(0x02);
    std::vector<uint8_t> bytes = v.to_be_bytes();
    der_write_len(output, bytes.size());
    output->insert(output->end(), bytes.begin(), bytes.end());
}

NUT_API void der_write_bit_string(std::vector<uint8_t> *output, const std::vector<uint8_t>& bs) noexcept
{
    assert(nullptr != output);

    output->push_back(0x03);
    der_write_len(output, bs.size() + 1);
    output->push_back(0); // Unused leading zero
    output->insert(output->end(), bs.begin(), bs.end());
}

NUT_API void der_write_octet_string(std::vector<uint8_t> *output, const std::vector<uint8_t>& os) noexcept
{
    assert(nullptr != output);

    output->push_back(0x04);
    der_write_len(output, os.size());
    output->insert(output->end(), os.begin(), os.end());
}

NUT_API void der_write_sequence(std::vector<uint8_t> *output, const std::vector<uint8_t>& sq) noexcept
{
    assert(nullptr != output);

    output->push_back(0x30);
    der_write_len(output, sq.size());
    output->insert(output->end(), sq.begin(), sq.end());
}

NUT_API void der_write_obj_ident(std::vector<uint8_t> *output, const uint8_t *id_begin, size_t id_size) noexcept
{
    assert(nullptr != output && nullptr != id_begin);

    output->push_back(0x06);
    der_write_len(output, id_size);
    output->insert(output->end(), id_begin, id_begin + id_size);
}

NUT_API void der_write_null(std::vector<uint8_t> *output) noexcept
{
    assert(nullptr != output);
    output->push_back(0x05);
    output->push_back(0);
}

/**
 * 结构:
 *
 * SEQUENCE
 *   INTEGER -- n
 *   INTEGER -- e
 */
NUT_API void der_write_pkcs1_public(std::vector<uint8_t> *output, const RSA::PublicKey& pubkey) noexcept
{
    assert(nullptr != output);
    std::vector<uint8_t> sq;
    der_write_integer(&sq, pubkey.n);
    der_write_integer(&sq, pubkey.e);
    der_write_sequence(output, sq);
}

/**
 * 结构:
 *
 * SEQUENCE -- sq1
 *   SEQUENCE -- sq2
 *     OBJECT IDENTIFIER
 *     NULL
 *   BIT STRING
 *     SEQUENCE -- sq3
 *       INTEGER -- n
 *       INTEGER -- e
 */
NUT_API void der_write_pkcs8_public(std::vector<uint8_t> *output, const RSA::PublicKey& pubkey) noexcept
{
    assert(nullptr != output);

    std::vector<uint8_t> sq2;
    const uint8_t rsa_ident[] = {0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01}; // Hex encoding of 1.2.840.113549.1.1
    der_write_obj_ident(&sq2, rsa_ident, sizeof(rsa_ident));
    der_write_null(&sq2);

    std::vector<uint8_t> bs;
    der_write_pkcs1_public(&bs, pubkey);

    std::vector<uint8_t> sq1;
    der_write_sequence(&sq1, sq2);
    der_write_bit_string(&sq1, bs);

    der_write_sequence(output, sq1);
}

/**
 * 结构:
 *
 * SEQUENCE
 *   INTEGER -- version
 *   INTEGER -- n
 *   INTEGER -- e
 *   INTEGER -- d
 *   INTEGER -- p
 *   INTEGER -- q
 *   INTEGER -- d mod (p - 1)
 *   INTEGER -- d mod (q - 1)
 *   INTEGER -- (inverse of q) mod p
 *   OtherPrimeInfos OPTIONAL
 */
NUT_API void der_write_pkcs1_private(std::vector<uint8_t> *output, const RSA::PrivateKey& prikey) noexcept
{
    assert(nullptr != output);
    std::vector<uint8_t> sq;
    der_write_integer(&sq, BigInteger(0));
    der_write_integer(&sq, prikey.n);
    der_write_integer(&sq, prikey.e);
    der_write_integer(&sq, prikey.d);
    der_write_integer(&sq, prikey.p);
    der_write_integer(&sq, prikey.q);
    der_write_integer(&sq, prikey.d % (prikey.p - 1));
    der_write_integer(&sq, prikey.d % (prikey.q - 1));

    const BigInteger iqmp = inverse_of_coprime_mod(prikey.q, prikey.p);
    der_write_integer(&sq, iqmp);

    der_write_sequence(output, sq);
}

/**
 * 结构:
 *
 * SEQUENCE -- sq1
 *   INTEGER -- version
 *   SEQUENCE -- sq2
 *     OBJECT IDENTIFIER
 *     NULL
 *   OCTET STRING
 *     SEQUENCE -- sq3
 *       INTEGER -- version
 *       INTEGER -- n
 *       INTEGER -- e
 *       INTEGER -- d
 *       INTEGER -- p
 *       INTEGER -- q
 *       INTEGER -- d mod (p - 1)
 *       INTEGER -- d mod (q - 1)
 *       INTEGER -- (inverse of q) mod p
 *       OtherPrimeInfos OPTIONAL
 */
NUT_API void der_write_pkcs8_private(std::vector<uint8_t> *output, const RSA::PrivateKey& prikey) noexcept
{
    assert(nullptr != output);

    std::vector<uint8_t> sq2;
    const uint8_t rsa_ident[] = {0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01}; // Hex encoding of 1.2.840.113549.1.1
    der_write_obj_ident(&sq2, rsa_ident, sizeof(rsa_ident));
    der_write_null(&sq2);

    std::vector<uint8_t> sq3;
    der_write_integer(&sq3, BigInteger(0));
    der_write_integer(&sq3, prikey.n);
    der_write_integer(&sq3, prikey.e);
    der_write_integer(&sq3, prikey.d);
    der_write_integer(&sq3, prikey.p);
    der_write_integer(&sq3, prikey.q);
    der_write_integer(&sq3, prikey.d % (prikey.p - 1));
    der_write_integer(&sq3, prikey.d % (prikey.q - 1));

    const BigInteger iqmp = inverse_of_coprime_mod(prikey.q, prikey.p);
    der_write_integer(&sq3, iqmp);

    std::vector<uint8_t> bs;
    der_write_sequence(&bs, sq3);

    std::vector<uint8_t> sq1;
    der_write_integer(&sq1, BigInteger(0));
    der_write_sequence(&sq1, sq2);
    der_write_octet_string(&sq1, bs);

    der_write_sequence(output, sq1);
}

}
