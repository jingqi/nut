
#ifndef ___HEADFILE_1F13D620_9C6B_474E_AACF_C0359DD7F80D_
#define ___HEADFILE_1F13D620_9C6B_474E_AACF_C0359DD7F80D_

#include <stdlib.h>
#include <utility> // for std::move() std::forward()

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
    Counter _ref_count = 0;

    // 避免多次销毁的检查器
    NUT_DEBUGGING_DESTROY_CHECKER

public:
// _MSC_VER == 1700 for Visual Studio 2012
#if (defined(_MSC_VER) && _MSC_VER > 1700) || (!defined(_MSC_VER) && !defined(_LIBCPP_HAS_NO_VARIADICS))
    template <typename ...Args>
    RCWrapper(Args&& ...args)
        : T(std::forward<Args>(args)...)
    {}
#else
    RCWrapper()
        : T()
    {}

    template <typename Arg1>
    RCWrapper(Arg1&& arg1)
        : T(std::forward<Arg1>(arg1))
    {}

    template <typename Arg1, typename Arg2>
    RCWrapper(Arg1&& arg1, Arg2&& arg2)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2))
    {}

    template <typename Arg1, typename Arg2, typename Arg3>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
            std::forward<Arg6>(arg6))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
            std::forward<Arg6>(arg6), std::forward<Arg7>(arg7))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
            std::forward<Arg6>(arg6), std::forward<Arg7>(arg7), std::forward<Arg8>(arg8))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
            std::forward<Arg6>(arg6), std::forward<Arg7>(arg7), std::forward<Arg8>(arg8), std::forward<Arg9>(arg9))
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
    RCWrapper(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9, Arg10&& arg10)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
            std::forward<Arg6>(arg6), std::forward<Arg7>(arg7), std::forward<Arg8>(arg8), std::forward<Arg9>(arg9), std::forward<Arg10>(arg10))
    {}
#endif

    virtual int add_ref() override final
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return ++_ref_count;
    }

    virtual int release_ref() override final
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        const int ret = --_ref_count;
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
        return _ref_count;
    }
};

template <typename T, typename Counter>
class RCAllocWrapper final : public T
{
protected:
    // 引用计数器
    Counter _ref_count = 0;

    // 内存分配器
    const rc_ptr<memory_allocator> _alloc;

    // 避免多次销毁的检查器
    NUT_DEBUGGING_DESTROY_CHECKER

public:
// _MSC_VER == 1700 for Visual Studio 2012
#if (defined(_MSC_VER) && _MSC_VER > 1700) || (!defined(_MSC_VER) && !defined(_LIBCPP_HAS_NO_VARIADICS))
    template <typename ...Args>
    RCAllocWrapper(memory_allocator *ma, Args&& ...args)
        : T(std::forward<Args>(args)...), _alloc(ma)
    {}
#else
    RCAllocWrapper(memory_allocator *ma)
        : T(), _m_alloc(ma)
    {}

    template <typename Arg1>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1)
        : T(std::forward<Arg1>(arg1)), _alloc(ma)
    {}

    template <typename Arg1, typename Arg2>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2)), _alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3)), _alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4)), _alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5)), _alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
            std::forward<Arg6>(arg6)), _alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
            std::forward<Arg6>(arg6), std::forward<Arg7>(arg7)), _alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
            std::forward<Arg6>(arg6), std::forward<Arg7>(arg7), std::forward<Arg8>(arg8)), _alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
            std::forward<Arg6>(arg6), std::forward<Arg7>(arg7), std::forward<Arg8>(arg8), std::forward<Arg9>(arg9)), _alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
    RCAllocWrapper(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9, Arg10&& arg10)
        : T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
            std::forward<Arg6>(arg6), std::forward<Arg7>(arg7), std::forward<Arg8>(arg8), std::forward<Arg9>(arg9), std::forward<Arg10>(arg10)),
          _alloc(ma)
    {}
#endif

    virtual int add_ref() override final
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return ++_ref_count;
    }

    virtual int release_ref() override final
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        const int ret = --_ref_count;
        if (0 == ret)
        {
            rc_ptr<memory_allocator> alloc = _alloc;
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
        return _ref_count;
    }
};

}

// 即使用户类的构造函数是private的，也有办法正常工作
#define NUT_PRIVATE_RCNEW template <typename ___T, typename ___C> friend class nut::RCWrapper;

#endif
