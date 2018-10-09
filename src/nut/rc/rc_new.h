
#ifndef ___HEADFILE___B77B6DC2_6C30_4732_943B_117AF7E35B83_
#define ___HEADFILE___B77B6DC2_6C30_4732_943B_117AF7E35B83_

#include <assert.h>
#include <stdlib.h>
#include <new>
#include <utility> // for std::move() std::forward()

#include "rc_ptr.h"
#include "rc_wrapper.h"
#include "ref_counter.h"


namespace nut
{

// _MSC_VER == 1700 for Visual Studio 2012
#if (defined(_MSC_VER) && _MSC_VER > 1700) || (!defined(_MSC_VER) && !defined(_LIBCPP_HAS_NO_VARIADICS))
template <typename T, typename ...Args>
rc_ptr<T> rc_new(Args&& ...args)
{
    RCWrapper<T> *p = (RCWrapper<T>*) ::malloc(sizeof(RCWrapper<T>));
    assert(nullptr != p);
    new (p) RCWrapper<T>(std::forward<Args>(args)...);
    return rc_ptr<T>(p);
}
#else
template <typename T>
rc_ptr<T> rc_new()
{
    RCWrapper<T> *p = (RCWrapper<T>*) ::malloc(sizeof(RCWrapper<T>));
    assert(nullptr != p);
    new (p) RCWrapper<T>();
    return rc_ptr<T>(p);
}

template <typename T, typename Arg1>
rc_ptr<T> rc_new(Arg1&& arg1)
{
    RCWrapper<T> *p = (RCWrapper<T>*) ::malloc(sizeof(RCWrapper<T>));
    assert(nullptr != p);
    new (p) RCWrapper<T>(std::forward<Arg1>(arg1));
    return rc_ptr<T>(p);
}

template <typename T, typename Arg1, typename Arg2>
rc_ptr<T> rc_new(Arg1&& arg1, Arg2&& arg2)
{
    RCWrapper<T> *p = (RCWrapper<T>*) ::malloc(sizeof(RCWrapper<T>));
    assert(nullptr != p);
    new (p) RCWrapper<T>(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2));
    return rc_ptr<T>(p);
}

template <typename T, typename Arg1, typename Arg2, typename Arg3>
rc_ptr<T> rc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
{
    RCWrapper<T> *p = (RCWrapper<T>*) ::malloc(sizeof(RCWrapper<T>));
    assert(nullptr != p);
    new (p) RCWrapper<T>(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2),
                         std::forward<Arg3>(arg3));
    return rc_ptr<T>(p);
}

template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
rc_ptr<T> rc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
{
    RCWrapper<T> *p = (RCWrapper<T>*) ::malloc(sizeof(RCWrapper<T>));
    assert(nullptr != p);
    new (p) RCWrapper<T>(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2),
                         std::forward<Arg3>(arg3), std::forward<Arg4>(arg4));
    return rc_ptr<T>(p);
}

template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4,
          typename Arg5>
rc_ptr<T> rc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
{
    RCWrapper<T> *p = (RCWrapper<T>*) ::malloc(sizeof(RCWrapper<T>));
    assert(nullptr != p);
    new (p) RCWrapper<T>(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2),
                         std::forward<Arg3>(arg3), std::forward<Arg4>(arg4),
                         std::forward<Arg5>(arg5));
    return rc_ptr<T>(p);
}

template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4,
          typename Arg5, typename Arg6>
rc_ptr<T> rc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
                 Arg6&& arg6)
{
    RCWrapper<T> *p = (RCWrapper<T>*) ::malloc(sizeof(RCWrapper<T>));
    assert(nullptr != p);
    new (p) RCWrapper<T>(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2),
                         std::forward<Arg3>(arg3), std::forward<Arg4>(arg4),
                         std::forward<Arg5>(arg5), std::forward<Arg6>(arg6));
    return rc_ptr<T>(p);
}

template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4,
          typename Arg5, typename Arg6, typename Arg7>
rc_ptr<T> rc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
                 Arg6&& arg6, Arg7&& arg7)
{
    RCWrapper<T> *p = (RCWrapper<T>*) ::malloc(sizeof(RCWrapper<T>));
    assert(nullptr != p);
    new (p) RCWrapper<T>(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2),
                         std::forward<Arg3>(arg3), std::forward<Arg4>(arg4),
                         std::forward<Arg5>(arg5), std::forward<Arg6>(arg6),
                         std::forward<Arg7>(arg7));
    return rc_ptr<T>(p);
}

template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4,
          typename Arg5, typename Arg6, typename Arg7, typename Arg8>
rc_ptr<T> rc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
                 Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
{
    RCWrapper<T> *p = (RCWrapper<T>*) ::malloc(sizeof(RCWrapper<T>));
    assert(nullptr != p);
    new (p) RCWrapper<T>(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2),
                         std::forward<Arg3>(arg3), std::forward<Arg4>(arg4),
                         std::forward<Arg5>(arg5), std::forward<Arg6>(arg6),
                         std::forward<Arg7>(arg7), std::forward<Arg8>(arg8));
    return rc_ptr<T>(p);
}

template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4,
          typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
rc_ptr<T> rc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
                 Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
{
    RCWrapper<T> *p = (RCWrapper<T>*) ::malloc(sizeof(RCWrapper<T>));
    assert(nullptr != p);
    new (p) RCWrapper<T>(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2),
                         std::forward<Arg3>(arg3), std::forward<Arg4>(arg4),
                         std::forward<Arg5>(arg5), std::forward<Arg6>(arg6),
                         std::forward<Arg7>(arg7), std::forward<Arg8>(arg8),
                         std::forward<Arg9>(arg9));
    return rc_ptr<T>(p);
}

template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4,
          typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9,
          typename Arg10>
rc_ptr<T> rc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
                 Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9, Arg10&& arg10)
{
    RCWrapper<T> *p = (RCWrapper<T>*) ::malloc(sizeof(RCWrapper<T>));
    assert(nullptr != p);
    new (p) RCWrapper<T>(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2),
                         std::forward<Arg3>(arg3), std::forward<Arg4>(arg4),
                         std::forward<Arg5>(arg5), std::forward<Arg6>(arg6),
                         std::forward<Arg7>(arg7), std::forward<Arg8>(arg8),
                         std::forward<Arg9>(arg9), std::forward<Arg10>(arg10));
    return rc_ptr<T>(p);
}
#endif

}

#endif /* head file guarder */
