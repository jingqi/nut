/**
 * @file -
 * @author jingqi
 * @date 2012-11-25
 * @last-edit 2013-01-25 15:42:08 jingqi
 */

#ifndef ___HEADFILE_12FB3097_3C87_4104_A964_9BF5D45EF28E_
#define ___HEADFILE_12FB3097_3C87_4104_A964_9BF5D45EF28E_

#include <assert.h>

#include <nut/numeric/biginteger.hpp>
#include <nut/numeric/numeric_algo.hpp>

#include <assert.h>

#include <nut/numeric/biginteger.hpp>
#include <nut/numeric/numeric_algo.hpp>

namespace nut
{

class RSA
{
public:
    struct PublicKey
    {
        unsigned e;
        BigInteger n;
    };

    struct PrivateKey
    {
        BigInteger d, n;
    };

    static void genKey(size_t bit_count, PublicKey *publicKey, PrivateKey *privateKey)
    {
        assert(bit_count > 10);

        // 随机选取两个素数 
        // NOTE:
        //	为了避免椭圆曲线因子分解算法，p、q应有大致相同的比特长度，且足够大。
        //  同时， p,q 不应太接近, 否则就容易分解, 保持几个比特长度差是可以的
        BigInteger bound(1);
        bound <<= ((bit_count + 1) / 2);
        BigInteger p = BigInteger::rand_between(bound, bound << 1);
        p = nextProbablePrime(p);
        bound <<= 3;
        BigInteger q = BigInteger::rand_between(bound, bound << 1);
        q = nextProbablePrime(q);

        // 选取小奇数 e，使得 e 与 gamma_n 互质
        const BigInteger n(p * q), gamma_n = (p - 1) * (q - 1);
        // NOTE:
        // 	e 常取 3 和 65537，比特位 bit1 少，利于提高计算速度
        unsigned e = 65537;

        // d 为 e 对模 gamma_n 的乘法逆元 
        BigInteger d;
        extended_euclid(BigInteger(e), gamma_n, NULL, &d, NULL);
        if (d < 0)
            d = gamma_n + (d % gamma_n); // % 运算符号与被除数一致

        // 公钥 (e, n)
        if (NULL != publicKey)
        {
            publicKey->e = e;
            publicKey->n = n;
        }

        // 私钥(d, n)
        if (NULL != privateKey)
        {
            privateKey->d = d;
            privateKey->n = n;
        }
    }

    static BigInteger encode(const BigInteger& m, const PublicKey& k)
    {
        return modular_exponentiation(m, BigInteger(k.e), k.n);
    }

    static BigInteger decode(const BigInteger& c, const PrivateKey& k)
    {
        return modular_exponentiation(c, k.d, k.n);
    }
};

}

#endif


