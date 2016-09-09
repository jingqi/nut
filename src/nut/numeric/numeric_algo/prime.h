
#ifndef ___HEADFILE_526CD359_EF76_437B_ACDD_D24322AB6FDC_
#define ___HEADFILE_526CD359_EF76_437B_ACDD_D24322AB6FDC_

#include <nut/nut_config.h>

#include "../big_integer.h"
#include "bit_sieve.h"
#include "mod.h"

// 优化程度，>= 0
#define OPTIMIZE_LEVEL 1000

namespace nut
{

/**
 * 费马小定理素数测试法, 伪素数测试
 * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P582
 */
bool psedoprime(const BigInteger& n);

/**
 * 米勒-拉宾(Miller-Rabin)素数测试
 */
bool miller_rabin(const BigInteger& n, unsigned s);

/**
 * 取下一个可能的素数
 * 参见java语言BigInteger.nextProbablePrime()实现
 */
NUT_API BigInteger next_prime(const BigInteger& n);

}

#undef OPTIMIZE_LEVEL

#endif
