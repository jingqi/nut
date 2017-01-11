
#ifndef ___HEADFILE_0BA3A842_E42B_4C60_A89E_86200AFC1B81_
#define ___HEADFILE_0BA3A842_E42B_4C60_A89E_86200AFC1B81_

#include <utility> // for std::move() std::forward()

#include "rc_ptr.h"

namespace nut
{

/**
 * enable rc_ptr count
 *
 * example:
 *    rc_ptr<enrc<std::string> > v = rc_new<enref<std::string> >("abc");
 */
template <typename T>
class enrc : public T
{
public:
    NUT_REF_COUNTABLE

#ifndef _LIBCPP_HAS_NO_VARIADICS
    template <typename ...Args>
    enrc(Args&& ...args)
        : T(std::forward<Args>(args)...)
    {}
#else
    enrc() = default;

    template <typename Arg1>
    enrc(Arg1&& arg1)
        : T(std::forward<Args>(arg1))
    {}

    template <typename Arg1, typename Arg2>
    enrc(Arg1&& arg1, Arg2&& arg2)
        : T(std::forward<Args>(arg1), std::forward<Arg2>(arg2))
    {}

    template <typename Arg1, typename Arg2, typename Arg3>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
        : T(std::forward<Args>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
        : T(std::forward<Args>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
        : T(std::forward<Args>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
         Arg6&& arg6)
        : T(std::forward<Args>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
            std::forward<Arg6>(arg6))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
         Arg6&& arg6, Arg7&& arg7)
        : T(std::forward<Args>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
            std::forward<Arg6>(arg6), std::forward<Arg7>(arg7))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
         Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
        : T(std::forward<Args>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
            std::forward<Arg6>(arg6), std::forward<Arg7>(arg7), std::forward<Arg8>(arg8))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
         Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
        : T(std::forward<Args>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
            std::forward<Arg6>(arg6), std::forward<Arg7>(arg7), std::forward<Arg8>(arg8), std::forward<Arg9>(arg9))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
         Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9, Arg10&& arg10)
        : T(std::forward<Args>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
            std::forward<Arg6>(arg6), std::forward<Arg7>(arg7), std::forward<Arg8>(arg8), std::forward<Arg9>(arg9), std::forward<Arg10>(arg10))
    {}
#endif
};

}

#endif /* head file guarder */
