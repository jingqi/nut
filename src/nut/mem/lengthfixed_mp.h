
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
 */
class lengthfixed_mp : public memory_allocator
{
    enum
    {
        // 最多缓存的空闲块数
        MAX_FREE_NUM = 50,
    };

    const rc_ptr<memory_allocator> _alloc;
    const size_t _granularity; // 粒度
    int volatile _free_num = 0;
    TagedPtr<void> _head;
    NUT_DEBUGGING_DESTROY_CHECKER

private:
    typedef lengthfixed_mp self_type;
    explicit lengthfixed_mp(const self_type&);
    self_type& operator=(const self_type&);

public:
    lengthfixed_mp(size_t granularity, memory_allocator *ma = NULL);
    virtual ~lengthfixed_mp();

    bool is_empty() const
    {
        return 0 == _free_num;
    }

    void clear();

    virtual void* alloc(size_t sz) override;
    virtual void* realloc(void *p, size_t old_sz, size_t new_sz) override;
    virtual void free(void *p, size_t sz) override;
};

}

#endif /* head file guarder */
