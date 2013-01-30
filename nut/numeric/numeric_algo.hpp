/**
 * @file -
 * @author jingqi
 * @date 2012-11-25
 * @last-edit 2013-01-25 15:42:04 jingqi
 */

#ifndef ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_
#define ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_

#include "unsignedinteger.hpp"
#include "bitsieve.hpp"

namespace nut
{

/**
 * 求(a**b)%n，即a的b次方(模n)
 */
template <size_t N>
inline UnsignedInteger<N> modular_exponentiation(const UnsignedInteger<N>& _a, const UnsignedInteger<N>& b, const UnsignedInteger<N>& _n)
{
    const UnsignedInteger<N * 2> a(_a), n(_n);
    UnsignedInteger<N * 2> ret(1);
    for (register size_t i = N * 8; i > 0; --i) // 从高位向低有效位取bit
    {
        ret = (ret * ret) % n;
        if (0 != b.bit_at(i - 1))
            ret = (ret * a) % n;
    }
    return ret;
}

/**
 * 欧几里德(EUCLID)算法，求最大公约数
 */
template <size_t N>
UnsignedInteger<N> euclid_gcd(const UnsignedInteger<N>& a, const UnsignedInteger<N>& b)
{
    if (b.is_zero())
        return a;
    return euclid_gcd(b, a % b);
}

/**
 * 欧几里得(EUCLID)算法的推广形式
 * d = gcd(a, b) = ax + by
 */
template <size_t N>
void extended_euclid(const UnsignedInteger<N>& a, const UnsignedInteger<N>& b,
    UnsignedInteger<N> *d, UnsignedInteger<N> *x, UnsignedInteger<N> *y)
{
    if (b.is_zero())
    {
        if (NULL != d)
            *d = a;
        if (NULL != x)
            *x = UnsignedInteger<N>(1);
        if (NULL != y)
            *y = UnsignedInteger<N>(0);
    }
    
    UnsignedInteger<N> xx, yy;
    extended_euclid(b, a % b, d, &xx, &yy);
    if (NULL != x)
        *x = yy;
    if (NULL != y)
        *y = xx - a / b * yy;
}

/**
 * 伪素数测试法
 */
template <size_t N>
bool psedoprime(const UnsignedInteger<N>& n)
{
    const UnsignedInteger<N> ONE(1);
    if (ONE != modular_exponentiation(2, n - 1, n))
        return false; // 一定是合数
    return true; // 可能是素数
}

template <size_t N>
bool _miller_rabin_witness(const UnsignedInteger<N>& a, const UnsignedInteger<N>& n)
{
    UnsignedInteger<N> d(1), b(n - UnsignedInteger<N>(1));
    for (register size_t i = N * 8; i > 0; --i)
    {
        UnsignedInteger<N> x = d;
        d = (d * d) % n;
        if (d == UnsignedInteger<N>(1) && x != UnsignedInteger<N>(1) && x != b)
            return true;
        if (0 != b.bit_at(i - 1))
            d = (d * a) % n;
    }
    if (d != UnsignedInteger<N>(1))
        return true;
    return false;
}

/**
 * Miller-Rabin 素数测试
 */
template <size_t N>
bool miller_rabin(const UnsignedInteger<N>& n, unsigned s)
{
    for (size_t i = 1; i < s; ++i)
    {
        UnsignedInteger<N> a;
        for (register size_t i = 0; i < N; ++i)
            a[i] = rand();
        a = a % (n - UnsignedInteger<N>(2));
        ++a; // a = random(1, n -1);

        if (_miller_rabin_witness(a, n))
            return false; // 一定是合数
    }
    return true; // 几乎肯定是素数
}

/**
 * 取下一个可能的素数
 */
template <size_t N>
UnsignedInteger<N> nextProbablePrime(const UnsignedInteger<N>& n)
{
    if (n <= UnsignedInteger<N>(1))
        return UnsignedInteger<N>(2);

    const int SMALL_PRIME_THRESHOLD = 95;
    const int DEFAULT_PRIME_CERTAINTY = 2;
    const UnsignedInteger<N> SMALL_PRIME_PRODUCT(3L * 5 * 7 * 11 * 13 * 17 * 19 * 23 * 29 * 31 * 37 * 41);

    UnsignedInteger<N> result = n + UnsignedInteger<N>(1);

    // Fastpath for small numbers
    if (bit_length(result.buffer(), N) < SMALL_PRIME_THRESHOLD)
    {

        // Ensure an odd number
        if (result.bit_at(0) == 0)
            ++result;

        while(true)
        {
            // Do cheap "pre-test" if applicable
            if (bit_length(result.buffer(), N) > 6) {
                long r = (result % SMALL_PRIME_PRODUCT).ulong_value();
                if ((r%3==0)  || (r%5==0)  || (r%7==0)  || (r%11==0) || 
                    (r%13==0) || (r%17==0) || (r%19==0) || (r%23==0) || 
                    (r%29==0) || (r%31==0) || (r%37==0) || (r%41==0)) {
                        result += UnsignedInteger<N>(2);
                        continue; // Candidate is composite; try another
                }
            }

            // All candidates of bitLength 2 and 3 are prime by this point
            if (bit_length(result.buffer(), N) < 4)
                return result;

            // The expensive test
            if (miller_rabin(result, DEFAULT_PRIME_CERTAINTY))
                return result;

            result += UnsignedInteger<N>(2);
        }
    }

    // Start at previous even number
    if (result.bit_at(0) == 1)
        --result;

    // Looking for the next large prime
    int searchLen = (bit_length(result.buffer(), N) / 20) * 64;

    while(true) {
        BitSieve searchSieve(result, searchLen);
        UnsignedInteger<N> candidate = searchSieve.retrieve(result,
            DEFAULT_PRIME_CERTAINTY);
        if (!candidate.is_zero())
            return candidate;
        result += UnsignedInteger<N>(2 * searchLen);
    }
}

}

#endif


