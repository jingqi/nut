/**
 * @file -
 * @author jingqi
 * @date 2012-11-25
 * @last-edit 2013-02-01 09:02:12 jingqi
 */

#ifndef ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_
#define ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_

#include "biginteger.hpp"
#include "bitsieve.hpp"

// 优化程度，>= 0
#define OPTIMIZE_LEVEL 1000

namespace nut
{

/**
 * 求(a**b)%n，即a的b次方(模n)
 * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P576
 */
inline BigInteger modular_exponentiation(const BigInteger& a, const BigInteger& b, const BigInteger& n)
{
    assert(a.is_positive() && b.is_positive() && n.is_positive());

    BigInteger ret(1);
    for (register size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
    {
        ret = (ret * ret) % n;
        if (0 != b.bit_at(i - 1))
            ret = (ret * a) % n;
    }
    return ret;
}

/**
 * 求最大公约数
 */
inline BigInteger gcd(const BigInteger& a, const BigInteger& b)
{
#if (OPTIMIZE_LEVEL == 0)
    /**
     * 欧几里德(EUCLID)算法，求最大公约数
     * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P563
     */
    if (b.is_zero())
        return a;
    return gcd(b, a % b);
#elif (OPTIMIZE_LEVEL == 1)
    /**
     * 利用二进制特性的gcd算法
     * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P590
     */
    if (b.is_zero())
        return a;
    if (a.bit_at(0) == 0 && b.bit_at(0) == 0)
        return gcd(a >> 1, b >> 1) << 1;
    else if (a.bit_at(0) == 0)
        return gcd(b, a >> 1);
    else if (b.bit_at(0) == 0)
        return gcd(a, b >> 1);
    else if (a > b)
        return gcd(b, (a - b) >> 1);
    else
        return gcd(a, (b - a) >> 1);
#else
    /**
     * 进一步优化上一个实现
     */
    size_t left_shift = 0;
    BigInteger aa(a), bb(b);
    while (!bb.is_zero())
    {
        if (aa.bit_at(0) == 0 && bb.bit_at(0) == 0)
        {
            aa >>= 1;
            bb >>= 1;
            ++left_shift;
        }
        else if (aa.bit_at(0) == 0)
        {
            aa >>= 1;
            BigInteger::swap(&aa, &bb); // 交换a b, 尽量使 b < a
        }
        else if (bb.bit_at(0) == 0)
        {
            bb >>= 1;
        }
        else if (aa > bb)
        {
            aa = (aa - bb) >> 1;
            BigInteger::swap(&aa, &bb); // 交换a b, 尽量使 b < a
        }
        else
        {
            bb = (bb - aa) >> 1;
        }
    }
    if (left_shift == 0)
        return aa;
    return aa << left_shift;
#endif
}

/**
 * 扩展欧几里得算法
 */
inline void extended_euclid(const BigInteger& a, const BigInteger& b, BigInteger *d, BigInteger *x, BigInteger *y)
{
#if (OPTIMIZE_LEVEL == 0)
    /**
     * 欧几里得(EUCLID)算法的推广形式
     * d = gcd(a, b) = ax + by
     * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P564
     */
    if (b.is_zero())
    {
        if (NULL != d)
            *d = a;
        if (NULL != x)
            *x = 1;
        if (NULL != y)
            *y = 0;
        return;
    }
    
    BigInteger xx, yy;
    extended_euclid(b, a % b, d, &yy, &xx);
    if (NULL != x)
        *x = xx;
    if (NULL != y)
        *y = yy - a / b * xx;
#else
    /**
     * 推广的 Euclidean 算法
     * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P590
     * 参见 《公开密钥密码算法及其快速实现》.周玉洁.冯国登.国防工业出版社.2002 P63
     */
    if (b.is_zero())
    {
        if (NULL != d)
            *d = a;
        if (NULL != x)
            *x = 1;
        if (NULL != y)
            *y = 0;
        return;
    }

    if (a.bit_at(0) == 0 && b.bit_at(0) == 0)
    {
        extended_euclid(a >> 1, b >> 1, d, x, y);
        // d = dd * 2;
        // x = xx; y = yy;
        if (NULL != d)
            *d <<= 1;
        return;
    }
    else if (a.bit_at(0) == 0)
    {
        BigInteger xx;
        extended_euclid(b, a >> 1, d, y, &xx);
        if (xx.bit_at(0) == 0)
        {
            // d = dd;
            // x = xx / 2; y = yy;
            if (NULL != x)
                *x = xx >> 1;
        }
        else
        {
            // d = dd;
            // x = (xx + b) / 2; y = yy - a / 2;
            if (NULL != x)
                *x = (xx + b) >> 1;
            if (NULL != y)
                *y -= a >> 1;
        }
        return;
    }
    else if (b.bit_at(0) == 0)
    {
        BigInteger yy;
        extended_euclid(a, b >> 1, d, x, &yy);
        if (yy.bit_at(0) == 0)
        {
            // d = dd;
            // x = xx; y = yy / 2;
            if (NULL != y)
                *y = yy >> 1;
        }
        else
        {
            // d = dd;
            // x = xx - b / 2; y = (yy + a) / 2;
            if (NULL != x)
                *x -= b >> 1;
            if (NULL != y)
                *y = (yy + a) >> 1;
        }
        return;
    }
    else if (a > b)
    {
        BigInteger xx;
        extended_euclid(b, (a - b) >> 1, d, y, &xx);
        if (xx.bit_at(0) == 0)
        {
            // d = dd;
            // x = xx / 2; y = yy - xx / 2;
            if (NULL != x)
                *x = xx >> 1;
            if (NULL != y)
                *y -= xx >> 1;
        }
        else
        {
            // d = dd;
            // x = (xx + b) / 2; y = yy - (xx + a) / 2;
            // 或者 x = (xx - b) / 2; y = yy - (xx - a) / 2;
            if (NULL != x)
                *x = (xx + b) >> 1;
            if (NULL != y)
                *y -= (xx + a) >> 1;
        }
        return;
    }
    else
    {
        BigInteger yy;
        extended_euclid(a, (b - a) >> 1, d, x, &yy);
        if (yy.bit_at(0) == 0)
        {
            // d = dd;
            // x = xx - yy / 2; y = yy / 2;
            if (NULL != x)
                *x -= yy >> 1;
            if (NULL != y)
                *y = yy >> 1;
        }
        else
        {
            // d = dd;
            // x = xx - (yy + b) / 2; y = (yy + a) / 2;
            // 或者 x == xx - (yy - b) / 2; y = (yy - a) / 2;
            if (NULL != x)
                *x -= (yy + b) >> 1;
            if (NULL != y)
                *y = (yy + a) >> 1;
        }
        return;
    }
#endif
}

/**
 * 费马小定理素数测试法, 伪素数测试
 * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P582
 */
inline bool psedoprime(const BigInteger& n)
{
    if (modular_exponentiation(BigInteger(2), n - 1, n) != 1)
        return false; // 一定是合数
    return true; // 可能是素数
}

inline bool _miller_rabin_witness(const BigInteger& a, const BigInteger& n)
{
    BigInteger d(1), b(n - 1);
    for (register size_t i = b.bit_length(); i > 0; --i)
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
    for (register size_t i = 0; i < s; ++i)
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
    const int a = lowest_bit(m.bytes(), m.significant_length());
    m >>= a;

    // Do the tests
    for (register size_t i = 0; i < s; ++i)
    {
        // Generate a uniform random in [1, n)
        const BigInteger b(2);//  = BigInteger::rand_between(ONE, n); // _rand_1_n(n);

        int j = 0;
        BigInteger z = modular_exponentiation(b, m, n);
        while(!((j == 0 && z == ONE) || z == thisMinusOne))
        {
            if ((j > 0 && z == ONE) || ++j == a)
                return false;
            z = modular_exponentiation(z, TWO, n);
        }
    }
    return true;
#endif
}

/**
 * 取下一个可能的素数
 * 参见java语言BigInteger.nextProbablePrime()实现
 */
inline BigInteger nextProbablePrime(const BigInteger& n)
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


