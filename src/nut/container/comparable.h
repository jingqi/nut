﻿
#ifndef ___HEADFILE_49B551AE_4D34_4B5D_A641_3002BDED3655_
#define ___HEADFILE_49B551AE_4D34_4B5D_A641_3002BDED3655_

#include <string>


namespace nut
{

/**
 * check if type 'T' has member function 'int T::compare(const T&) const'
 */
template <typename T>
class has_compare_method
{
    template<typename U, int (U::*)(const U&) const>
    struct SFINAE
    {};

    template<typename U>
    static char test(SFINAE<U, &U::compare>*);

    template<typename U>
    static int test(...);

public:
    static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

/**
 * Compare two operands
 *
 * @param lhs left hand side operand
 * @param rhs right hand side operand
 * @return <0 if 'lhs' less than 'rhs'
 *         0 if 'lhs' equals to 'rhs'
 *         >= if 'lhs' greater than 'rhs'
 */
template <typename T>
typename std::enable_if<!has_compare_method<T>::value,int>::type
compare(const T& lhs, const T& rhs)
{
    return lhs < rhs ? -1 : (rhs < lhs ? 1 : 0);
}

template <typename T>
typename std::enable_if<has_compare_method<T>::value,int>::type
compare(const T& lhs, const T&rhs)
{
    return lhs.compare(rhs);
}

}

#endif