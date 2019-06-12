
#ifndef ___HEADFILE_526CD359_EF76_437B_ACDD_D24322AB6FDC_
#define ___HEADFILE_526CD359_EF76_437B_ACDD_D24322AB6FDC_

#include <type_traits>

#include "../../nut_config.h"
#include "../big_integer.h"
#include "bit_sieve.h"
#include "mod.h"
#include "gcd.h"


namespace nut
{

/**
 * 费马小定理素数测试法, 伪素数测试
 *
 * 参考文献：
 *     [1]潘金贵，顾铁成. 现代计算机常用数据结构和算法[M]. 南京大学出版社. 1994. 582
 */
NUT_API bool psedoprime(const BigInteger& n) noexcept;

/**
 * 米勒-拉宾(Miller-Rabin)素数测试
 */
NUT_API bool miller_rabin(const BigInteger& n, unsigned s) noexcept;

/**
 * 取下一个可能的素数
 * 参见java语言BigInteger.nextProbablePrime()实现
 */
NUT_API BigInteger next_prime(const BigInteger& n) noexcept;

/**
 * a, n 互质，计算 a 的乘法逆元 (mod n)
 */
template <typename T>
T inverse_of_coprime_mod(T a, T n) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Unexpected integer type");
    typedef typename StdInt<T>::signed_type sword_type;

    /**
     * NOTE 小整数可以根据扩展欧几里得算法求，超大整数可以通过
     *      pow_mod(a, n - 2, n) 来求(因为 pow_mod(a, n - 1, n) == 1)
     */
    sword_type ret;
    const T gcd = extended_euclid(a, n, &ret, nullptr);
    assert(1 == gcd);
    if (ret < 0)
    {
        ret %= (sword_type) n; // NOTE 负数模需要有符号数操作
        ret += n;
    }
    else
    {
        ret %= n;
    }
    assert(mul_mod(a, (T) ret, n) == 1);
    return ret;
}

NUT_API BigInteger inverse_of_coprime_mod(const BigInteger& a, const BigInteger& n) noexcept;

}

#endif
