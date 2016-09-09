
#ifndef ___HEADFILE_6460A1FB_BAA9_4633_A442_22707D79969B_
#define ___HEADFILE_6460A1FB_BAA9_4633_A442_22707D79969B_

#include <nut/nut_config.h>

#include "../big_integer.h"

namespace nut
{

/**
 * 求最大公约数
 */
NUT_API BigInteger gcd(const BigInteger& a, const BigInteger& b);

/**
 * 扩展欧几里得算法
 */
NUT_API void extended_euclid(const BigInteger& a, const BigInteger& b, BigInteger *d, BigInteger *x, BigInteger *y);

}

#endif
