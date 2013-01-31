/**
 * @file -
 * @author jingqi
 * @date 2012-11-25
 * @last-edit 2013-01-25 15:42:04 jingqi
 */

#ifndef ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_
#define ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_

#include "biginteger.hpp"
#include "bitsieve.hpp"

namespace nut
{

/**
 * 求(a**b)%n，即a的b次方(模n)
 */
inline BigInteger modular_exponentiation(const BigInteger& a, const BigInteger& b, const BigInteger& n)
{
    BigInteger ret(1);
    for (register size_t i = b.significant_length() * 8; i > 0; --i) // 从高位向低有效位取bit
    {
        ret = (ret * ret) % n;
        if (0 != b.bit_at(i - 1))
            ret = (ret * a) % n;
    }
    return ret;
}

/**
 * 利用二进制特性的gcd算法
 */
inline BigInteger gcd(const BigInteger& a, const BigInteger& b)
{
    if (b.is_zero())
        return a;

    if (a.bit_at(0) == 0 && b.bit_at(0) == 0)
        return gcd(a >> 1, b >> 1) << 1;
    else if (a.bit_at(0) == 0)
        return gcd(a >> 1, b);
    else if (b.bit_at(0) == 0)
        return gcd(a, b >> 1);
    else if (a > b)
        return gcd((a - b) >> 1, b);
    else
        return gcd(a, (b - a) >> 1);
}

/**
 * 欧几里德(EUCLID)算法，求最大公约数
 */
BigInteger euclid_gcd(const BigInteger& a, const BigInteger& b)
{
    if (b.is_zero())
        return a;
    return euclid_gcd(b, a % b);
}

/**
 * 欧几里得(EUCLID)算法的推广形式
 * d = gcd(a, b) = ax + by
 */
void extended_euclid(const BigInteger& a, const BigInteger& b, BigInteger *d, BigInteger *x, BigInteger *y)
{
    if (b.is_zero())
    {
        if (NULL != d)
            *d = a;
        if (NULL != x)
            *x = BigInteger(1);
        if (NULL != y)
            *y = BigInteger(0);
        return;
    }
    
    BigInteger xx, yy;
    extended_euclid(b, a % b, d, &xx, &yy);
    if (NULL != x)
        *x = yy;
    if (NULL != y)
        *y = xx - a / b * yy;
}

/**
 * 伪素数测试法
 */
bool psedoprime(const BigInteger& n)
{
    const BigInteger ONE(1), TWO(2);
    if (ONE != modular_exponentiation(TWO, n - ONE, n))
        return false; // 一定是合数
    return true; // 可能是素数
}

bool _miller_rabin_witness(const BigInteger& a, const BigInteger& n)
{
    BigInteger d(1), b(n - BigInteger(1));
    for (register size_t i = b.significant_length() * 8; i > 0; --i)
    {
        BigInteger x = d;
        d = (d * d) % n;
        if (d == BigInteger(1) && x != BigInteger(1) && x != b)
            return true;
        if (0 != b.bit_at(i - 1))
            d = (d * a) % n;
    }
    if (d != BigInteger(1))
        return true;
    return false;
}

/**
 * Miller-Rabin 素数测试
 */
bool miller_rabin(const BigInteger& n, unsigned s)
{
    for (register size_t i = 0; i < s; ++i)
    {
        BigInteger a = BigInteger::rand_between(BigInteger(1), n - BigInteger(1));
        if (_miller_rabin_witness(a, n))
            return false; // 一定是合数
    }
    return true; // 几乎肯定是素数
}

/**
 * 取下一个可能的素数
 */
BigInteger nextProbablePrime(const BigInteger& n)
{
    if (n <= BigInteger(1))
        return BigInteger(2);

    const int SMALL_PRIME_THRESHOLD = 95;
    const int DEFAULT_PRIME_CERTAINTY = 1;
    const BigInteger SMALL_PRIME_PRODUCT(((uint64_t) 3) * 5 * 7 * 11 * 13 * 17 * 19 * 23 * 29 * 31 * 37 * 41);

    BigInteger result = n + BigInteger(1);

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
                        result += BigInteger(2);
                        continue; // Candidate is composite; try another
                }
            }

            // All candidates of bitLength 2 and 3 are prime by this point
            if (result.bit_length() < 4)
                return result;

            // The expensive test
            if (miller_rabin(result, DEFAULT_PRIME_CERTAINTY))
                return result;

            result += BigInteger(2);
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
        result += BigInteger(2 * searchLen);
    }
}

}

#endif


