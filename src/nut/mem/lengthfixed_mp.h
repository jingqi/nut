
#ifndef ___HEADFILE_155BBE6F_6F7B_4B42_A097_B9C87EE5EEE0_
#define ___HEADFILE_155BBE6F_6F7B_4B42_A097_B9C87EE5EEE0_

#include <atomic>

#include <nut/debugging/destroy_checker.h>

#include "../nut_config.h"
#include "memory_allocator.h"


namespace nut
{

/**
 * 定长内存池(length fixed granularity single thread memory pool)
 */
class NUT_API lengthfixed_stmp : public memory_allocator
{
    NUT_DEBUGGING_DESTROY_CHECKER

private:
    enum { MAX_FREE_NUM = 50 }; // 最多缓存的空闲块数

public:
    lengthfixed_stmp(size_t granularity, memory_allocator *ma = nullptr);
    virtual ~lengthfixed_stmp();

    bool is_empty() const;
    void clear();

    virtual void* alloc(size_t sz) override;
    virtual void* realloc(void *p, size_t old_sz, size_t new_sz) override;
    virtual void free(void *p, size_t sz) override;

private:
    // Non-copyable
    lengthfixed_stmp(const lengthfixed_stmp&) = delete;
    lengthfixed_stmp& operator=(const lengthfixed_stmp&) = delete;

private:
    const rc_ptr<memory_allocator> _alloc;
    const size_t _granularity; // 粒度
    int _free_num = 0;
    void *_head = nullptr;
};

/**
 * 定长内存池(length fixed granularity memory pool)
 */
class NUT_API lengthfixed_mtmp : public memory_allocator
{
    NUT_DEBUGGING_DESTROY_CHECKER

private:
    enum { MAX_FREE_NUM = 50 }; // 最多缓存的空闲块数

public:
    lengthfixed_mtmp(size_t granularity, memory_allocator *ma = nullptr);
    virtual ~lengthfixed_mtmp();

    bool is_empty() const;
    void clear();

    virtual void* alloc(size_t sz) override;
    virtual void* realloc(void *p, size_t old_sz, size_t new_sz) override;
    virtual void free(void *p, size_t sz) override;

private:
    // Non-copyable
    lengthfixed_mtmp(const lengthfixed_mtmp&) = delete;
    lengthfixed_mtmp& operator=(const lengthfixed_mtmp&) = delete;

private:
    const rc_ptr<memory_allocator> _alloc;
    const size_t _granularity; // 粒度
    std::atomic<int> _free_num = ATOMIC_VAR_INIT(0);
    std::atomic<void*> _head = ATOMIC_VAR_INIT(nullptr);
};

}

#endif /* head file guarder */
