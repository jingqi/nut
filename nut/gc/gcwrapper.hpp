/**
 * @file -
 * @author jingqi
 * @date 2011-11-19
 */

#ifndef ___HEADFILE_4AB18864_614E_4752_A996_3CC927369C09_
#define ___HEADFILE_4AB18864_614E_4752_A996_3CC927369C09_

#include <nut/memtool/refarg.hpp>
#include <nut/debugging/destroychecker.hpp>
#include "refcounter.hpp"
#include "enref.hpp"

namespace nut
{


/**
 * 可引用计数对象包装器
 *
 * @note
 * 使用包装器来包装的好处：
 * 1. 析构函数的部分见gc_new的note
 * 2. 如果对象是多继承的，计数器仍然只有一份，仍然能正常运行
 */
template <typename T, typename COUNTER = RefCounter>
class GCWrapper : public T
{
protected:
    /** 销毁器类型 */
    typedef void (*destroyer)(GCWrapper<T,COUNTER>*);

    /** 引用计数 */
    COUNTER m_counter;
    /** 销毁器 */
    destroyer m_destroyer;

    /** 避免多次销毁的检查器 */
    NUT_DEBUGGING_DESTROY_CHECKER

public:
    GCWrapper(int c, destroyer d) : T(), m_counter(c), m_destroyer(d) {}

    template <typename Arg1>
    GCWrapper(int c, destroyer d, Arg1 arg1)
        : T(RefargTraits<Arg1>::value(arg1)), m_counter(c), m_destroyer(d)
    {}

    template <typename Arg1, typename Arg2>
    GCWrapper(int c, destroyer d, Arg1 arg1, Arg2 arg2)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2)), m_counter(c), m_destroyer(d)
    {}

    template <typename Arg1, typename Arg2, typename Arg3>
    GCWrapper(int c, destroyer d, Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3)),
        m_counter(c), m_destroyer(d)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    GCWrapper(int c, destroyer d, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
        RefargTraits<Arg4>::value(arg4)), m_counter(c), m_destroyer(d)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    GCWrapper(int c, destroyer d, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
        RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5)), m_counter(c), m_destroyer(d)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6>
    GCWrapper(int c, destroyer d, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
              Arg6 arg6)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
        RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6)),
        m_counter(c), m_destroyer(d)
    {}

    virtual void add_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        ++m_counter;
    }

    virtual void rls_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        if (--m_counter == 0)
            m_destroyer(this);
    }
};

template <typename T, typename COUNTER>
class GCWrapper<enref<T>,COUNTER> : public enref<T>
{
protected:
    /** 销毁器类型 */
    typedef void (*destroyer)(GCWrapper<enref<T>,COUNTER>*);

    /** 引用计数 */
    COUNTER m_counter;
    /** 销毁器 */
    destroyer m_destroyer;

    /** 检查多次销毁的检查器 */
    NUT_DEBUGGING_DESTROY_CHECKER

public:
    GCWrapper(int c, destroyer d) : T(), m_counter(c), m_destroyer(d) {}

    template <typename Arg1>
    GCWrapper(int c, destroyer d, Arg1 arg1)
        : enref<T>(arg1), m_counter(c), m_destroyer(d)
    {}

    template <typename Arg1, typename Arg2>
    GCWrapper(int c, destroyer d, Arg1 arg1, Arg2 arg2)
        : enref<T>(arg1, arg2), m_counter(c), m_destroyer(d)
    {}

    template <typename Arg1, typename Arg2, typename Arg3>
    GCWrapper(int c, destroyer d, Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : enref<T>(arg1, arg2, arg3), m_counter(c), m_destroyer(d)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    GCWrapper(int c, destroyer d, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
        : enref<T>(arg1, arg2, arg3, arg4), m_counter(c), m_destroyer(d)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    GCWrapper(int c, destroyer d, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
        : enref<T>(arg1, arg2, arg3, arg4, arg5), m_counter(c), m_destroyer(d)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
        typename Arg6>
        GCWrapper(int c, destroyer d, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
        Arg6 arg6)
        : enref<T>(arg1, arg2, arg3, arg4, arg5, arg6), m_counter(c), m_destroyer(d)
    {}

    virtual void add_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        ++m_counter;
    }

    virtual void rls_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        if (--m_counter == 0)
            m_destroyer(this);
    }
};

/** 即使用户类的构造函数是private的，也能用gc_new */
#define NUT_GC_PRIVATE_GCNEW \
template <typename ___T, typename ___C> friend class nut::GCWrapper;

}

#endif
