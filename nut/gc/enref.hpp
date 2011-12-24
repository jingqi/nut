/**
 * @file -
 * @author jingqi
 * @date 2010-11-10
 */

#ifndef ___HEADFILE_0BA3A842_E42B_4C60_A89E_86200AFC1B81_
#define ___HEADFILE_0BA3A842_E42B_4C60_A89E_86200AFC1B81_

#include "ref.hpp"

namespace nut
{

/**
 * to make an unrefable class refable
 * example:
 *    ref<enref<std::string> > v = gc_new<enref<std::string> >("abc");
 */
template <typename T>
class enref : public T
{
    NUT_GC_REFERABLE

public :
    enref()
    {}

    template <typename Arg1>
    enref(Arg1 arg1)
        : T(RefargTraits<Arg1>::value(arg1))
    {}

    template <typename Arg1, typename Arg2>
    enref(Arg1 arg1, Arg2 arg2)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2))
    {}

    template <typename Arg1, typename Arg2, typename Arg3>
    enref(Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    enref(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3), 
        RefargTraits<Arg4>::value(arg4))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    enref(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3), 
        RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5))
    {}
};

}

#endif /* head file guarder */

