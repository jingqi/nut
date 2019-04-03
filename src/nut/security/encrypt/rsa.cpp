
#include <assert.h>

#include <nut/numeric/numeric_algo/mod.h>
#include <nut/numeric/numeric_algo/prime.h>

#include "rsa.h"

namespace nut
{

void RSA::gen_key(size_t max_bit_length, PublicKey *public_key, PrivateKey *private_key)
{
    assert(max_bit_length >= 16);

    // 随机选取两个素数
    // NOTE:
    //  为了避免椭圆曲线因子分解算法，p、q应有大致相同的比特长度，且足够大。
    //  同时， p,q 不应太接近, 否则就容易分解, 保持几个比特长度差是可以的
    const unsigned p_len = (max_bit_length + 1) / 2 - 2,
        q_len = max_bit_length - p_len; // q_len - p_len = 3 or 4
    BigInteger p = BigInteger::rand_positive(p_len, true);
    BigInteger q = BigInteger::rand_positive(q_len, true);
    p = next_prime(p);
    q = next_prime(q);

    // 选取小奇数 e，使得 e 与 gamma_n 互质
    // NOTE:
    //  e 常取 3 和 65537，比特位中 bit1 少，利于提高计算速度
    const BigInteger n = p * q, gamma_n = (p - 1) * (q - 1);
    const unsigned e = 65537;

    // d 为 e 对模 gamma_n 的乘法逆元
    BigInteger d(0);
    extended_euclid(BigInteger(e), gamma_n, nullptr, &d, nullptr);
    if (d < 0)
        d = gamma_n + (d % gamma_n); // 模运算符(%)返回符号与被除数符号是一致的

    // 公钥 (e, n)
    if (nullptr != public_key)
    {
        public_key->e = e;
        public_key->n = n;
    }

    // 私钥(d, n)
    if (nullptr != private_key)
    {
        private_key->d = std::move(d);
        private_key->n = std::move(n);
    }
}

BigInteger RSA::transfer(const BigInteger& m, const PublicKey& k)
{
    return mod_pow(m, k.e, k.n);
}

BigInteger RSA::transfer(const BigInteger& c, const PrivateKey& k)
{
    return mod_pow(c, k.d, k.n);
}

}
