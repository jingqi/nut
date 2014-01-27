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

template <typename T1, typename T2, typename T3 = void, typename T4 = void, typename T5 = void,
          typename T6 = void>
struct Tuple
{
    T1 first;
    T2 second;
    T3 third;
    T4 forth;
    T5 fifth;
    T6 sixth;

    Tuple() {}

    Tuple(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5,
          const T6& t6)
        : first(t1), second(t2), third(t3), forth(t4), fifth(t5),
          sixth(t6)
    {}
};

template <typename T1, typename T2, typename T3, typename T4, typename T5>
struct Tuple<T1, T2, T3, T4, T5, void>
{
    T1 first;
    T2 second;
    T3 third;
    T4 forth;
    T5 fifth;

    Tuple() {}

    Tuple(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
        : first(t1), second(t2), third(t3), forth(t4), fifth(t5)
    {}
};

template <typename T1, typename T2, typename T3, typename T4>
struct Tuple<T1, T2, T3, T4, void, void>
{
    T1 first;
    T2 second;
    T3 third;
    T4 forth;

    Tuple() {}

    Tuple(const T1& t1, const T2& t2, const T3& t3, const T4& t4)
        : first(t1), second(t2), third(t3), forth(t4)
    {}
};

template <typename T1, typename T2, typename T3>
struct Tuple<T1, T2, T3, void, void, void>
{
    T1 first;
    T2 second;
    T3 third;

    Tuple() {}

    Tuple(const T1& t1, const T2& t2, const T3& t3)
        : first(t1), second(t2), third(t3)
    {}
};

template <typename T1, typename T2>
struct Tuple<T1, T2, void, void, void, void>
{
    T1 first;
    T2 second;

    Tuple() {}

    Tuple(const T1& t1, const T2& t2)
        : first(t1), second(t2)
    {}
};

}

#endif /* head file guarder */
