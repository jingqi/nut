
#include <assert.h>

#include "../../numeric/numeric_algo/mod.h"
#include "../../numeric/numeric_algo/prime.h"
#include "rsa.h"


namespace nut
{

RSA::PrivateKey RSA::gen_key(size_t max_bit_length)
{
    assert(max_bit_length >= 16);

    // 随机选取两个素数
    // NOTE:
    //  为了避免椭圆曲线因子分解算法，p、q应有大致相同的比特长度，且足够大。
    //  同时， p,q 不应太接近, 否则就容易分解, 保持几个比特长度差是可以的
    PrivateKey key;
    const unsigned p_len = (max_bit_length + 1) / 2 - 2,
        q_len = max_bit_length - p_len; // q_len - p_len = 3 or 4
    key.p = BigInteger::rand_positive(p_len, true);
    key.p = next_prime(key.p);

    key.q = BigInteger::rand_positive(q_len, true);
    key.q = next_prime(key.q);

    // 选取小奇数 e，使得 e 与 gamma_n 互质
    // NOTE:
    //  e 常取 3 和 65537，比特位中 bit1 少，利于提高计算速度
    key.e = 65537;

    // d 为 e 对模 gamma_n 的乘法逆元
    const BigInteger gamma_n = (key.p - 1) * (key.q - 1);
    extended_euclid(key.e, gamma_n, nullptr, &key.d, nullptr);
    if (!key.d.is_positive())
    {
        key.d %= gamma_n; // NOTE 模运算符(%)返回符号与被除数符号是一致的
        key.d += gamma_n;
    }

    key.n = key.p * key.q;

    return key;
}

BigInteger RSA::public_transfer(const BigInteger& m, const PublicKey& k)
{
    return pow_mod(m, k.e, k.n);
}

BigInteger RSA::private_transfer(const BigInteger& c, const PrivateKey& k)
{
    return pow_mod(c, k.d, k.n);
}

}
