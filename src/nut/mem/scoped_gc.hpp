/**
 * @file -
 * @author jingqi
 * @date 2012-03-11
 * @last-edit 2012-03-11 17:36:58 jingqi
 */

#ifndef ___HEADFILE_38E24C42_E36D_453F_A61D_4FE033FF649D_
#define ___HEADFILE_38E24C42_E36D_453F_A61D_4FE033FF649D_

#include <assert.h>
#include <nut/platform/stdint.hpp>
#include <nut/memtool/refarg.hpp>
#include <nut/debugging/static_assert.hpp>

#include "sys_ma.hpp"

namespace nut
{

/**
 * 由该分配器生成的对象将统一由该分配器的clear()函数进行清理
 */
template <typename MemAlloc = sys_ma>
class scoped_gc
{
    enum
	{
		/** 默认内存块大小，可根据需要调整 */
		DEFAULT_BLOCK_LEN = 2048,
		/** 内存块头部大小 */
		BLOCK_HEADER_SIZE = sizeof(void*) + sizeof(size_t),
		/** 内存块数据部分大小 */
		DEFAULT_BLOCK_BODY_SIZE = DEFAULT_BLOCK_LEN - BLOCK_HEADER_SIZE,
	};

	/** 内存块 */
    struct Block
    {
        Block *prev;
		size_t blockSize;
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

	MemAlloc m_mem_alloc;
	Block *m_currentBlock;
	uint8_t *m_end;
    DestructorNode *m_destructChain;

private:
    explicit scoped_gc(const scoped_gc&);
    scoped_gc& operator=(const scoped_gc&);

	template <typename T>
	static void destructSingle(void *p)
	{
		assert(NULL != p);
		((T*) p)->~T();
	}

	template <typename T>
	static void destructArray(void *p)
	{
		assert(NULL != p);
		size_t count = *(size_t*)p;
		T *pd = (T*)(((size_t*) p) + 1);
		for (register int i = 0; i < (int) count; ++i)
		{
			pd->~T();
			++pd;
		}
	}

public:
    scoped_gc()
        : m_currentBlock(NULL), m_end(NULL), m_destructChain(NULL)
    {}

    ~scoped_gc()
    {
		clear();
	}

private:

    void* raw_alloc(size_t cb)
    {
		if (m_currentBlock->body + cb > m_end)
		{
			if (cb >= DEFAULT_BLOCK_BODY_SIZE)
			{
				Block *newBlk = (Block*) m_mem_alloc.alloc(BLOCK_HEADER_SIZE + cb);
				assert(NULL != newBlk);
				newBlk->blockSize = BLOCK_HEADER_SIZE + cb;
				if (NULL != m_currentBlock)
				{
					newBlk->prev = m_currentBlock->prev;
					m_currentBlock->prev = newBlk;
				}
				else
				{
					newBlk->prev = NULL;
					m_currentBlock = newBlk;
					m_end = m_currentBlock->body;
				}
				return newBlk->body;
			}
			else
			{
				Block *newBlk = (Block*) m_mem_alloc.alloc(DEFAULT_BLOCK_LEN);
				assert(NULL != newBlk);
				newBlk->blockSize = DEFAULT_BLOCK_LEN;
				newBlk->prev = m_currentBlock;
				m_currentBlock = newBlk;
				m_end = m_currentBlock->body + DEFAULT_BLOCK_BODY_SIZE;
			}
		}
		m_end -= cb;
		return m_end;
	}
	
	void* alloc(size_t cb, destruct_func_type func)
	{
		DestructorNode *dn = (DestructorNode*) raw_alloc(sizeof(DestructorNode) + cb);
		assert(NULL != dn);
		dn->destruct_func = func;
		dn->prev = m_destructChain;
		m_destructChain = dn;
		return dn + 1;
	}
	
	void* alloc(size_t cb, size_t count, destruct_func_type func)
	{
		DestructorNode *dn = (DestructorNode*) raw_alloc(sizeof(DestructorNode) + sizeof(size_t) + cb * count);
		assert(NULL != dn);
		dn->destruct_func = func;
		*(size_t*)(dn + 1) = count;
		dn->prev = m_destructChain;
		m_destructChain = dn;
		return ((size_t*)(dn + 1)) + 1;
	}

public:
    void clear()
    {
        while (NULL != m_destructChain)
        {
            assert(NULL != m_destructChain->destruct_func);
            m_destructChain->destruct_func(m_destructChain + 1);
            m_destructChain = m_destructChain->prev;
        }
        
        while (NULL != m_currentBlock)
        {
			Block *prev = m_currentBlock->prev;
			m_mem_alloc.free((uint8_t*) m_currentBlock, m_currentBlock->blockSize);
			m_currentBlock = prev;
        }
		m_end = NULL;
    }

	void* gc_alloc(size_t cb)
	{
		void* ret = raw_alloc(cb);
		assert(NULL != ret);
		return ret;
	}

    template <typename T>
    T* gc_new()
    {
		T *ret = (T*) alloc(sizeof(T), destructSingle<T>);
		assert(NULL != ret);
		new (ret) T;
		return ret;
	}

    template <typename T, typename A1>
    T* gc_new(A1 a1)
    {
		T *ret = (T*) alloc(sizeof(T), destructSingle<T>);
		assert(NULL != ret);
		new (ret) T(RefargTraits<A1>::value(a1));
		return ret;
	}

    template <typename T, typename A1, typename A2>
    T* gc_new(A1 a1, A2 a2)
    {
		T *ret = (T*) alloc(sizeof(T), destructSingle<T>);
		assert(NULL != ret);
		new (ret) T(RefargTraits<A1>::value(a1), RefargTraits<A2>::value(a2));
		return ret;
	}

	template <typename T>
	T* gc_new_array(size_t count)
	{
		T *ret = (T*) alloc(sizeof(T), count, destructArray<T>);
		assert(NULL != ret);
		for (register int i = 0; i < (int) count; ++i)
		{
			new (ret + i) T;
		}
		return ret;
	}
};

}

#endif /* head file guarder */

