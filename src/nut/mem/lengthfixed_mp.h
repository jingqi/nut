
#ifndef ___HEADFILE_155BBE6F_6F7B_4B42_A097_B9C87EE5EEE0_
#define ___HEADFILE_155BBE6F_6F7B_4B42_A097_B9C87EE5EEE0_

#include <nut/threading/lockfree/atomic.h>
#include <nut/debugging/destroy_checker.h>

#include "memory_allocator.h"

namespace nut
{

/**
 * 定长内存池(length fixed granularity single thread memory pool)
 */
class lengthfixed_stmp : public memory_allocator
{
    enum
    {
        // 最多缓存的空闲块数
        MAX_FREE_NUM = 50,
    };

    const rc_ptr<memory_allocator> _alloc;
    const size_t _granularity; // 粒度
    int _free_num = 0;
    void *_head = NULL;
    NUT_DEBUGGING_DESTROY_CHECKER

private:
    explicit lengthfixed_stmp(const lengthfixed_stmp&);
    lengthfixed_stmp& operator=(const lengthfixed_stmp&);

public:
    lengthfixed_stmp(size_t granularity, memory_allocator *ma = NULL);
    virtual ~lengthfixed_stmp();

    bool is_empty() const
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return 0 == _free_num;
    }

    void clear();

    virtual void* alloc(size_t sz) override;
    virtual void* realloc(void *p, size_t old_sz, size_t new_sz) override;
    virtual void free(void *p, size_t sz) override;
};

/**
 * 定长内存池(length fixed granularity memory pool)
 */
class lengthfixed_mtmp : public memory_allocator
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
    explicit lengthfixed_mtmp(const lengthfixed_mtmp&);
    lengthfixed_mtmp& operator=(const lengthfixed_mtmp&);

public:
    lengthfixed_mtmp(size_t granularity, memory_allocator *ma = NULL);
    virtual ~lengthfixed_mtmp();

    bool is_empty() const
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return 0 == _free_num;
    }

    void clear();

    virtual void* alloc(size_t sz) override;
    virtual void* realloc(void *p, size_t old_sz, size_t new_sz) override;
    virtual void free(void *p, size_t sz) override;
};

}

#endif /* head file guarder */
