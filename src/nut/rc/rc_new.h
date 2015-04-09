
#ifndef ___HEADFILE___B77B6DC2_6C30_4732_943B_117AF7E35B83_
#define ___HEADFILE___B77B6DC2_6C30_4732_943B_117AF7E35B83_

#include <stdint.h>
#include <stdlib.h>
#include <new>

#include <nut/memtool/refarg.h>
#include <nut/debugging/destroy_checker.h>
#include <nut/threading/lockfree/atomic.h>
#include <nut/mem/memory_allocator.h>

#include "enrc.h"
#include "rc_ptr.h"

namespace nut
{

/**
 * 可引用计数对象包装器
 *
 * @note
 * 使用包装器来包装的好处：
 * 1. 析构函数的部分见RC_NEW的note
 * 2. 如果对象是多继承的，计数器仍然只有一份，仍然能正常运行
 */
template <typename T>
class RCWrapper : public T
{
protected:
    // 引用计数
    int volatile m_ref_count;

    // 内存分配器
    const rc_ptr<memory_allocator> m_alloc;

    // 避免多次销毁的检查器
    NUT_DEBUGGING_DESTROY_CHECKER

public:
    RCWrapper(memory_allocator *ma)
        : T(), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1>
    RCWrapper(memory_allocator *ma, Arg1 arg1)
        : T(RefargTraits<Arg1>::value(arg1)), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2)), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3)),
          m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
            RefargTraits<Arg4>::value(arg4)), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
            RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5)), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
              Arg6 arg6)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
            RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6)),
          m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
              Arg6 arg6, Arg7 arg7)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
            RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6),
            RefargTraits<Arg7>::value(arg7)), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
              Arg6 arg6, Arg7 arg7, Arg8 arg8)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
            RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6),
            RefargTraits<Arg7>::value(arg7), RefargTraits<Arg8>::value(arg8)), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
              Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
            RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6),
            RefargTraits<Arg7>::value(arg7), RefargTraits<Arg8>::value(arg8), RefargTraits<Arg9>::value(arg9)),
          m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
              Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9, Arg10 arg10)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
            RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6),
            RefargTraits<Arg7>::value(arg7), RefargTraits<Arg8>::value(arg8), RefargTraits<Arg9>::value(arg9),
            RefargTraits<Arg10>::value(arg10)), m_ref_count(0), m_alloc(ma)
    {}

    virtual int add_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return atomic_add(&m_ref_count, 1) + 1;
    }

    virtual int rls_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        const int ret = atomic_add(&m_ref_count, -1) - 1;
        if (0 == ret)
        {
            rc_ptr<memory_allocator> alloc = m_alloc;
            this->~RCWrapper();
            if (alloc.is_not_null())
                alloc->free(this);
            else
                ::free(this);
        }
        return ret;
    }

    virtual int get_ref() const
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return m_ref_count;
    }
};

template <typename T>
class RCWrapper<enrc<T> > : public enrc<T>
{
protected:
    // 引用计数
    int volatile m_ref_count;

    // 内存分配器
    memory_allocator *m_alloc;

    //* 检查多次销毁的检查器
    NUT_DEBUGGING_DESTROY_CHECKER

public:
    RCWrapper(memory_allocator *ma)
        : enrc<T>(), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1>
    RCWrapper(memory_allocator *ma, Arg1 arg1)
        : enrc<T>(arg1), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2)
        : enrc<T>(arg1, arg2), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : enrc<T>(arg1, arg2, arg3), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
        : enrc<T>(arg1, arg2, arg3, arg4), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
        : enrc<T>(arg1, arg2, arg3, arg4, arg5), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
              Arg6 arg6)
        : enrc<T>(arg1, arg2, arg3, arg4, arg5, arg6), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
              Arg6 arg6, Arg7 arg7)
        : enrc<T>(arg1, arg2, arg3, arg4, arg5, arg6, arg7), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
              Arg6 arg6, Arg7 arg7, Arg8 arg8)
        : enrc<T>(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
              Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9)
        : enrc<T>(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
    RCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
              Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9, Arg10 arg10)
        : enrc<T>(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10), m_ref_count(0), m_alloc(ma)
    {}

    virtual int add_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return atomic_add(&m_ref_count, 1) + 1;
    }

    virtual int rls_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        const int ret = atomic_add(&m_ref_count, -1) - 1;
        if (0 == ret)
        {
            rc_ptr<memory_allocator> alloc = m_alloc;
            this->~RCWrapper();
            if (alloc.is_not_null())
                alloc->free(this);
            else
                ::free(this);
        }
        return ret;
    }

    virtual int get_ref() const
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return m_ref_count;
    }
};

}

// 即使用户类的构造函数是private的，也能用 RC_NEW
#define NUT_PRIVATE_RCNEW template <typename ___T> friend class nut::RCWrapper;

#define RC_NEW(ma, type, ...) (type*) (new ((nut::RCWrapper<type>*) nut::ma_alloc((ma), sizeof(nut::RCWrapper<type>))) nut::RCWrapper<type>((ma), ##__VA_ARGS__))

#endif /* head file guarder */
