﻿
#ifndef ___HEADFILE_38E24C42_E36D_453F_A61D_4FE033FF649D_
#define ___HEADFILE_38E24C42_E36D_453F_A61D_4FE033FF649D_

#include <assert.h>
#include <stdint.h>
#include <stddef.h> // for size_t
#include <utility>

#include <nut/rc/rc_ptr.h>
#include <nut/debugging/destroy_checker.h>

#include "../nut_config.h"


namespace nut
{

/**
 * 由该分配器生成的对象将统一由该分配器的clear()函数进行清理
 * 仅用于单线程环境下
 */
class NUT_API scoped_gc
{
    NUT_REF_COUNTABLE

private:
    typedef scoped_gc self_type;

    enum
    {
        /** 默认内存块大小，可根据需要调整 */
        DEFAULT_BLOCK_LEN = 2048,
        /** 内存块头部大小 */
        BLOCK_HEADER_SIZE = sizeof(void*),
        /** 内存块数据部分大小 */
        DEFAULT_BLOCK_BODY_SIZE = DEFAULT_BLOCK_LEN - BLOCK_HEADER_SIZE,
    };

    /** 内存块 */
    struct Block
    {
        Block *prev = nullptr;
        uint8_t body[DEFAULT_BLOCK_BODY_SIZE];
    };
    static_assert(sizeof(Block) == DEFAULT_BLOCK_LEN, "数据结构对齐问题");

    /** 析构函数 */
    typedef void (*destruct_func_type)(void*);

    /** 析构函数链表 */
    struct DestructorNode
    {
        DestructorNode *prev = nullptr;
        destruct_func_type destruct_func = nullptr;
    };

public:
    scoped_gc() = default;
    ~scoped_gc();

    void clear();

    void* gc_alloc(size_t sz);

#ifndef _LIBCPP_HAS_NO_VARIADICS
    template <typename T, typename ...Args>
    T* gc_new(Args&& ...args)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        T *p = (T*) alloc(sizeof(T), destruct_single<T>);
        assert(nullptr != p);
        new (p) T(std::forward<Args>(args)...);
        return p;
    }
#else
    template <typename T>
    T* gc_new()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        T *p = (T*) alloc(sizeof(T), destruct_single<T>);
        assert(nullptr != p);
        new (p) T;
        return p;
    }

    template <typename T, typename Arg1>
    T* gc_new(Arg1&& arg1)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        T *p = (T*) alloc(sizeof(T), destruct_single<T>);
        assert(nullptr != p);
        new (p) T(std::forward<Arg1>(arg1));
        return p;
    }

    template <typename T, typename Arg1, typename Arg2>
    T* gc_new(Arg1&& arg1, Arg2&& arg2)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        T *p = (T*) alloc(sizeof(T), destruct_single<T>);
        assert(nullptr != p);
        new (p) T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2));
        return p;
    }

    template <typename T, typename Arg1, typename Arg2, typename Arg3>
    T* gc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        T *p = (T*) alloc(sizeof(T), destruct_single<T>);
        assert(nullptr != p);
        new (p) T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3));
        return p;
    }

    template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    T* gc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        T *p = (T*) alloc(sizeof(T), destruct_single<T>);
        assert(nullptr != p);
        new (p) T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4));
        return p;
    }

    template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    T* gc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        T *p = (T*) alloc(sizeof(T), destruct_single<T>);
        assert(nullptr != p);
        new (p) T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5));
        return p;
    }

    template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6>
    T* gc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        T *p = (T*) alloc(sizeof(T), destruct_single<T>);
        assert(nullptr != p);
        new (p) T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
                  std::forward<Arg6>(arg6));
        return p;
    }

    template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7>
    T* gc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        T *p = (T*) alloc(sizeof(T), destruct_single<T>);
        assert(nullptr != p);
        new (p) T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
                  std::forward<Arg6>(arg6), std::forward<Arg7>(arg7));
        return p;
    }

    template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8>
    T* gc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        T *p = (T*) alloc(sizeof(T), destruct_single<T>);
        assert(nullptr != p);
        new (p) T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
                  std::forward<Arg6>(arg6), std::forward<Arg7>(arg7), std::forward<Arg8>(arg8));
        return p;
    }

    template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    T* gc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        T *p = (T*) alloc(sizeof(T), destruct_single<T>);
        assert(nullptr != p);
        new (p) T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
                  std::forward<Arg6>(arg6), std::forward<Arg7>(arg7), std::forward<Arg8>(arg8), std::forward<Arg9>(arg9));
        return p;
    }

    template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
    T* gc_new(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
              Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9, Arg10&& arg10)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        T *p = (T*) alloc(sizeof(T), destruct_single<T>);
        assert(nullptr != p);
        new (p) T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5),
                  std::forward<Arg6>(arg6), std::forward<Arg7>(arg7), std::forward<Arg8>(arg8), std::forward<Arg9>(arg9), std::forward<Arg10>(arg10));
        return p;
    }
#endif

    template <typename T>
    T* gc_new_array(size_t count)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        T *ret = (T*) alloc(sizeof(T), count, destruct_array<T>);
        assert(nullptr != ret);
        for (size_t i = 0; i < count; ++i)
            new (ret + i) T;
        return ret;
    }

private:
    // Non-copyable
    scoped_gc(const self_type&) = delete;
    self_type& operator=(const self_type&) = delete;

    template <typename T>
    static void destruct_single(void *p)
    {
        assert(nullptr != p);
        ((T*) p)->~T();
    }

    template <typename T>
    static void destruct_array(void *p)
    {
        assert(nullptr != p);
        size_t count = *(size_t*)p;
        T *pd = (T*)(((size_t*) p) + 1);
        for (int i = 0; i < (int) count; ++i)
        {
            pd->~T();
            ++pd;
        }
    }

    void* raw_alloc(size_t sz);

    void* alloc(size_t sz, destruct_func_type func);

    void* alloc(size_t sz, size_t count, destruct_func_type func);

private:
    Block *_current_block = nullptr;
    uint8_t *_end = nullptr;
    DestructorNode *_destruct_chain = nullptr;
    NUT_DEBUGGING_DESTROY_CHECKER
};

}

#endif /* head file guarder */
