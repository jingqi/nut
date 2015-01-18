﻿/**
 * @file -
 * @author jingqi
 * @date 2012-03-11
 * @last-edit 2015-01-06 22:33:09 jingqi
 */

#ifndef ___HEADFILE_38E24C42_E36D_453F_A61D_4FE033FF649D_
#define ___HEADFILE_38E24C42_E36D_453F_A61D_4FE033FF649D_

#include <assert.h>
#include <nut/platform/stdint.hpp>
#include <nut/memtool/refarg.hpp>
#include <nut/debugging/static_assert.hpp>
#include <nut/debugging/destroy_checker.hpp>

#include "sys_ma.hpp"

namespace nut
{

/**
 * 由该分配器生成的对象将统一由该分配器的clear()函数进行清理
 */
template <typename MemAlloc = sys_ma>
class scoped_gc
{
    typedef scoped_gc<MemAlloc> self_type;

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

    int volatile m_ref_count;
    MemAlloc *const m_alloc;
	Block *m_current_block;
	uint8_t *m_end;
    DestructorNode *m_destruct_chain;
    NUT_DEBUGGING_DESTROY_CHECKER

private:
    explicit scoped_gc(const self_type&);
    self_type& operator=(const self_type&);

    scoped_gc(MemAlloc *ma)
        : m_ref_count(0), m_alloc(ma), m_current_block(NULL), m_end(NULL), m_destruct_chain(NULL)
    {
        if (NULL != m_alloc)
            m_alloc->add_ref();
    }

    ~scoped_gc()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
		clear();
        if (NULL != m_alloc)
            m_alloc->rls_ref();
	}

public:
    static self_type* create(MemAlloc *ma = NULL)
    {
        self_type *ret = NULL;
        if (NULL != ma)
            ret = (self_type*) ma->alloc(sizeof(self_type));
        else
            ret = (self_type*) ::malloc(sizeof(self_type));
        assert(NULL != ret);
        new (ret) self_type(ma);
        ret->add_ref();
        return ret;
    }

    int add_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return atomic_add(&m_ref_count, 1) + 1;
    }

    int rls_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        const int ret = atomic_add(&m_ref_count, -1) - 1;
        if (0 == ret)
        {
            MemAlloc *const ma = m_alloc;
            if (NULL != ma)
                ma->add_ref();
            this->~scoped_gc();
            if (NULL != ma)
            {
                ma->free(this);
                ma->rls_ref();
            }
            else
            {
                ::free(this);
            }
        }
        return ret;
    }

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

    void* raw_alloc(size_t cb)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
		if (m_current_block->body + cb > m_end)
		{
			if (cb >= DEFAULT_BLOCK_BODY_SIZE)
			{
                Block *new_blk = NULL;
                if (NULL != m_alloc)
                    new_blk = (Block*) m_alloc->alloc(BLOCK_HEADER_SIZE + cb);
                else
                    new_blk = (Block*) ::malloc(BLOCK_HEADER_SIZE + cb);
				assert(NULL != new_blk);

				if (NULL != m_current_block)
				{
					new_blk->prev = m_current_block->prev;
					m_current_block->prev = new_blk;
				}
				else
				{
					new_blk->prev = NULL;
					m_current_block = new_blk;
					m_end = m_current_block->body;
				}
				return new_blk->body;
			}
			else
			{
                Block *new_blk = NULL;
                if (NULL != m_alloc)
                    new_blk = (Block*) m_alloc->alloc(DEFAULT_BLOCK_LEN);
                else
                    new_blk = (Block*) ::malloc(DEFAULT_BLOCK_LEN);
				assert(NULL != new_blk);

				new_blk->prev = m_current_block;
				m_current_block = new_blk;
				m_end = m_current_block->body + DEFAULT_BLOCK_BODY_SIZE;
			}
		}
		m_end -= cb;
		return m_end;
	}

	void* alloc(size_t cb, destruct_func_type func)
	{
        NUT_DEBUGGING_ASSERT_ALIVE;
		DestructorNode *dn = (DestructorNode*) raw_alloc(sizeof(DestructorNode) + cb);
		assert(NULL != dn);
		dn->destruct_func = func;
		dn->prev = m_destruct_chain;
		m_destruct_chain = dn;
		return dn + 1;
	}

	void* alloc(size_t cb, size_t count, destruct_func_type func)
	{
        NUT_DEBUGGING_ASSERT_ALIVE;
		DestructorNode *dn = (DestructorNode*) raw_alloc(sizeof(DestructorNode) + sizeof(size_t) + cb * count);
		assert(NULL != dn);
		dn->destruct_func = func;
		*(size_t*)(dn + 1) = count;
		dn->prev = m_destruct_chain;
		m_destruct_chain = dn;
		return ((size_t*)(dn + 1)) + 1;
	}

public:
    void clear()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        while (NULL != m_destruct_chain)
        {
            assert(NULL != m_destruct_chain->destruct_func);
            m_destruct_chain->destruct_func(m_destruct_chain + 1);
            m_destruct_chain = m_destruct_chain->prev;
        }

        while (NULL != m_current_block)
        {
			Block *prev = m_current_block->prev;
            if (NULL != m_alloc)
                m_alloc->free(m_current_block);
            else
                ::free(m_current_block);
			m_current_block = prev;
        }
		m_end = NULL;
    }

	void* gc_alloc(size_t cb)
	{
        NUT_DEBUGGING_ASSERT_ALIVE;
		void* ret = raw_alloc(cb);
		assert(NULL != ret);
		return ret;
	}

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
		for (int i = 0; i < (int) count; ++i)
		{
			new (ret + i) T;
		}
		return ret;
	}
};

}

#endif /* head file guarder */
