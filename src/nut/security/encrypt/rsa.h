﻿
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
        BigInteger e, n;
    };

    class PrivateKey
    {
    public:
        BigInteger d, n;
    };

public:
    /**
     * 生成密钥对
     *
     * @param max_bit_length 最大密钥长度，最终生成比特长度为 max_bit_length - 1
     *                       或者 max_bit_length (因为 M 位正整数乘以 N 位正整数
     *                       的结果为 M + N -1 位或者 M + N 位)
     */
    static void gen_key(size_t max_bit_length, PublicKey *public_key, PrivateKey *private_key);

    static BigInteger transfer(const BigInteger& m, const PublicKey& k);
    static BigInteger transfer(const BigInteger& c, const PrivateKey& k);
};

}

#endif
