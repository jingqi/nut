/**
 * DER 格式参看 https://www.lightky.com/backend/2016/09/rsa-pem-format.html
 */

#include <assert.h>
#include <string.h> // for ::memcpy()

#include "../../../platform/endian.h"
#include "der_reader.h"


namespace nut
{

NUT_API ssize_t der_read_len(const uint8_t *data, size_t available, size_t *rs)
{
    assert(nullptr != data && available > 0 && nullptr != rs);

    const size_t len = data[0] & 0x7f;
    if (0 != (data[0] & 0x80))
    {
        if (len > sizeof(size_t) || 1 + len > available)
            return -1;
        *rs = *(const size_t*)(data + 1);
#if NUT_ENDIAN_LITTLE_BYTE
        bswap(rs, sizeof(size_t));
#endif
        *rs >>= (sizeof(size_t) - len) * 8;
        return 1 + len;
    }

    *rs = len;
    return 1;
}

NUT_API ssize_t der_read_integer(const uint8_t *data, size_t available, BigInteger *rs)
{
    assert(nullptr != data && available > 0);

    if (available < 2)
        return -1;
    if (0x02 != data[0])
        return -1; // Not integer type

    size_t len = 0;
    const ssize_t readed = der_read_len(data + 1, available - 1, &len);
    if (readed < 0 || 1 + readed + len > available)
        return -1;

    if (nullptr != rs)
    {
#if NUT_ENDIAN_LITTLE_BYTE
        uint8_t *buf = (uint8_t*) ::malloc(len);
        ::memcpy(buf, data + 1 + readed, len);
        bswap(buf, len);
        rs->set(buf, len, false);
        ::free(buf);
#else
        rs->set(data + 1 + readed, len, false);
#endif
    }

    return 1 + readed + len;
}

NUT_API ssize_t der_read_bit_string(const uint8_t *data, size_t available, const uint8_t **bs_begin, size_t *bs_size)
{
    assert(nullptr != data && available > 0 && nullptr != bs_begin && nullptr != bs_size);

    if (0x03 != data[0])
        return -1;

    size_t index = 1;
    if (index >= available)
        return -1;
    size_t len = 0;
    ssize_t readed = der_read_len(data + index, available - index, &len);
    if (readed < 0 || len < 1 || index + readed + len > available)
        return -1;
    index += readed;

    if (0 != data[index]) // Unused leading zero
        return -1;
    *bs_begin = data + index + 1;
    *bs_size = len - 1;
    return index + len;
}

NUT_API ssize_t der_read_octet_string(const uint8_t *data, size_t available, const uint8_t **os_begin, size_t *os_size)
{
    assert(nullptr != data && available > 0 && nullptr != os_begin && nullptr != os_size);

    if (0x04 != data[0])
        return -1;

    size_t index = 1;
    if (index >= available)
        return -1;
    size_t len = 0;
    ssize_t readed = der_read_len(data + index, available - index, &len);
    if (readed < 0 || index + readed + len > available)
        return -1;
    index += readed;

    *os_begin = data + index;
    *os_size = len;
    return index + len;
}

NUT_API ssize_t der_read_sequence(const uint8_t *data, size_t available, const uint8_t **sq_begin, size_t *sq_size)
{
    assert(nullptr != data && available > 0 && nullptr != sq_begin && nullptr != sq_size);

    if (0x30 != data[0])
        return -1;

    size_t index = 1;
    if (index >= available)
        return -1;
    size_t len = 0;
    ssize_t readed = der_read_len(data + index, available - index, &len);
    if (readed < 0 || index + readed + len > available)
        return -1;
    index += readed;

    *sq_begin = data + index;
    *sq_size = len;
    return index + len;
}

NUT_API ssize_t der_read_obj_ident(const uint8_t *data, size_t available, const uint8_t **id_begin, size_t *id_size)
{
    assert(nullptr != data && available > 0 && nullptr != id_begin && nullptr != id_size);

    if (0x06 != data[0])
        return -1;
    size_t index = 1;
    if (index >= available)
        return -1;
    size_t len = 0;
    ssize_t readed = der_read_len(data + index, available - index, &len);
    if (readed < 0 || index + readed + len > available)
        return -1;
    index += readed;

    *id_begin = data + index;
    *id_size = len;
    return index + len;
}

NUT_API ssize_t der_read_null(const uint8_t *data, size_t available)
{
    assert(nullptr != data && available > 0);
    if (available < 2)
        return -1;
    if (0x05 != data[0] && 0 == data[1])
        return -1;
    return 2;
}

/**
 * 结构:
 *
 * SEQUENCE
 *   INTEGER -- n
 *   INTEGER -- e
 */
NUT_API ssize_t der_read_pkcs1_public(const uint8_t *data, size_t available, RSA::PublicKey *pubkey)
{
    assert(nullptr != data && available > 0 && nullptr != pubkey);

    // Sequence
    const uint8_t *sq_begin = nullptr;
    size_t sq_size = 0, sq_index = 0;
    ssize_t readed = der_read_sequence(data, available, &sq_begin, &sq_size);
    if (readed < 0)
        return -1;
    const size_t sq_total_size = readed;

    // n
    if (sq_index >= sq_size)
        return -1;
    readed = der_read_integer(sq_begin + sq_index, sq_size - sq_index, &(pubkey->n));
    if (readed < 0)
        return -1;
    sq_index += readed;

    // e
    if (sq_index >= sq_size)
        return -1;
    readed = der_read_integer(sq_begin + sq_index, sq_size - sq_index, &(pubkey->e));
    if (readed < 0 || sq_index + readed != sq_size)
        return -1;
    sq_index += readed;

    return sq_total_size;
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
NUT_API ssize_t der_read_pkcs8_public(const uint8_t *data, size_t available, RSA::PublicKey *pubkey)
{
    assert(nullptr != data && available > 0 && nullptr != pubkey);

    // sq1
    const uint8_t *sq1_begin = nullptr;
    size_t sq1_size = 0, sq1_index = 0;
    ssize_t readed = der_read_sequence(data, available, &sq1_begin, &sq1_size);
    if (readed < 0)
        return -1;
    const ssize_t sq1_total_size = readed;

    // sq2
    if (sq1_index >= sq1_size)
        return -1;
    const uint8_t *sq2_begin = nullptr;
    size_t sq2_size = 0, sq2_index = 0;
    readed = der_read_sequence(sq1_begin + sq1_index, sq1_size - sq1_index, &sq2_begin, &sq2_size);
    if (readed < 0)
        return -1;
    sq1_index += readed;

    // Object identifier
    if (sq2_index >= sq2_size)
        return -1;
    const uint8_t *id_begin = nullptr;
    size_t id_size = 0;
    readed = der_read_obj_ident(sq2_begin + sq2_index, sq2_size - sq2_index, &id_begin, &id_size);
    if (readed < 0)
        return -1;
    sq2_index += readed;
    const uint8_t rsa_ident[] = {0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01}; // Hex encoding of 1.2.840.113549.1.1
    if (id_size != sizeof(rsa_ident) || 0 != ::memcmp(id_begin, rsa_ident, sizeof(rsa_ident)))
        return -1; // Not RSA public key

    // Bit string
    if (sq1_index >= sq1_size)
        return -1;
    const uint8_t *bs_begin = nullptr;
    size_t bs_size = 0;
    readed = der_read_bit_string(sq1_begin + sq1_index, sq1_size - sq1_index, &bs_begin, &bs_size);
    if (readed < 0 || sq1_index + readed != sq1_size)
        return -1;
    sq1_index += readed;

    // sq3 => pkcs1
    readed = der_read_pkcs1_public(bs_begin, bs_size, pubkey);
    if (readed < 0 || readed != (ssize_t) bs_size)
        return -1;

    return sq1_total_size;
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
NUT_API ssize_t der_read_pkcs1_private(const uint8_t *data, size_t available, RSA::PrivateKey *prikey)
{
    assert(nullptr != data && available > 0 && nullptr != prikey);

    // Sequence
    const uint8_t *sq_begin = nullptr;
    size_t sq_size = 0, sq_index = 0;
    ssize_t readed = der_read_sequence(data, available, &sq_begin, &sq_size);
    if (readed < 0)
        return -1;
    const size_t sq_total_size = readed;

    // version
    if (sq_index >= sq_size)
        return -1;
    BigInteger version;
    readed = der_read_integer(sq_begin + sq_index, sq_size - sq_index, &version);
    if (readed < 0 || version != 0)
        return -1;
    sq_index += readed;

    // n
    if (sq_index >= sq_size)
        return -1;
    readed = der_read_integer(sq_begin + sq_index, sq_size - sq_index, &(prikey->n));
    if (readed < 0)
        return -1;
    sq_index += readed;

    // e
    if (sq_index >= sq_size)
        return -1;
    readed = der_read_integer(sq_begin + sq_index, sq_size - sq_index, &(prikey->e));
    if (readed < 0)
        return -1;
    sq_index += readed;

    // d
    if (sq_index >= sq_size)
        return -1;
    readed = der_read_integer(sq_begin + sq_index, sq_size - sq_index, &(prikey->d));
    if (readed < 0)
        return -1;
    sq_index += readed;

    // p
    if (sq_index >= sq_size)
        return -1;
    readed = der_read_integer(sq_begin + sq_index, sq_size - sq_index, &(prikey->p));
    if (readed < 0)
        return -1;
    sq_index += readed;

    // q
    if (sq_index >= sq_size)
        return -1;
    readed = der_read_integer(sq_begin + sq_index, sq_size - sq_index, &(prikey->q));
    if (readed < 0)
        return -1;
    sq_index += readed;

    return sq_total_size;
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
NUT_API ssize_t der_read_pkcs8_private(const uint8_t *data, size_t available, RSA::PrivateKey *prikey)
{
    assert(nullptr != data && available > 0 && nullptr != prikey);

    // sq1
    const uint8_t *sq1_begin = nullptr;
    size_t sq1_size = 0, sq1_index = 0;
    ssize_t readed = der_read_sequence(data, available, &sq1_begin, &sq1_size);
    if (readed < 0)
        return -1;
    const ssize_t sq1_total_size = readed;

    // version
    if (sq1_index >= sq1_size)
        return -1;
    BigInteger version;
    readed = der_read_integer(sq1_begin + sq1_index, sq1_size - sq1_index, &version);
    if (readed < 0 || version != 0)
        return -1;
    sq1_index += readed;

    // sq2
    if (sq1_index >= sq1_size)
        return -1;
    const uint8_t *sq2_begin = nullptr;
    size_t sq2_size = 0, sq2_index = 0;
    readed = der_read_sequence(sq1_begin + sq1_index, sq1_size - sq1_index, &sq2_begin, &sq2_size);
    if (readed < 0)
        return -1;
    sq1_index += readed;

    // Object identifier
    if (sq2_index >= sq2_size)
        return -1;
    const uint8_t *id_begin = nullptr;
    size_t id_size = 0;
    readed = der_read_obj_ident(sq2_begin + sq2_index, sq2_size - sq2_index, &id_begin, &id_size);
    if (readed < 0)
        return -1;
    sq2_index += readed;
    const uint8_t rsa_ident[] = {0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01}; // Hex encoding of 1.2.840.113549.1.1
    if (id_size != sizeof(rsa_ident) || 0 != ::memcmp(id_begin, rsa_ident, sizeof(rsa_ident)))
        return -1; // Not RSA private key

    // Octet string
    if (sq1_index >= sq1_size)
        return -1;
    const uint8_t *os_begin = nullptr;
    size_t os_size = 0, os_index = 0;
    readed = der_read_octet_string(sq1_begin + sq1_index, sq1_size - sq1_index, &os_begin, &os_size);
    if (readed < 0 || sq1_index + readed != sq1_size)
        return -1;
    sq1_index += readed;

    // sq3
    if (os_index >= os_size)
        return -1;
    const uint8_t *sq3_begin = nullptr;
    size_t sq3_size = 0, sq3_index = 0;
    readed = der_read_sequence(os_begin + os_index, os_size - os_index, &sq3_begin, &sq3_size);
    if (readed < 0)
        return -1;
    os_index += readed;

    // version
    if (sq3_index >= sq3_size)
        return -1;
    readed = der_read_integer(sq3_begin + sq3_index, sq3_size - sq3_index, &version);
    if (readed < 0 || version != 0)
        return -1;
    sq3_index += readed;

    // n
    if (sq3_index >= sq3_size)
        return -1;
    readed = der_read_integer(sq3_begin + sq3_index, sq3_size - sq3_index, &(prikey->n));
    if (readed < 0)
        return -1;
    sq3_index += readed;

    // e
    if (sq3_index >= sq3_size)
        return -1;
    readed = der_read_integer(sq3_begin + sq3_index, sq3_size - sq3_index, &(prikey->e));
    if (readed < 0)
        return -1;
    sq3_index += readed;

    // d
    if (sq3_index >= sq3_size)
        return -1;
    readed = der_read_integer(sq3_begin + sq3_index, sq3_size - sq3_index, &(prikey->d));
    if (readed < 0)
        return -1;
    sq3_index += readed;

    // p
    if (sq3_index >= sq3_size)
        return -1;
    readed = der_read_integer(sq3_begin + sq3_index, sq3_size - sq3_index, &(prikey->p));
    if (readed < 0)
        return -1;
    sq3_index += readed;

    // q
    if (sq3_index >= sq3_size)
        return -1;
    readed = der_read_integer(sq3_begin + sq3_index, sq3_size - sq3_index, &(prikey->q));
    if (readed < 0)
        return -1;
    sq3_index += readed;

    return sq1_total_size;
}

}
