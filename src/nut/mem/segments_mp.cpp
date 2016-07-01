
#include <string.h>
#include <algorithm>

#include <nut/rc/rc_new.h>

#include "segments_mp.h"

namespace nut
{

segments_mp::segments_mp(memory_allocator *ma)
    : _alloc(ma)
{
    for (size_t i = 0; i < FREE_LIST_COUNT; ++i)
        _freelists[i] = rc_new<lengthfixed_mp>(GRANULARITY * (i + 1), ma);
}

segments_mp::~segments_mp()
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    clear();
}

void segments_mp::clear()
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    for (int i = 0; i < FREE_LIST_COUNT; ++i)
        _freelists[i]->clear();
}

void* segments_mp::alloc(size_t sz)
{
    assert(sz > 0);
    NUT_DEBUGGING_ASSERT_ALIVE;

    if (sz > GRANULARITY * FREE_LIST_COUNT)
        return ma_alloc(_alloc, sz);

    const size_t idx = (sz - 1) / GRANULARITY;
    return _freelists[idx]->alloc(GRANULARITY * (idx + 1));
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

    if (_freelists[new_idx]->is_empty())
        return ma_realloc(_alloc, p, GRANULARITY * (old_idx + 1), GRANULARITY * (new_idx + 1));

    void *ret = _freelists[new_idx]->alloc(GRANULARITY * (new_idx + 1));
    assert(NULL != ret);
    ::memcpy(ret, p, std::min(old_sz, new_sz));
    _freelists[old_idx]->free(p, GRANULARITY * (old_idx + 1));
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
    _freelists[idx]->free(p, GRANULARITY * (idx + 1));
}

}
