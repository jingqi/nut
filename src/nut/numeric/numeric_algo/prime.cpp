﻿
#include "prime.h"
#include "bit_sieve.h"
#include "mod.h"


namespace nut
{

/**
 * 费马小定理素数测试法, 伪素数测试
 *
 * 参考文献：
 *     [1]潘金贵，顾铁成. 现代计算机常用数据结构和算法[M]. 南京大学出版社. 1994. 582
 */
bool psedoprime(const BigInteger& n)
{
    BigInteger mp(0);
    mod_pow(BigInteger(2), n - 1, n, &mp);
    if (mp != 1)
        return false; // 一定是合数
    return true; // 可能是素数
}

#if 0 // unoptimized
static bool _miller_rabin_witness(const BigInteger& a, const BigInteger& n)
{
    BigInteger d(1), b(n);
    --b;
    for (size_t i = b.bit_length(); i > 0; --i)
    {
        BigInteger x = d;
        d *= d;
        d %= n;
        if (d == 1 && x != 1 && x != b)
            return true;
        if (0 != b.bit_at(i - 1))
        {
            d *= a;
            d %= n;
        }
    }
    if (d != 1)
        return true;
    return false;
}
#endif

/**
 * 米勒-拉宾(Miller-Rabin)素数测试
 */
bool miller_rabin(const BigInteger& n, unsigned s)
{
    assert(s > 0);

#if 0 // unoptimized
    /**
     * 米勒-拉宾(Miller-Rabin)素数测试
     *
     * 参考文献：
     *     [1]潘金贵，顾铁成. 现代计算机常用数据结构和算法[M]. 南京大学出版社. 1994. 584
     */
    const BigInteger ONE(1);
    for (size_t i = 0; i < s; ++i)
    {
        const BigInteger a = BigInteger::rand_between(ONE, n); // rand in [1, n)
        if (_miller_rabin_witness(a, n))
            return false; // 一定是合数
    }
    return true; // 几乎肯定是素数
#else
    /**
     * Miller-Rabin 素数测试
     * 参见java语言BigInteger.passesMillerRabin()实现
     */
    const BigInteger ONE(1), TWO(2);

    // Find a and m such that m is odd and n == 1 + 2**a * m
    BigInteger this_minus_one(n);
    --this_minus_one;
    BigInteger m(this_minus_one);
    const int a = m.lowest_bit();
    m >>= a;

    // Do the tests
    for (size_t i = 0; i < s; ++i)
    {
        // Generate a uniform random in [1, n)
        const BigInteger b = BigInteger::rand_between(ONE, n); // _rand_1_n(n);

        int j = 0;
        BigInteger z(0);
        mod_pow(b, m, n, &z);
        while (!((j == 0 && z == ONE) || z == this_minus_one))
        {
            if ((j > 0 && z == ONE) || ++j == a)
                return false;
            mod_pow(z, TWO, n, &z);
        }
    }
    return true;
#endif
}

/**
 * 取下一个可能的素数
 * 参见java语言BigInteger.nextProbablePrime()实现
 */
NUT_API BigInteger next_prime(const BigInteger& n)
{
    if (n <= 1)
        return BigInteger(2);

    const size_t SMALL_PRIME_THRESHOLD = 95;
    const size_t DEFAULT_PRIME_CERTAINTY = 2;
    const BigInteger SMALL_PRIME_PRODUCT(((uint64_t) 3) * 5 * 7 * 11 * 13 * 17 * 19 *
            23 * 29 * 31 * 37 * 41);

    BigInteger result(n);
    ++result;

    // Fastpath for small numbers
    if (result.bit_length() < SMALL_PRIME_THRESHOLD)
    {
        // Ensure an odd number
        if (result.bit_at(0) == 0)
            ++result;

        while(true)
        {
            // Do cheap "pre-test" if applicable
            if (result.bit_length() > 6)
            {
                const int64_t r = (int64_t) (result % SMALL_PRIME_PRODUCT).to_integer();
                if ((0 == r % 3)  || (0 == r % 5)  || (0 == r % 7)  || (0 == r % 11) ||
                    (0 == r % 13) || (0 == r % 17) || (0 == r % 19) || (0 == r % 23) ||
                    (0 == r % 29) || (0 == r % 31) || (0 == r % 37) || (0 == r % 41))
                {
                        result += 2;
                        continue; // Candidate is composite; try another
                }
            }

            // All candidates of bit_length 2 and 3 are prime by this point
            if (result.bit_length() < 4)
                return result;

            // The expensive test
            if (miller_rabin(result, DEFAULT_PRIME_CERTAINTY))
                return result;

            result += 2;
        }
    }

    // Start at previous even number
    if (result.bit_at(0) == 1)
        --result;

    // Looking for the next large prime
    size_t search_len = (result.bit_length() / 20) * 64;

    while (true)
    {
        BitSieve search_sieve(result, (int) search_len);
        const BigInteger candidate = search_sieve.retrieve(result, DEFAULT_PRIME_CERTAINTY);
        if (!candidate.is_zero())
            return candidate;
        result += 2 * search_len;
    }
}

}
