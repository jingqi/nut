
#include <algorithm>

#include "lengthfixed_mp.h"

namespace nut
{

lengthfixed_mp::lengthfixed_mp(size_t granularity, memory_allocator *ma)
    : _alloc(ma), _granularity(std::max(granularity, sizeof(void*)))
{}

lengthfixed_mp::~lengthfixed_mp()
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    clear();
}

void lengthfixed_mp::clear()
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    void *p = _head.ptr;
    while (NULL != p)
    {
        void *next = *reinterpret_cast<void**>(p);
        ma_free(_alloc, p, _granularity);
        p = next;
    }
    _head.ptr = NULL;
    _free_num = 0;
}

void* lengthfixed_mp::alloc(size_t sz)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(_granularity == std::max(sz, sizeof(void*)));

    while (true)
    {
        const TagedPtr<void> old_head(_head.cas);

        if (NULL == old_head.ptr)
            return ma_alloc(_alloc, _granularity);

        void *next = *reinterpret_cast<void**>(old_head.ptr);
        const TagedPtr<void> new_head(next, old_head.tag + 1);
        if (atomic_cas(&(_head.cas), old_head.cas, new_head.cas))
        {
			_free_num = std::max(0, _free_num - 1); // NOTE _free_num 在多线程下并不可靠
            return old_head.ptr;
        }
    }
}

void* lengthfixed_mp::realloc(void *p, size_t old_sz, size_t new_sz)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(NULL != p && _granularity == old_sz && _granularity == new_sz);
    return p;
}

void lengthfixed_mp::free(void *p, size_t sz)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(NULL != p && _granularity == std::max(sz, sizeof(void*)));

    while(true)
    {
        if (_free_num >= (int) MAX_FREE_NUM) // NOTE _free_num 只起参考作用
        {
            ma_free(_alloc, p, _granularity);
            return;
        }

        const TagedPtr<void> old_head(_head.cas);
        *reinterpret_cast<void**>(p) = old_head.ptr;
        const TagedPtr<void> new_head(p, old_head.tag + 1);
        if (atomic_cas(&(_head.cas), old_head.cas, new_head.cas))
        {
			// NOTE _free_num 在多线程下并不可靠
			if (NULL == old_head.ptr)
				_free_num = 1;
			else
				++_free_num;
            return;
        }
    }
}

}
