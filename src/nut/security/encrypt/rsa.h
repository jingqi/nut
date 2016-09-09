
#ifndef ___HEADFILE_12FB3097_3C87_4104_A964_9BF5D45EF28E_
#define ___HEADFILE_12FB3097_3C87_4104_A964_9BF5D45EF28E_

#include <nut/nut_config.h>
#include <nut/numeric/big_integer.h>

namespace nut
{

class NUT_API RSA
{
public:
    struct PublicKey
    {
        BigInteger e;
        BigInteger n;
    };

    struct PrivateKey
    {
        BigInteger d, n;
    };

    static void gen_key(size_t bit_count, PublicKey *public_key, PrivateKey *private_key);

    static void encode(const BigInteger& m, const PublicKey& k, BigInteger *rs);

    static void decode(const BigInteger& c, const PrivateKey& k, BigInteger *rs);
};

}

#endif
