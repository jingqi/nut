﻿
#ifndef ___HEADFILE_6460A1FB_BAA9_4633_A442_22707D79969B_
#define ___HEADFILE_6460A1FB_BAA9_4633_A442_22707D79969B_

#include "../../nut_config.h"
#include "../../platform/int_type.h"
#include "../big_integer.h"


namespace nut
{

/**
 * 求最大公约数
 */
template <typename T>
T gcd(T a, T b)
{
    static_assert(std::is_integral<T>::value, "Unexpected integer type");
    if (0 == b)
        return a;
    return gcd(b, a % b);
}

NUT_API BigInteger gcd(const BigInteger& a, const BigInteger& b);

/**
 * 扩展欧几里得算法
 * d = gcd(a, b) = ax + by
 */
template <typename T>
T extended_euclid(T a, T b, typename StdInt<T>::signed_type *x, typename StdInt<T>::signed_type *y)
{
    static_assert(std::is_integral<T>::value, "Unexpected integer type");

    if (0 == b)
    {
        if (nullptr != x)
            *x = 1;
        if (nullptr != y)
            *y = 0;
        return a;
    }

    typename StdInt<T>::signed_type xx, yy;
    const T gcd = extended_euclid(b, a % b, &yy, &xx);
    if (nullptr != y)
        *y = yy - a / b * xx;
    if (nullptr != x)
        *x = xx;
    return gcd;
}

NUT_API void extended_euclid(const BigInteger& a, const BigInteger& b, BigInteger *d, BigInteger *x, BigInteger *y);

}

#endif
