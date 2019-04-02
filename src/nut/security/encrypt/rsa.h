
#ifndef ___HEADFILE_12FB3097_3C87_4104_A964_9BF5D45EF28E_
#define ___HEADFILE_12FB3097_3C87_4104_A964_9BF5D45EF28E_

#include <nut/numeric/big_integer.h>

#include "../../nut_config.h"


namespace nut
{

class NUT_API RSA
{
public:
    class KeyBase
    {
    public:
        /**
         * 单次允许输入的最长比特数
         */
        size_t max_input_bit_size() const;

        /**
         * 单次可能输出的最长比特数
         */
        size_t max_output_bit_size() const;

    public:
        BigInteger n;
    };

    class PublicKey : public KeyBase
    {
    public:
        BigInteger e;
    };

    class PrivateKey : public KeyBase
    {
    public:
        BigInteger d;
    };

public:
    static void gen_key(size_t bit_count, PublicKey *public_key, PrivateKey *private_key);

    static BigInteger encode(const BigInteger& m, const PublicKey& k);

    static BigInteger decode(const BigInteger& c, const PrivateKey& k);
};

}

#endif
