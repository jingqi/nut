
#ifndef ___HEADFILE_0B60CC46_3A1F_4574_AE13_0B50E6E5371B_
#define ___HEADFILE_0B60CC46_3A1F_4574_AE13_0B50E6E5371B_

#include <stdint.h>
#include <vector>

#include "../../../nut_config.h"
#include "../../../numeric/big_integer.h"
#include "../rsa.h"


namespace nut
{

NUT_API void der_write_len(std::vector<uint8_t> *output, size_t len) noexcept;
NUT_API void der_write_integer(std::vector<uint8_t> *output, const BigInteger& v) noexcept;
NUT_API void der_write_bit_string(std::vector<uint8_t> *output, const std::vector<uint8_t>& bs) noexcept;
NUT_API void der_write_octet_string(std::vector<uint8_t> *output, const std::vector<uint8_t>& os) noexcept;
NUT_API void der_write_sequence(std::vector<uint8_t> *output, const std::vector<uint8_t>& sq) noexcept;
NUT_API void der_write_obj_ident(std::vector<uint8_t> *output, const uint8_t *id_begin, size_t id_size) noexcept;
NUT_API void der_write_null(std::vector<uint8_t> *output) noexcept;

NUT_API void der_write_pkcs1_public(std::vector<uint8_t> *output, const RSA::PublicKey& pubkey) noexcept;
NUT_API void der_write_pkcs8_public(std::vector<uint8_t> *output, const RSA::PublicKey& pubkey) noexcept;

NUT_API void der_write_pkcs1_private(std::vector<uint8_t> *output, const RSA::PrivateKey& prikey) noexcept;
NUT_API void der_write_pkcs8_private(std::vector<uint8_t> *output, const RSA::PrivateKey& prikey) noexcept;

}

#endif
