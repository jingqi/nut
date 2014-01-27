/**
 * @file -
 * @author jingqi
 * @date 2012-04-05
 * @last-edit 2012-08-19 18:52:05 jingqi
 */

#ifndef ___HEADFILE_155BBE6F_6F7B_4B42_A097_B9C87EE5EEE0_
#define ___HEADFILE_155BBE6F_6F7B_4B42_A097_B9C87EE5EEE0_

#include <assert.h>

#include <nut/debugging/static_assert.hpp>
#include <nut/threading/lockfree/atomic.hpp>

#include "sys_ma.hpp"

namespace nut
{

/**
 * 固定粒度内存池(size fixed memory pool)
 *
 * @param G                 粒度
 * @param MAX_FREE_BLOCKS   最多缓存的块数
 * @param sys_ma            内存分配器
 */
template <size_t G, size_t MAX_FREE_BLOCKS = 50, typename MemAlloc = sys_ma>
class lengthfixed_mp
{
    NUT_STATIC_ASSERT(G > 0);
    typedef lengthfixed_mp<G,MAX_FREE_BLOCKS,MemAlloc> self_type;

    union FreeNode
    {
        FreeNode *next;
        uint8_t body[G];
    };

    MemAlloc m_mem_alloc;
    TagedPtr<FreeNode> m_head;
    int volatile m_free_num;

private:
    explicit lengthfixed_mp(const lengthfixed_mp&);
    lengthfixed_mp& operator=(const lengthfixed_mp&);

public:
    lengthfixed_mp()
        : m_free_num(0)
    {}

    ~lengthfixed_mp()
    {
        clear();
    }

public:
    void clear()
    {
        FreeNode *p = m_head.ptr;
        while (NULL != p)
        {
            FreeNode *next = p->next;
            m_mem_alloc.free(p, G);
            p = next;
        }
        m_head.ptr = NULL;
        m_free_num = 0;
    }

    void* alloc()
    {
        while (true)
        {
            const TagedPtr<FreeNode> oldHead(m_head.cas);

            if (NULL == oldHead.ptr)
                return m_mem_alloc.alloc(G);

            const TagedPtr<FreeNode> newHead(oldHead.ptr->next, oldHead.tag + 1);
            if (atomic_cas(&(m_head.cas), oldHead.cas, newHead.cas))
            {
                atomic_add(&m_free_num, -1);
                return oldHead.ptr;
            }
        }
    }

    void free(void *p)
    {
        assert(NULL != p);
        while(true)
        {
            if (m_free_num >= (int) MAX_FREE_BLOCKS)
            {
                m_mem_alloc.free(p, G);
                return;
            }

            const TagedPtr<FreeNode> oldHead(m_head.cas);
            reinterpret_cast<FreeNode*>(p)->next = oldHead.ptr;
            const TagedPtr<FreeNode> newHead(reinterpret_cast<FreeNode*>(p), oldHead.tag + 1);
            if (atomic_cas(&(m_head.cas), oldHead.cas, newHead.cas))
            {
                atomic_add(&m_free_num, 1);
                return;
            }
        }
    }

    inline void* alloc(size_t cb)
    {
        assert(G == cb);
        void *p = self_type::alloc();
        assert(NULL != p);
        return p;
    }

    inline void free(void *p, size_t cb)
    {
        assert(NULL != p && G == cb);
        self_type::free(p);
    }
};

}

#endif /* head file guarder */

