
#ifndef ___HEADFILE_1F13D620_9C6B_474E_AACF_C0359DD7F80D_
#define ___HEADFILE_1F13D620_9C6B_474E_AACF_C0359DD7F80D_

#include <stdlib.h>
#include <utility> // for std::forward()

#include "../debugging/destroy_checker.h"
#include "ref_counter.h"


namespace nut
{

/**
 * 可引用计数对象包装器
 *
 * @note
 * 使用包装器来包装的好处：
 * 1. 即使用户类的构造函数是private的，也有办法正常工作
 * 2. 如果对象是多继承的，计数器仍然只有一份，仍然能正常运行
 */
template <typename T>
class RCWrapper final : public T
{
public:
    template <typename ...Args>
    RCWrapper(Args&& ...args) noexcept
        : T(std::forward<Args>(args)...)
    {}

    virtual int add_ref() const noexcept override final
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return ++_ref_count;
    }

    virtual int release_ref() const noexcept override final
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        const int ret = --_ref_count;
        if (0 == ret)
        {
            this->~RCWrapper();
            ::free(const_cast<RCWrapper<T>*>(this));
        }
        return ret;
    }

    virtual int get_ref() const noexcept override final
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return _ref_count;
    }

protected:
    // 引用计数器
    mutable RefCounter _ref_count;

    // 避免多次销毁的检查器
    NUT_DEBUGGING_DESTROY_CHECKER
};

}

// 保证即使用户类的构造函数是 private 的，也能从 RCWrapper 中调用其构造函数
#define NUT_PRIVATE_RCNEW template <typename ___T> friend class nut::RCWrapper;

#endif
