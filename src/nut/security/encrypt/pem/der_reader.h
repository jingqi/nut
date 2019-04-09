
#ifndef ___HEADFILE_B8331BB6_1126_4255_A278_E60D5B9FBD49_
#define ___HEADFILE_B8331BB6_1126_4255_A278_E60D5B9FBD49_

#include "../../../nut_config.h"
#include "../../../platform/int_type.h"
#include "../../../numeric/big_integer.h"
#include "../rsa.h"


namespace nut
{

NUT_API ssize_t der_read_len(const uint8_t *data, size_t available, size_t *rs);
NUT_API ssize_t der_read_integer(const uint8_t *data, size_t available, BigInteger *rs);
NUT_API ssize_t der_read_bit_string(const uint8_t *data, size_t available, const uint8_t **bs_begin, size_t *bs_size);
NUT_API ssize_t der_read_octet_string(const uint8_t *data, size_t available, const uint8_t **os_begin, size_t *os_size);
NUT_API ssize_t der_read_sequence(const uint8_t *data, size_t available, const uint8_t **sq_begin, size_t *sq_size);
NUT_API ssize_t der_read_obj_ident(const uint8_t *data, size_t available, const uint8_t **id_begin, size_t *id_size);
NUT_API ssize_t der_read_null(const uint8_t *data, size_t available);

NUT_API ssize_t der_read_pkcs1_public(const uint8_t *data, size_t available, RSA::PublicKey *pubkey);
NUT_API ssize_t der_read_pkcs8_public(const uint8_t *data, size_t available, RSA::PublicKey *pubkey);

NUT_API ssize_t der_read_pkcs1_private(const uint8_t *data, size_t available, RSA::PrivateKey *prikey);
NUT_API ssize_t der_read_pkcs8_private(const uint8_t *data, size_t available, RSA::PrivateKey *prikey);

}

#endif
