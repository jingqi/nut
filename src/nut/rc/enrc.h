
#ifndef ___HEADFILE_0BA3A842_E42B_4C60_A89E_86200AFC1B81_
#define ___HEADFILE_0BA3A842_E42B_4C60_A89E_86200AFC1B81_

#include <utility> // for std::forward()

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
public: // NOTE 暴露给外部，使其能手动操作引用计数，比如 Copy-On-Write 算法
    NUT_REF_COUNTABLE

    template <typename ...Args>
    enrc(Args&& ...args) noexcept
        : T(std::forward<Args>(args)...)
    {}
};

}

#endif /* head file guarder */
