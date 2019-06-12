
#ifndef ___HEADFILE___B77B6DC2_6C30_4732_943B_117AF7E35B83_
#define ___HEADFILE___B77B6DC2_6C30_4732_943B_117AF7E35B83_

#include <assert.h>
#include <stdlib.h>
#include <new>
#include <utility> // for std::forward()

#include "rc_ptr.h"
#include "rc_wrapper.h"
#include "ref_counter.h"


namespace nut
{

template <typename T, typename ...Args>
rc_ptr<T> rc_new(Args&& ...args) noexcept
{
    RCWrapper<T> *p = (RCWrapper<T>*) ::malloc(sizeof(RCWrapper<T>));
    assert(nullptr != p);
    new (p) RCWrapper<T>(std::forward<Args>(args)...);
    return rc_ptr<T>(p);
}

}

#endif /* head file guarder */
