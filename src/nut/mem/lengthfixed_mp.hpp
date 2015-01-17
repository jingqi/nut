/**
 * @file -
 * @author jingqi
 * @date 2012-04-05
 * @last-edit 2015-01-06 22:35:06 jingqi
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

    MemAlloc *m_alloc;
    TagedPtr<FreeNode> m_head;
    int volatile m_free_num;

private:
    explicit lengthfixed_mp(const lengthfixed_mp&);
    lengthfixed_mp& operator=(const lengthfixed_mp&);

public:
    lengthfixed_mp(MemAlloc *ma = NULL)
        : m_alloc(ma), m_free_num(0)
    {
        if (NULL == ma)
            m_alloc = sys_ma::create();
        else
            m_alloc->add_ref();
    }

    ~lengthfixed_mp()
    {
        clear();
        m_alloc->rls_ref();
        m_alloc = NULL;
    }

public:
    void clear()
    {
        FreeNode *p = m_head.ptr;
        while (NULL != p)
        {
            FreeNode *next = p->next;
            m_alloc->free(p);
            p = next;
        }
        m_head.ptr = NULL;
        m_free_num = 0;
    }

    void* alloc()
    {
        while (true)
        {
            const TagedPtr<FreeNode> old_head(m_head.cas);

            if (NULL == old_head.ptr)
                return m_alloc->alloc(sizeof(FreeNode));

            const TagedPtr<FreeNode> new_head(old_head.ptr->next, old_head.tag + 1);
            if (atomic_cas(&(m_head.cas), old_head.cas, new_head.cas))
            {
                atomic_add(&m_free_num, -1);
                return old_head.ptr;
            }
        }
    }

    void* alloc(size_t cb)
    {
        assert(G == cb);
        void *p = self_type::alloc();
        assert(NULL != p);
        return p;
    }

    void free(void *p)
    {
        assert(NULL != p);
        while(true)
        {
            if (m_free_num >= (int) MAX_FREE_BLOCKS)
            {
                m_alloc->free(p);
                return;
            }

            const TagedPtr<FreeNode> old_head(m_head.cas);
            reinterpret_cast<FreeNode*>(p)->next = old_head.ptr;
            const TagedPtr<FreeNode> new_head(reinterpret_cast<FreeNode*>(p), old_head.tag + 1);
            if (atomic_cas(&(m_head.cas), old_head.cas, new_head.cas))
            {
                atomic_add(&m_free_num, 1);
                return;
            }
        }
    }
};

}

#endif /* head file guarder */
