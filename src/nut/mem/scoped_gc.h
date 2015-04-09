
#ifndef ___HEADFILE_38E24C42_E36D_453F_A61D_4FE033FF649D_
#define ___HEADFILE_38E24C42_E36D_453F_A61D_4FE033FF649D_

#include <assert.h>
#include <nut/platform/stdint.h>
#include <nut/memtool/refarg.h>
#include <nut/debugging/static_assert.h>
#include <nut/debugging/destroy_checker.h>

#include "sys_ma.h"

namespace nut
{

/**
 * 由该分配器生成的对象将统一由该分配器的clear()函数进行清理
 */
class scoped_gc
{
    NUT_REF_COUNTABLE

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
        Block *prev;
        uint8_t body[DEFAULT_BLOCK_BODY_SIZE];
    };
	NUT_STATIC_ASSERT(sizeof(Block) == DEFAULT_BLOCK_LEN);

	/** 析构函数 */
    typedef void (*destruct_func_type)(void*);

	/** 析构函数链表 */
    struct DestructorNode
    {
        DestructorNode *prev;
        destruct_func_type destruct_func;
    };

    const rc_ptr<memory_allocator> m_alloc;
	Block *m_current_block;
	uint8_t *m_end;
    DestructorNode *m_destruct_chain;
    NUT_DEBUGGING_DESTROY_CHECKER

private:
    explicit scoped_gc(const self_type&);
    self_type& operator=(const self_type&);

public:
    scoped_gc(memory_allocator *ma = NULL);
    ~scoped_gc();

private:
    template <typename T>
    static void destruct_single(void *p)
    {
        assert(NULL != p);
        ((T*) p)->~T();
    }

    template <typename T>
    static void destruct_array(void *p)
    {
        assert(NULL != p);
        size_t count = *(size_t*)p;
        T *pd = (T*)(((size_t*) p) + 1);
        for (int i = 0; i < (int) count; ++i)
        {
            pd->~T();
            ++pd;
        }
    }

    void* raw_alloc(size_t cb);

    void* alloc(size_t cb, destruct_func_type func);

    void* alloc(size_t cb, size_t count, destruct_func_type func);

public:
    void clear();

    void* gc_alloc(size_t cb);

    template <typename T>
    T* gc_new()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
		T *ret = (T*) alloc(sizeof(T), destruct_single<T>);
		assert(NULL != ret);
		new (ret) T;
		return ret;
	}

    template <typename T, typename A1>
    T* gc_new(A1 a1)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
		T *ret = (T*) alloc(sizeof(T), destruct_single<T>);
		assert(NULL != ret);
		new (ret) T(RefargTraits<A1>::value(a1));
		return ret;
	}

    template <typename T, typename A1, typename A2>
    T* gc_new(A1 a1, A2 a2)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
		T *ret = (T*) alloc(sizeof(T), destruct_single<T>);
		assert(NULL != ret);
		new (ret) T(RefargTraits<A1>::value(a1), RefargTraits<A2>::value(a2));
		return ret;
	}

	template <typename T>
	T* gc_new_array(size_t count)
	{
        NUT_DEBUGGING_ASSERT_ALIVE;
		T *ret = (T*) alloc(sizeof(T), count, destruct_array<T>);
		assert(NULL != ret);
		for (size_t i = 0; i < count; ++i)
			new (ret + i) T;
		return ret;
	}
};

}

#endif /* head file guarder */
