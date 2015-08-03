
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
    static_assert(G > 0, "模板参数问题");
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
            ma_free(_alloc.pointer(), p);
            p = next;
        }
        _head.ptr = NULL;
        _free_num = 0;
    }

    void* alloc()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        while (true)
        {
            const TagedPtr<FreeNode> old_head(_head.cas);

            if (NULL == old_head.ptr)
                return ma_realloc(_alloc.pointer(), NULL, sizeof(FreeNode));

            const TagedPtr<FreeNode> new_head(old_head.ptr->next, old_head.tag + 1);
            if (atomic_cas(&(_head.cas), old_head.cas, new_head.cas))
            {
                atomic_add(&_free_num, -1);
                return old_head.ptr;
            }
        }
    }

    virtual void* realloc(void *p, size_t cb) override
    {
        assert(NULL == p && G == cb);
        NUT_DEBUGGING_ASSERT_ALIVE;
        if (NULL != p)
            self_type::free(p);
        if (G == cb)
            return self_type::alloc();
        return NULL;
    }

    virtual void free(void *p) override
    {
        assert(NULL != p);
        NUT_DEBUGGING_ASSERT_ALIVE;
        while(true)
        {
            if (_free_num >= (int) MAX_FREE_BLOCKS)
            {
                ma_free(_alloc.pointer(), p);
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
