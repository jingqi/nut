/**
 * PEM 格式参看 https://www.lightky.com/backend/2016/09/rsa-pem-format.html
 *
 * Openssl 生成私钥、公钥:
 *   openssl genrsa -out rsa_private.pem 1024
 *   openssl rsa -in rsa_private.pem -pubout -out rsa_public.pem
 */

#include <string.h> // for ::strlen()
#include <algorithm> // for std::min()

#include "../../../util/string/string_utils.h"
#include "pem.h"
#include "der_reader.h"
#include "der_writer.h"


namespace nut
{

namespace
{

// PKCS#1
const char *BEGIN_RSA_PUBLIC_KEY = "-----BEGIN RSA PUBLIC KEY-----";
const char *END_RSA_PUBLIC_KEY = "-----END RSA PUBLIC KEY-----";

const char *BEGIN_RSA_PRIVATE_KEY = "-----BEGIN RSA PRIVATE KEY-----";
const char *END_RSA_PRIVATE_KEY = "-----END RSA PRIVATE KEY-----";

// PKCS#8
const char *BEGIN_PUBLIC_KEY = "-----BEGIN PUBLIC KEY-----";
const char *END_PUBLIC_KEY = "-----END PUBLIC KEY-----";

const char *BEGIN_PRIVATE_KEY = "-----BEGIN PRIVATE KEY-----";
const char *END_PRIVATE_KEY = "-----END PRIVATE KEY-----";

}

NUT_API bool pem_read_rsa_public(const std::string& pem, RSA::PublicKey *pubkey)
{
    assert(nullptr != pubkey);

    // PKCS#1
    size_t begin = pem.find(BEGIN_RSA_PUBLIC_KEY);
    if (std::string::npos != begin)
    {
        begin += ::strlen(BEGIN_RSA_PUBLIC_KEY);
        const size_t end = pem.find(END_RSA_PUBLIC_KEY, begin);
        if (std::string::npos == end)
            return false;
        const std::vector<uint8_t> der =  nut::base64_decode(pem.c_str() + begin, end - begin);
        return der_read_pkcs1_public(der.data(), der.size(), pubkey) > 0;
    }

    // PKCS#8
    begin = pem.find(BEGIN_PUBLIC_KEY);
    if (std::string::npos == begin)
        return false;
    begin += ::strlen(BEGIN_PUBLIC_KEY);
    const size_t end = pem.find(END_PUBLIC_KEY, begin);
    if (std::string::npos == end)
        return false;
    const std::vector<uint8_t> der =  nut::base64_decode(pem.c_str() + begin, end - begin);
    return der_read_pkcs8_public(der.data(), der.size(), pubkey) > 0;
}

NUT_API bool pem_read_rsa_private(const std::string& pem, RSA::PrivateKey *prikey)
{
    assert(nullptr != prikey);

    // PKCS#1
    size_t begin = pem.find(BEGIN_RSA_PRIVATE_KEY);
    if (std::string::npos != begin)
    {
        begin += ::strlen(BEGIN_RSA_PRIVATE_KEY);
        const size_t end = pem.find(END_RSA_PRIVATE_KEY, begin);
        if (std::string::npos == end)
            return false;
        const std::vector<uint8_t> der =  nut::base64_decode(pem.c_str() + begin, end - begin);
        return der_read_pkcs1_private(der.data(), der.size(), prikey) > 0;
    }

    // PKCS#8
    begin = pem.find(BEGIN_PRIVATE_KEY);
    if (std::string::npos == begin)
        return false;
    begin += ::strlen(BEGIN_PRIVATE_KEY);
    const size_t end = pem.find(END_PRIVATE_KEY, begin);
    if (std::string::npos == end)
        return false;
    const std::vector<uint8_t> der =  nut::base64_decode(pem.c_str() + begin, end - begin);
    return der_read_pkcs8_private(der.data(), der.size(), prikey) > 0;
}

NUT_API std::string pem_write_rsa_public(const RSA::PublicKey& pubkey, bool pkcs8)
{
    std::vector<uint8_t> der;
    if (pkcs8)
        der_write_pkcs8_public(&der, pubkey);
    else
        der_write_pkcs1_public(&der, pubkey);

    std::string ret(pkcs8 ? BEGIN_PUBLIC_KEY : BEGIN_RSA_PUBLIC_KEY);
    ret.push_back('\n');
    const std::string base64 = base64_encode(der.data(), der.size());
    for (size_t i = 0, len = base64.length(); i < len; i += 64)
    {
        ret.insert(ret.end(), base64.data() + i, base64.data() + std::min(len, i + 64));
        ret.push_back('\n');
    }
    ret += (pkcs8 ? END_PUBLIC_KEY : END_RSA_PUBLIC_KEY);
    return ret;
}

NUT_API std::string pem_write_rsa_private(const RSA::PrivateKey& prikey, bool pkcs8)
{
    std::vector<uint8_t> der;
    if (pkcs8)
        der_write_pkcs8_private(&der, prikey);
    else
        der_write_pkcs1_private(&der, prikey);

    std::string ret(pkcs8 ? BEGIN_PRIVATE_KEY : BEGIN_RSA_PRIVATE_KEY);
    ret.push_back('\n');
    const std::string base64 = base64_encode(der.data(), der.size());
    for (size_t i = 0, len = base64.length(); i < len; i += 64)
    {
        ret.insert(ret.end(), base64.data() + i, base64.data() + std::min(len, i + 64));
        ret.push_back('\n');
    }
    ret += (pkcs8 ? END_PRIVATE_KEY : END_RSA_PRIVATE_KEY);
    return ret;
}

}
