
#ifndef ___HEADFILE_12FB3097_3C87_4104_A964_9BF5D45EF28E_
#define ___HEADFILE_12FB3097_3C87_4104_A964_9BF5D45EF28E_

#include <nut/numeric/big_integer.h>

#include "../../nut_config.h"


namespace nut
{

class NUT_API RSA
{
public:
    class PublicKey
    {
    public:
        BigInteger e;
        BigInteger n;
    };

    class PrivateKey
    {
    public:
        BigInteger d, n;
    };

public:
    static void gen_key(size_t bit_count, PublicKey *public_key, PrivateKey *private_key);

    static void encode(const BigInteger& m, const PublicKey& k, BigInteger *rs);

    static void decode(const BigInteger& c, const PrivateKey& k, BigInteger *rs);
};

}

#endif
