
#ifndef ___HEADFILE_155BBE6F_6F7B_4B42_A097_B9C87EE5EEE0_
#define ___HEADFILE_155BBE6F_6F7B_4B42_A097_B9C87EE5EEE0_

#include <assert.h>

#include <nut/threading/lockfree/atomic.h>
#include <nut/debugging/destroy_checker.h>

#include "memory_allocator.h"
#include "sys_ma.h"

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
    static_assert(G > 0, "Size of memory block should bigger than 0");
    typedef lengthfixed_mp<G,MAX_FREE_BLOCKS> self_type;

    union FreeNode
    {
        FreeNode *next = NULL;
        uint8_t body[G];
    };

    const rc_ptr<memory_allocator> _alloc;
    TagedPtr<FreeNode> _head;
    int volatile _free_num = 0;
    NUT_DEBUGGING_DESTROY_CHECKER

private:
    explicit lengthfixed_mp(const self_type&);
    self_type& operator=(const self_type&);

public:
    lengthfixed_mp(memory_allocator *ma = NULL)
        : _alloc(ma)
    {}

    ~lengthfixed_mp()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        clear();
    }

    void clear()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;

        FreeNode *p = _head.ptr;
        while (NULL != p)
        {
            FreeNode *next = p->next;
            ma_free(_alloc, p, sizeof(FreeNode));
            p = next;
        }
        _head.ptr = NULL;
        _free_num = 0;
    }

    virtual void* alloc(size_t sz) override
    {
        assert(G == sz);
        NUT_DEBUGGING_ASSERT_ALIVE;

        while (true)
        {
            const TagedPtr<FreeNode> old_head(_head.cas);

            if (NULL == old_head.ptr)
                return ma_alloc(_alloc, sizeof(FreeNode));

            const TagedPtr<FreeNode> new_head(old_head.ptr->next, old_head.tag + 1);
            if (atomic_cas(&(_head.cas), old_head.cas, new_head.cas))
            {
                atomic_add(&_free_num, -1);
                return old_head.ptr;
            }
        }
    }

    virtual void* realloc(void *p, size_t old_sz, size_t new_sz) override
    {
        assert(NULL != p && G == old_sz && G == new_sz);
        NUT_DEBUGGING_ASSERT_ALIVE;
        return p;
    }

    virtual void free(void *p, size_t sz) override
    {
        assert(NULL != p && G == sz);
        NUT_DEBUGGING_ASSERT_ALIVE;

        while(true)
        {
            if (_free_num >= (int) MAX_FREE_BLOCKS)
            {
                ma_free(_alloc, p, sizeof(FreeNode));
                return;
            }

            const TagedPtr<FreeNode> old_head(_head.cas);
            reinterpret_cast<FreeNode*>(p)->next = old_head.ptr;
            const TagedPtr<FreeNode> new_head(reinterpret_cast<FreeNode*>(p), old_head.tag + 1);
            if (atomic_cas(&(_head.cas), old_head.cas, new_head.cas))
            {
                atomic_add(&_free_num, 1);
                return;
            }
        }
    }
};

}

#endif /* head file guarder */
