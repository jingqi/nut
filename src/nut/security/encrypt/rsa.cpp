
#include <assert.h>

#include <nut/numeric/numeric_algo/mod.h>
#include <nut/numeric/numeric_algo/prime.h>

#include "rsa.h"

namespace nut
{

void RSA::gen_key(size_t bit_count, PublicKey *public_key, PrivateKey *private_key)
{
    assert(bit_count > 10);

    // 随机选取两个素数
    // NOTE:
    //  为了避免椭圆曲线因子分解算法，p、q应有大致相同的比特长度，且足够大。
    //  同时， p,q 不应太接近, 否则就容易分解, 保持几个比特长度差是可以的
    BigInteger bound(1);
    bound <<= ((bit_count + 1) / 2);
    BigInteger p = BigInteger::rand_between(bound, bound << 1);
    p = next_prime(p);
    bound <<= 3;
    BigInteger q = BigInteger::rand_between(bound, bound << 1);
    q = next_prime(q);

    // 选取小奇数 e，使得 e 与 gamma_n 互质
    BigInteger n(p * q), gamma_n = (p - 1) * (q - 1);
    // NOTE:
    //  e 常取 3 和 65537，比特位 bit1 少，利于提高计算速度
    unsigned e = 65537;

    // d 为 e 对模 gamma_n 的乘法逆元
    BigInteger d(0);
    extended_euclid(BigInteger(e), gamma_n, NULL, &d, NULL);
    if (d < 0)
        d = gamma_n + (d % gamma_n); // % 运算符号与被除数一致

    // 公钥 (e, n)
    if (NULL != public_key)
    {
        public_key->e = e;
        public_key->n = n;
    }

    // 私钥(d, n)
    if (NULL != private_key)
    {
        private_key->d = std::move(d);
        private_key->n = std::move(n);
    }
}

void RSA::encode(const BigInteger& m, const PublicKey& k, BigInteger *rs)
{
    assert(NULL != rs);
    mod_pow(m, k.e, k.n, rs);
}

void RSA::decode(const BigInteger& c, const PrivateKey& k, BigInteger *rs)
{
    assert(NULL != rs);
    mod_pow(c, k.d, k.n, rs);
}

}
