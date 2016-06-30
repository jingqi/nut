
#include <string.h>

#include "segments_mp.h"

namespace nut
{

segments_mp::segments_mp(memory_allocator *ma)
    : _alloc(ma)
{}

segments_mp::~segments_mp()
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    clear();
}

void segments_mp::clear()
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    for (int i = 0; i < FREE_LIST_COUNT; ++i)
    {
        void *block = _freelist[i].head.ptr;
        while (NULL != block)
        {
            void *next = *reinterpret_cast<void**>(block);
            ma_free(_alloc, block, GRANULARITY * (i + 1));
            block = next;
        }
        _freelist[i].head.ptr = NULL;
        _freelist[i].length = 0;
    }
}

void* segments_mp::alloc(size_t sz)
{
    assert(sz > 0);
    NUT_DEBUGGING_ASSERT_ALIVE;

    if (sz > GRANULARITY * FREE_LIST_COUNT)
        return ma_alloc(_alloc, sz);

    const size_t idx = (sz - 1) / GRANULARITY;
    while (true)
    {
        const TagedPtr<void> old_head(_freelist[idx].head);

        if (NULL == old_head.ptr)
            return ma_alloc(_alloc, GRANULARITY * (idx + 1));

        void *next = *reinterpret_cast<void**>(old_head.ptr);
        const TagedPtr<void> new_head(next, old_head.tag + 1);
        if (atomic_cas(&(_freelist[idx].head.cas), old_head.cas, new_head.cas))
        {
            atomic_add(&(_freelist[idx].length), -1);
            return old_head.ptr;
        }
    }
}

void* segments_mp::realloc(void *p, size_t old_sz, size_t new_sz)
{
    assert(NULL != p && old_sz > 0 && new_sz > 0);
    NUT_DEBUGGING_ASSERT_ALIVE;
    
    if (old_sz > GRANULARITY * FREE_LIST_COUNT && new_sz > GRANULARITY * FREE_LIST_COUNT)
        return ma_realloc(_alloc, p, old_sz, new_sz);

    const size_t new_idx = (new_sz - 1) / GRANULARITY;
    if (old_sz > GRANULARITY * FREE_LIST_COUNT) // Shrink size
        return ma_realloc(_alloc, p, old_sz, GRANULARITY * (new_idx + 1));

    const size_t old_idx = (old_sz - 1) / GRANULARITY;
    if (new_sz > GRANULARITY * FREE_LIST_COUNT) // Enlarge size
        return ma_realloc(_alloc, p, GRANULARITY * (old_idx + 1), new_sz);

    if (old_idx == new_idx)
        return p;

    void *ret = segments_mp::alloc(new_sz);
    assert(NULL != ret);
    ::memcpy(ret, p, (old_sz < new_sz ? old_sz : new_sz));
    segments_mp::free(p, old_sz);
    return ret;
}

void segments_mp::free(void *p, size_t sz)
{
    assert(NULL != p);
    NUT_DEBUGGING_ASSERT_ALIVE;

    if (sz > GRANULARITY * FREE_LIST_COUNT)
    {
        ma_free(_alloc, p, sz);
        return;
    }
    
    const size_t idx = (sz - 1) / GRANULARITY;
    while (true)
    {
        if (_freelist[idx].length > MAX_FREE_LIST_LENGTH)
        {
            ma_free(_alloc, p, GRANULARITY * (idx + 1));
            return;
        }
        
        const TagedPtr<void> old_head(_freelist[idx].head);
        *reinterpret_cast<void**>(p) = old_head.ptr;
        const TagedPtr<void> new_head(p, old_head.tag + 1);
        if (atomic_cas(&(_freelist[idx].head.cas), old_head.cas, new_head.cas))
        {
            atomic_add(&(_freelist[idx].length), 1);
            return;
        }
    }
}

}
