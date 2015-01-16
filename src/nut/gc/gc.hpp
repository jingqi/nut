/**
 * @file -
 * @author jingqi
 * @date 2011-11-11 18:49
 */

#ifndef ___HEADFILE___B77B6DC2_6C30_4732_943B_117AF7E35B83_
#define ___HEADFILE___B77B6DC2_6C30_4732_943B_117AF7E35B83_

#include <stdint.h>
#include <stdlib.h>
#include <new>

#include "gc_wrapper.hpp"
#include "ref_counter.hpp"
#include "ref.hpp"

namespace nut
{

/**
 * 可引用计数对象生成器
 */
template <typename T, typename COUNTER = RefCounter>
class gc_new : public ref<typename RefTraits<T>::plain_type>
{
    typedef typename RefTraits<T>::plain_type plain_type;
    typedef GCWrapper<plain_type, COUNTER> wrapper_type;

    using weak_ref<plain_type>::m_ptr;

    static wrapper_type* alloc_wrapper()
    {
        return (wrapper_type*)::malloc(sizeof(wrapper_type));
    }

    /**
     * @note
     * 该处如此实现的好处：
     * 1. 保证内存分配和销毁的代码在同一模块
     * 2. 即使对象没有申明虚的析构函数也不会析构出错
     */
    static void destroy_wrapper(wrapper_type *p)
    {
        p->~wrapper_type();
        ::free(p);
    }

public:
    gc_new()
    {
        m_ptr = alloc_wrapper();
        new (m_ptr) wrapper_type(1, &destroy_wrapper);
    }

    template <typename Arg1>
    gc_new(Arg1 arg1)
    {
        m_ptr = alloc_wrapper();
        new (m_ptr) wrapper_type(1, &destroy_wrapper, arg1);
    }

    template <typename Arg1, typename Arg2>
    gc_new(Arg1 arg1, Arg2 arg2)
    {
        m_ptr = alloc_wrapper();
        new (m_ptr) wrapper_type(1, &destroy_wrapper, arg1, arg2);
    }

    template <typename Arg1, typename Arg2, typename Arg3>
    gc_new(Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        m_ptr = alloc_wrapper();
        new (m_ptr) wrapper_type(1, &destroy_wrapper, arg1, arg2, arg3);
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    gc_new(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        m_ptr = alloc_wrapper();
        new (m_ptr) wrapper_type(1, &destroy_wrapper, arg1, arg2, arg3, arg4);
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    gc_new(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        m_ptr = alloc_wrapper();
        new (m_ptr) wrapper_type(1, &destroy_wrapper, arg1, arg2, arg3, arg4, arg5);
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6>
    gc_new(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
           Arg6 arg6)
    {
        m_ptr = alloc_wrapper();
        new (m_ptr) wrapper_type(1, &destroy_wrapper, arg1, arg2, arg3, arg4, arg5,
                                 arg6);
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7>
    gc_new(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
           Arg6 arg6, Arg7 arg7)
    {
        m_ptr = alloc_wrapper();
        new (m_ptr) wrapper_type(1, &destroy_wrapper, arg1, arg2, arg3, arg4, arg5,
                                 arg6, arg7);
    }
    
    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8>
    gc_new(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
           Arg6 arg6, Arg7 arg7, Arg8 arg8)
    {
        m_ptr = alloc_wrapper();
        new (m_ptr) wrapper_type(1, &destroy_wrapper, arg1, arg2, arg3, arg4, arg5,
                                 arg6, arg7, arg8);
    }
    
    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    gc_new(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
           Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9)
    {
        m_ptr = alloc_wrapper();
        new (m_ptr) wrapper_type(1, &destroy_wrapper, arg1, arg2, arg3, arg4, arg5,
                                 arg6, arg7, arg8, arg9);
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
        gc_new(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
               Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9, Arg10 arg10)
    {
        m_ptr = alloc_wrapper();
        new (m_ptr) wrapper_type(1, &destroy_wrapper, arg1, arg2, arg3, arg4, arg5,
                                 arg6, arg7, arg8, arg9, arg10);
    }
};

}

#endif /* head file guarder */
