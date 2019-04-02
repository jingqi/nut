﻿
#include <assert.h>

#include <nut/numeric/numeric_algo/mod.h>
#include <nut/numeric/numeric_algo/prime.h>

#include "rsa.h"

namespace nut
{

size_t RSA::KeyBase::max_input_bit_size() const
{
    assert(n.is_positive());
    return n.bit_length() - 1; // 输入必须小于 n
}

size_t RSA::KeyBase::max_output_bit_size() const
{
    assert(n.is_positive());
    return n.bit_length(); // 输出一定小于 n
}

void RSA::gen_key(size_t bit_count, PublicKey *public_key, PrivateKey *private_key)
{
    assert(bit_count > 10);

    // 随机选取两个素数
    // NOTE:
    //  为了避免椭圆曲线因子分解算法，p、q应有大致相同的比特长度，且足够大。
    //  同时， p,q 不应太接近, 否则就容易分解, 保持几个比特长度差是可以的
    BigInteger bound(1);
    bound <<= (bit_count + 1) / 2 - 2;
    BigInteger p = BigInteger::rand_between(bound, bound << 1);
    p = next_prime(p);
    bound <<= 4;
    BigInteger q = BigInteger::rand_between(bound, bound << 1);
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

BigInteger RSA::encode(const BigInteger& m, const PublicKey& k)
{
    BigInteger ret;
    mod_pow(m, k.e, k.n, &ret);
    return ret;
}

BigInteger RSA::decode(const BigInteger& c, const PrivateKey& k)
{
    BigInteger ret;
    mod_pow(c, k.d, k.n, &ret);
    return ret;
}

}
