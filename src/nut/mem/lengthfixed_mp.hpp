/**
 * @file -
 * @author jingqi
 * @date 2012-04-05
 * @last-edit 2015-01-06 22:35:06 jingqi
 */

#ifndef ___HEADFILE_155BBE6F_6F7B_4B42_A097_B9C87EE5EEE0_
#define ___HEADFILE_155BBE6F_6F7B_4B42_A097_B9C87EE5EEE0_

#include <assert.h>

#include <nut/threading/lockfree/atomic.hpp>
#include <nut/debugging/static_assert.hpp>
#include <nut/debugging/destroy_checker.hpp>

#include "memory_allocator.hpp"
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
template <size_t G, size_t MAX_FREE_BLOCKS = 50>
class lengthfixed_mp : public memory_allocator
{
    NUT_STATIC_ASSERT(G > 0);
    typedef lengthfixed_mp<G,MAX_FREE_BLOCKS> self_type;

    union FreeNode
    {
        FreeNode *next;
        uint8_t body[G];
    };

    const ref<memory_allocator> m_alloc;
    TagedPtr<FreeNode> m_head;
    int volatile m_free_num;
    NUT_DEBUGGING_DESTROY_CHECKER

private:
    explicit lengthfixed_mp(const self_type&);
    self_type& operator=(const self_type&);

public:
    lengthfixed_mp(memory_allocator *ma = NULL)
        : m_alloc(ma), m_free_num(0)
    {}

    ~lengthfixed_mp()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        clear();
    }

    void clear()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        FreeNode *p = m_head.ptr;
        while (NULL != p)
        {
            FreeNode *next = p->next;
            ma_free(m_alloc.pointer(), p);
            p = next;
        }
        m_head.ptr = NULL;
        m_free_num = 0;
    }

    void* alloc()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        while (true)
        {
            const TagedPtr<FreeNode> old_head(m_head.cas);

            if (NULL == old_head.ptr)
                return ma_alloc(m_alloc.pointer(), sizeof(FreeNode));

            const TagedPtr<FreeNode> new_head(old_head.ptr->next, old_head.tag + 1);
            if (atomic_cas(&(m_head.cas), old_head.cas, new_head.cas))
            {
                atomic_add(&m_free_num, -1);
                return old_head.ptr;
            }
        }
    }

    virtual void* alloc(size_t cb)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        assert(G == cb);
        void *p = self_type::alloc();
        assert(NULL != p);
        return p;
    }

    virtual void* realloc(void *p, size_t cb)
    {
        assert(G == cb);
        return p;
    }

    virtual void free(void *p)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        assert(NULL != p);
        while(true)
        {
            if (m_free_num >= (int) MAX_FREE_BLOCKS)
            {
                ma_free(m_alloc.pointer(), p);
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
