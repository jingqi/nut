/**
 * @file -
 * @author jingqi
 * @date 2011-12-17
 * @last-edit 2011-12-17 17:23:51 jingqi
 */

#ifndef ___HEADFILE_E134CCCD_BA57_49BB_A1D7_8BA2C91957FA_
#define ___HEADFILE_E134CCCD_BA57_49BB_A1D7_8BA2C91957FA_

namespace nut
{

template <typename T1, typename T2, typename T3 = void, typename T4 = void, typename T5 = void>
struct Tuple
{
    T1 first;
    T2 second;
    T3 third;
    T4 forth;
    T5 fifth;
};

template <typename T1, typename T2, typename T3, typename T4>
struct Tuple<T1, T2, T3, T4, void>
{
    T1 first;
    T2 second;
    T3 third;
    T4 forth;
};

template <typename T1, typename T2, typename T3>
struct Tuple<T1, T2, T3, void, void>
{
    T1 first;
    T2 second;
    T3 third;
};

template <typename T1, typename T2>
struct Tuple<T1, T2, void, void, void>
{
    T1 first;
    T2 second;
};

}

#endif /* head file guarder */
