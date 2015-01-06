﻿/**
 * @file -
 * @author jingqi
 * @date 2013-02-08
 * @last-edit 2015-01-06 22:44:34 jingqi
 * @brief
 */

#ifndef ___HEADFILE_526CD359_EF76_437B_ACDD_D24322AB6FDC_
#define ___HEADFILE_526CD359_EF76_437B_ACDD_D24322AB6FDC_

#include "../big_integer.hpp"
#include "bit_sieve.hpp"

// 优化程度，>= 0
#define OPTIMIZE_LEVEL 1000

namespace nut
{

/**
 * 费马小定理素数测试法, 伪素数测试
 * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P582
 */
inline bool psedoprime(const BigInteger& n)
{
    if (mod_pow(BigInteger(2), n - 1, n) != 1)
        return false; // 一定是合数
    return true; // 可能是素数
}

inline bool _miller_rabin_witness(const BigInteger& a, const BigInteger& n)
{
    BigInteger d(1), b(n - 1);
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

/**
 * 米勒-拉宾(Miller-Rabin)素数测试
 */
inline bool miller_rabin(const BigInteger& n, unsigned s)
{
    assert(s > 0);

#if (OPTIMIZE_LEVEL == 0)
    /**
     * 米勒-拉宾(Miller-Rabin)素数测试
     * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P584
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
    const BigInteger thisMinusOne(n - ONE);
    BigInteger m(thisMinusOne);
    const int a = m.lowest_bit();
    m >>= a;

    // Do the tests
    for (size_t i = 0; i < s; ++i)
    {
        // Generate a uniform random in [1, n)
        const BigInteger b = BigInteger::rand_between(ONE, n); // _rand_1_n(n);

        int j = 0;
        BigInteger z = mod_pow(b, m, n);
        while(!((j == 0 && z == ONE) || z == thisMinusOne))
        {
            if ((j > 0 && z == ONE) || ++j == a)
                return false;
            z = mod_pow(z, TWO, n);
        }
    }
    return true;
#endif
}

/**
 * 取下一个可能的素数
 * 参见java语言BigInteger.nextProbablePrime()实现
 */
inline BigInteger next_prime(const BigInteger& n)
{
    if (n <= 1)
        return BigInteger(2);

    const size_t SMALL_PRIME_THRESHOLD = 95;
    const size_t DEFAULT_PRIME_CERTAINTY = 2;
    const BigInteger SMALL_PRIME_PRODUCT(((uint64_t) 3) * 5 * 7 * 11 * 13 * 17 * 19 * 23 * 29 * 31 * 37 * 41);

    BigInteger result = n + 1;

    // Fastpath for small numbers
    if (result.bit_length() < SMALL_PRIME_THRESHOLD)
    {
        // Ensure an odd number
        if (result.bit_at(0) == 0)
            ++result;

        while(true)
        {
            // Do cheap "pre-test" if applicable
            if (result.bit_length() > 6) {
                int64_t r = (int64_t) (result % SMALL_PRIME_PRODUCT).llong_value();
                if ((r%3==0)  || (r%5==0)  || (r%7==0)  || (r%11==0) ||
                    (r%13==0) || (r%17==0) || (r%19==0) || (r%23==0) ||
                    (r%29==0) || (r%31==0) || (r%37==0) || (r%41==0)) {
                        result += 2;
                        continue; // Candidate is composite; try another
                }
            }

            // All candidates of bitLength 2 and 3 are prime by this point
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
    int searchLen = (result.bit_length() / 20) * 64;

    while(true) {
        BitSieve searchSieve(result, searchLen);
        BigInteger candidate = searchSieve.retrieve(result,
            DEFAULT_PRIME_CERTAINTY);
        if (!candidate.is_zero())
            return candidate;
        result += 2 * searchLen;
    }
}

}

#undef OPTIMIZE_LEVEL

#endif
