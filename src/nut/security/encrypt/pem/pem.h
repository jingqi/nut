
#ifndef ___HEADFILE_30280161_1CA6_4078_A84B_AF6D7481A915_
#define ___HEADFILE_30280161_1CA6_4078_A84B_AF6D7481A915_

#include <string>

#include "../../../nut_config.h"
#include "../rsa.h"


namespace nut
{

NUT_API bool pem_read_rsa_public(const std::string& pem, RSA::PublicKey *pubkey);
NUT_API bool pem_read_rsa_private(const std::string& pem, RSA::PrivateKey *prikey);

NUT_API std::string pem_write_rsa_public(const RSA::PublicKey& pubkey, bool pkcs8 = false);
NUT_API std::string pem_write_rsa_private(const RSA::PrivateKey& prikey, bool pkcs8 = false);

}

#endif
