/**
 * @file -
 * @author jingqi
 * @date 2010-11-10
 */

#ifndef ___HEADFILE_0BA3A842_E42B_4C60_A89E_86200AFC1B81_
#define ___HEADFILE_0BA3A842_E42B_4C60_A89E_86200AFC1B81_

#include "rc_ptr.hpp"

namespace nut
{

/**
 * enable rc_ptr count
 *
 * example:
 *    rc_ptr<enrc<std::string> > v = RC_NEW(NULL, enrc<std::string>, "abc");
 */
template <typename T>
class enrc : public T
{
    NUT_REF_COUNTABLE

public :
    enrc()
    {}

    template <typename Arg1>
    enrc(Arg1 arg1)
        : T(RefargTraits<Arg1>::value(arg1))
    {}

    template <typename Arg1, typename Arg2>
    enrc(Arg1 arg1, Arg2 arg2)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2))
    {}

    template <typename Arg1, typename Arg2, typename Arg3>
    enrc(Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    enrc(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
            RefargTraits<Arg4>::value(arg4))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    enrc(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
            RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6>
    enrc(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
            RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7>
    enrc(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
            RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6),
            RefargTraits<Arg7>::value(arg7))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8>
    enrc(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
            RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6),
            RefargTraits<Arg7>::value(arg7), RefargTraits<Arg8>::value(arg8))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    enrc(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
            RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6),
            RefargTraits<Arg7>::value(arg7), RefargTraits<Arg8>::value(arg8), RefargTraits<Arg9>::value(arg9))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
    enrc(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9, Arg10 arg10)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
            RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6),
            RefargTraits<Arg7>::value(arg7), RefargTraits<Arg8>::value(arg8), RefargTraits<Arg9>::value(arg9),
            RefargTraits<Arg10>::value(arg10))
    {}
};

}

#endif /* head file guarder */
