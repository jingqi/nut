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

#include <nut/mem/memory_allocator.hpp>

#include "gc_wrapper.hpp"
#include "ref.hpp"

namespace nut
{

/**
 * 可引用计数对象生成器
 */
template <typename T>
class gc_new : public ref<typename RefTraits<T>::plain_type>
{
    typedef typename RefTraits<T>::plain_type plain_type;
    typedef GCWrapper<plain_type> wrapper_type;

    using weak_ref<plain_type>::m_ptr;

public:
    gc_new(memory_allocator *ma)
    {
        m_ptr = (wrapper_type*) ma_alloc(ma, sizeof(wrapper_type));
        new (m_ptr) wrapper_type(1, ma);
    }

    template <typename Arg1>
    gc_new(memory_allocator *ma, Arg1 arg1)
    {
        m_ptr = (wrapper_type*) ma_alloc(ma, sizeof(wrapper_type));
        new (m_ptr) wrapper_type(1, ma, arg1);
    }

    template <typename Arg1, typename Arg2>
    gc_new(memory_allocator *ma, Arg1 arg1, Arg2 arg2)
    {
        m_ptr = (wrapper_type*) ma_alloc(ma, sizeof(wrapper_type));
        new (m_ptr) wrapper_type(1, ma, arg1, arg2);
    }

    template <typename Arg1, typename Arg2, typename Arg3>
    gc_new(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        m_ptr = (wrapper_type*) ma_alloc(ma, sizeof(wrapper_type));
        new (m_ptr) wrapper_type(1, ma, arg1, arg2, arg3);
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    gc_new(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        m_ptr = (wrapper_type*) ma_alloc(ma, sizeof(wrapper_type));
        new (m_ptr) wrapper_type(1, ma, arg1, arg2, arg3, arg4);
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    gc_new(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
    {
        m_ptr = (wrapper_type*) ma_alloc(ma, sizeof(wrapper_type));
        new (m_ptr) wrapper_type(1, ma, arg1, arg2, arg3, arg4, arg5);
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6>
    gc_new(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
           Arg6 arg6)
    {
        m_ptr = (wrapper_type*) ma_alloc(ma, sizeof(wrapper_type));
        new (m_ptr) wrapper_type(1, ma, arg1, arg2, arg3, arg4, arg5,
                                 arg6);
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7>
    gc_new(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
           Arg6 arg6, Arg7 arg7)
    {
        m_ptr = (wrapper_type*) ma_alloc(ma, sizeof(wrapper_type));
        new (m_ptr) wrapper_type(1, ma, arg1, arg2, arg3, arg4, arg5,
                                 arg6, arg7);
    }
    
    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8>
    gc_new(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
           Arg6 arg6, Arg7 arg7, Arg8 arg8)
    {
        m_ptr = (wrapper_type*) ma_alloc(ma, sizeof(wrapper_type));
        new (m_ptr) wrapper_type(1, ma, arg1, arg2, arg3, arg4, arg5,
                                 arg6, arg7, arg8);
    }
    
    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    gc_new(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
           Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9)
    {
        m_ptr = (wrapper_type*) ma_alloc(ma, sizeof(wrapper_type));
        new (m_ptr) wrapper_type(1, ma, arg1, arg2, arg3, arg4, arg5,
                                 arg6, arg7, arg8, arg9);
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
        gc_new(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
               Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9, Arg10 arg10)
    {
        m_ptr = (wrapper_type*) ma_alloc(ma, sizeof(wrapper_type));
        new (m_ptr) wrapper_type(1, ma, arg1, arg2, arg3, arg4, arg5,
                                 arg6, arg7, arg8, arg9, arg10);
    }
};

}

#endif /* head file guarder */
