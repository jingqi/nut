
#ifndef ___HEADFILE_DE2E0013_ABB5_49D4_AB22_F859CCAFCFA5_
#define ___HEADFILE_DE2E0013_ABB5_49D4_AB22_F859CCAFCFA5_

#include <string.h>
#include <algorithm>

#include "../rc/rc_new.h"
#include "../debugging/destroy_checker.h"
#include "lengthfixed_mp.h"


namespace nut
{

/**
 * 分段长度内存池(segmental granularity memory pool)
 */
template <typename lengthfixed_mp_type>
class segments_mp : public memory_allocator
{
private:
    // 粒度
    static constexpr size_t GRANULARITY = 8;
    static_assert(GRANULARITY >= sizeof(void*), "Granularity should greater then or equal to a pointer");

    // free list 数
    static constexpr size_t FREE_LIST_COUNT = 128;

public:
    segments_mp(memory_allocator *ma = nullptr) noexcept
        : _alloc(ma)
    {
        for (size_t i = 0; i < FREE_LIST_COUNT; ++i)
            _freelists[i] = rc_new<lengthfixed_mp_type>(GRANULARITY * (i + 1), ma);
    }

    ~segments_mp() noexcept
    {
        NUT_DEBUGGING_ASSERT_ALIVE;

        clear();
    }

    void clear() noexcept
    {
        NUT_DEBUGGING_ASSERT_ALIVE;

        for (size_t i = 0; i < FREE_LIST_COUNT; ++i)
            _freelists[i]->clear();
    }

    virtual void* alloc(size_t sz) noexcept override
    {
        assert(sz > 0);
        NUT_DEBUGGING_ASSERT_ALIVE;

        if (sz > GRANULARITY * FREE_LIST_COUNT)
            return ma_alloc(_alloc, sz);

        const size_t idx = (sz - 1) / GRANULARITY;
        return _freelists[idx]->alloc(GRANULARITY * (idx + 1));
    }

    virtual void* realloc(void *p, size_t old_sz, size_t new_sz) noexcept override
    {
        assert(nullptr != p && old_sz > 0 && new_sz > 0);
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
        assert(nullptr != ret);
        ::memcpy(ret, p, std::min<size_t>(old_sz, new_sz));
        _freelists[old_idx]->free(p, GRANULARITY * (old_idx + 1));
        return ret;
    }

    virtual void free(void *p, size_t sz) noexcept override
    {
        assert(nullptr != p);
        NUT_DEBUGGING_ASSERT_ALIVE;

        if (sz > GRANULARITY * FREE_LIST_COUNT)
        {
            ma_free(_alloc, p, sz);
            return;
        }

        const size_t idx = (sz - 1) / GRANULARITY;
        _freelists[idx]->free(p, GRANULARITY * (idx + 1));
    }

private:
    segments_mp(const segments_mp&) = delete;
    segments_mp& operator=(const segments_mp&) = delete;

private:
    const rc_ptr<memory_allocator> _alloc;
    rc_ptr<lengthfixed_mp_type> _freelists[FREE_LIST_COUNT];

    NUT_DEBUGGING_DESTROY_CHECKER
};

typedef segments_mp<lengthfixed_stmp> segments_stmp;
typedef segments_mp<lengthfixed_mtmp> segments_mtmp;

}

#endif
