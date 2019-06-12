
#ifndef ___HEADFILE_38E24C42_E36D_453F_A61D_4FE033FF649D_
#define ___HEADFILE_38E24C42_E36D_453F_A61D_4FE033FF649D_

#include <assert.h>
#include <stdint.h>
#include <stddef.h> // for size_t
#include <utility>

#include "../nut_config.h"
#include "../rc/rc_ptr.h"
#include "../debugging/destroy_checker.h"


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

    /** 默认内存块大小，可根据需要调整 */
    static constexpr size_t DEFAULT_BLOCK_LEN = 2048;

    /** 内存块头部大小 */
    static constexpr size_t BLOCK_HEADER_SIZE = sizeof(void*);

    /** 内存块数据部分大小 */
    static constexpr size_t DEFAULT_BLOCK_BODY_SIZE = DEFAULT_BLOCK_LEN - BLOCK_HEADER_SIZE;

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
    ~scoped_gc() noexcept;

    void clear() noexcept;

    void* gc_alloc(size_t sz) noexcept;

    template <typename T, typename ...Args>
    T* gc_new(Args&& ...args) noexcept
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        T *p = (T*) alloc(sizeof(T), destruct_single<T>);
        assert(nullptr != p);
        new (p) T(std::forward<Args>(args)...);
        return p;
    }

    template <typename T>
    T* gc_new_array(size_t count) noexcept
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        T *ret = (T*) alloc(sizeof(T), count, destruct_array<T>);
        assert(nullptr != ret);
        for (size_t i = 0; i < count; ++i)
            new (ret + i) T;
        return ret;
    }

private:
    scoped_gc(const self_type&) = delete;
    self_type& operator=(const self_type&) = delete;

    template <typename T>
    static void destruct_single(void *p) noexcept
    {
        assert(nullptr != p);
        ((T*) p)->~T();
    }

    template <typename T>
    static void destruct_array(void *p) noexcept
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

    void* raw_alloc(size_t sz) noexcept;

    void* alloc(size_t sz, destruct_func_type func) noexcept;

    void* alloc(size_t sz, size_t count, destruct_func_type func) noexcept;

private:
    Block *_current_block = nullptr;
    uint8_t *_end = nullptr;
    DestructorNode *_destruct_chain = nullptr;

    NUT_DEBUGGING_DESTROY_CHECKER
};

}

#endif /* head file guarder */
