
#ifndef ___HEADFILE_1F13D620_9C6B_474E_AACF_C0359DD7F80D_
#define ___HEADFILE_1F13D620_9C6B_474E_AACF_C0359DD7F80D_

#include <stdlib.h>

#include <nut/debugging/destroy_checker.h>
#include <nut/mem/memory_allocator.h>

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
template <typename T, typename Counter>
class RCWrapper final : public T
{
protected:
    // 引用计数器
    Counter m_ref_count;

    // 避免多次销毁的检查器
    NUT_DEBUGGING_DESTROY_CHECKER

public:
#ifndef _LIBCPP_HAS_NO_VARIADICS
    template <typename ...Args>
    RCWrapper(Args&& ...args)
        : T(args...), m_ref_count(0)
    {}
#else
    RCWrapper()
        : T(), m_ref_count(0)
    {}

    template <typename Arg1>
    RCWrapper(Arg1&& arg1)
        : T(arg1), m_ref_count(0)
    {}

    template <typename Arg1, typename Arg2>
    RCWrapper(Arg1&& arg1, Arg2&& arg2)
        : T(arg1, arg2), m_ref_count(0)
    {}

    template <typename Arg1, typename Arg2, typename Arg3>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
        : T(arg1, arg2, arg3), m_ref_count(0)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
        : T(arg1, arg2, arg3, arg4), m_ref_count(0)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
        : T(arg1, arg2, arg3, arg4, arg5), m_ref_count(0)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6)
        : T(arg1, arg2, arg3, arg4, arg5, arg6), m_ref_count(0)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7)
        : T(arg1, arg2, arg3, arg4, arg5, arg6, arg7), m_ref_count(0)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
        : T(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8), m_ref_count(0)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
        : T(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9),
          m_ref_count(0)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9, Arg10&& arg10)
        : T(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10),
          m_ref_count(0)
    {}
#endif

    virtual int add_ref() override final
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return ++m_ref_count;
    }

    virtual int release_ref() override final
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        const int ret = --m_ref_count;
        if (0 == ret)
        {
            this->~RCWrapper();
            ::free(this);
        }
        return ret;
    }

    virtual int get_ref() const override final
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return m_ref_count;
    }
};

template <typename T, typename Counter>
class RCAllocWrapper final : public T
{
protected:
    // 引用计数器
    Counter m_ref_count;

    // 内存分配器
    const rc_ptr<memory_allocator> m_alloc;

    // 避免多次销毁的检查器
    NUT_DEBUGGING_DESTROY_CHECKER

public:
#ifndef _LIBCPP_HAS_NO_VARIADICS
    template <typename ...Args>
    RCAllocWrapper(memory_allocator *ma, Args&& ...args)
        : T(args...), m_ref_count(0), m_alloc(ma)
    {}
#else
    RCAllocWrapper(memory_allocator *ma)
        : T(), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1)
        : T(arg1), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2)
        : T(arg1, arg2), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
        : T(arg1, arg2, arg3), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
        : T(arg1, arg2, arg3, arg4), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
        : T(arg1, arg2, arg3, arg4, arg5), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6)
        : T(arg1, arg2, arg3, arg4, arg5, arg6), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7)
        : T(arg1, arg2, arg3, arg4, arg5, arg6, arg7), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
        : T(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
        : T(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9),
          m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9, Arg10&& arg10)
        : T(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10),
          m_ref_count(0), m_alloc(ma)
    {}
#endif

    virtual int add_ref() override final
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return ++m_ref_count;
    }

    virtual int release_ref() override final
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        const int ret = --m_ref_count;
        if (0 == ret)
        {
            rc_ptr<memory_allocator> alloc = m_alloc;
            this->~RCAllocWrapper();
            if (alloc.is_not_null())
                alloc->free(this);
            else
                ::free(this);
        }
        return ret;
    }

    virtual int get_ref() const override final
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return m_ref_count;
    }
};

}

// 即使用户类的构造函数是private的，也有办法正常工作
#define NUT_PRIVATE_RCNEW template <typename ___T, typename ___C> friend class nut::RCWrapper;

#endif
