
#ifndef ___HEADFILE_0BA3A842_E42B_4C60_A89E_86200AFC1B81_
#define ___HEADFILE_0BA3A842_E42B_4C60_A89E_86200AFC1B81_

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
    NUT_REF_COUNTABLE

public :
#ifndef _LIBCPP_HAS_NO_VARIADICS
    template <typename ...Args>
    enrc(Args&& ...args)
        : T(args...)
    {}
#else
    enrc()
    {}

    template <typename Arg1>
    enrc(Arg1&& arg1)
        : T(arg1)
    {}

    template <typename Arg1, typename Arg2>
    enrc(Arg1&& arg1, Arg2&& arg2)
        : T(arg1, arg2)
    {}

    template <typename Arg1, typename Arg2, typename Arg3>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
        : T(arg1, arg2, arg3)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
        : T(arg1, arg2, arg3, arg4)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
        : T(arg1, arg2, arg3, arg4, arg5)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
         Arg6&& arg6)
        : T(arg1, arg2, arg3, arg4, arg5, arg6)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
         Arg6&& arg6, Arg7&& arg7)
        : T(arg1, arg2, arg3, arg4, arg5, arg6, arg7)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
         Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
        : T(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
         Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
        : T(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
    enrc(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
         Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9, Arg10&& arg10)
        : T(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10)
    {}
#endif
};

}

#endif /* head file guarder */
