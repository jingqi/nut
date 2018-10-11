
#ifndef ___HEADFILE_E134CCCD_BA57_49BB_A1D7_8BA2C91957FA_
#define ___HEADFILE_E134CCCD_BA57_49BB_A1D7_8BA2C91957FA_

#include <utility>

namespace nut
{

template <typename T1, typename T2, typename T3 = void, typename T4 = void,
          typename T5 = void, typename T6 = void>
class Tuple
{
public:
    Tuple() = default;

    Tuple(T1&& t1, T2&& t2, T3&& t3, T4&& t4, T5&& t5,
          T6&& t6)
        : first(std::forward<T1>(t1)), second(std::forward<T2>(t2)),
          third(std::forward<T3>(t3)), forth(std::forward<T4>(t4)),
          fifth(std::forward<T5>(t5)), sixth(std::forward<T6>(t6))
    {}

public:
    T1 first;
    T2 second;
    T3 third;
    T4 forth;
    T5 fifth;
    T6 sixth;
};

template <typename T1, typename T2, typename T3, typename T4, typename T5>
class Tuple<T1, T2, T3, T4, T5, void>
{
public:
    Tuple() = default;

    Tuple(T1&& t1, T2&& t2, T3&& t3, T4&& t4, T5&& t5)
        : first(std::forward<T1>(t1)), second(std::forward<T2>(t2)),
          third(std::forward<T3>(t3)), forth(std::forward<T4>(t4)),
          fifth(std::forward<T5>(t5))
    {}

public:
    T1 first;
    T2 second;
    T3 third;
    T4 forth;
    T5 fifth;
};

template <typename T1, typename T2, typename T3, typename T4>
class Tuple<T1, T2, T3, T4, void, void>
{
public:
    Tuple() = default;

    Tuple(T1&& t1, T2&& t2, T3&& t3, T4&& t4)
        : first(std::forward<T1>(t1)), second(std::forward<T2>(t2)),
          third(std::forward<T3>(t3)), forth(std::forward<T4>(t4))
    {}

public:
    T1 first;
    T2 second;
    T3 third;
    T4 forth;
};

template <typename T1, typename T2, typename T3>
class Tuple<T1, T2, T3, void, void, void>
{
public:
    Tuple() = default;

    Tuple(T1&& t1, T2&& t2, T3&& t3)
        : first(std::forward<T1>(t1)), second(std::forward<T2>(t2)),
          third(std::forward<T3>(t3))
    {}

public:
    T1 first;
    T2 second;
    T3 third;
};

template <typename T1, typename T2>
class Tuple<T1, T2, void, void, void, void>
{
public:
    Tuple() = default;

    Tuple(T1&& t1, T2&& t2)
        : first(std::forward<T1>(t1)), second(std::forward<T2>(t2))
    {}

public:
    T1 first;
    T2 second;
};

}

#endif /* head file guarder */
