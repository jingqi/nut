
#include <algorithm>

#include <nut/platform/platform.h>

#include "lengthfixed_mp.h"

namespace nut
{

lengthfixed_stmp::lengthfixed_stmp(size_t granularity, memory_allocator *ma)
    : _alloc(ma), _granularity((std::max)(granularity, sizeof(void*)))
{}

lengthfixed_stmp::~lengthfixed_stmp()
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    clear();
}

bool lengthfixed_stmp::is_empty() const
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    return 0 == _free_num;
}

void lengthfixed_stmp::clear()
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    void *p = _head;
    while (nullptr != p)
    {
        void *next = *reinterpret_cast<void**>(p);
        ma_free(_alloc, p, _granularity);
        p = next;
    }
    _head = nullptr;
    _free_num = 0;
}

void* lengthfixed_stmp::alloc(size_t sz)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(_granularity == (std::max)(sz, sizeof(void*)));

    if (nullptr == _head)
        return ma_alloc(_alloc, _granularity);

    void *p = _head;
    _head = *reinterpret_cast<void**>(_head);
    --_free_num;
    return p;
}

void* lengthfixed_stmp::realloc(void *p, size_t old_sz, size_t new_sz)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(nullptr != p && _granularity == old_sz && _granularity == new_sz);
    UNUSED(old_sz);
    UNUSED(new_sz);
    return p;
}

void lengthfixed_stmp::free(void *p, size_t sz)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(nullptr != p && _granularity == (std::max)(sz, sizeof(void*)));

    if (_free_num >= (int) MAX_FREE_NUM)
    {
        ma_free(_alloc, p, _granularity);
        return;
    }

    *reinterpret_cast<void**>(p) = _head;
    _head = p;
    ++_free_num;
}

}

namespace nut
{

lengthfixed_mtmp::lengthfixed_mtmp(size_t granularity, memory_allocator *ma)
    : _alloc(ma), _granularity((std::max)(granularity, sizeof(void*)))
{}

lengthfixed_mtmp::~lengthfixed_mtmp()
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    clear();
}

bool lengthfixed_mtmp::is_empty() const
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    return 0 == _free_num.load(std::memory_order_relaxed);
}

void lengthfixed_mtmp::clear()
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    void *old_head = _head.load(std::memory_order_relaxed);
    while (!_head.compare_exchange_weak(
               old_head, nullptr,
               std::memory_order_release, std::memory_order_relaxed))
    {}

    while (nullptr != old_head)
    {
        void *next = *reinterpret_cast<void**>(old_head);
        ma_free(_alloc, old_head, _granularity);
        _free_num.fetch_sub(1, std::memory_order_relaxed);
        old_head = next;
    }
}

void* lengthfixed_mtmp::alloc(size_t sz)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(_granularity == (std::max)(sz, sizeof(void*)));

    void *old_head = _head.load(std::memory_order_relaxed);
    while (nullptr != old_head && !_head.compare_exchange_weak(
               old_head, *reinterpret_cast<void**>(old_head),
               std::memory_order_release, std::memory_order_relaxed))
    {}

    if (nullptr == old_head)
        return ma_alloc(_alloc, _granularity);

    _free_num.fetch_sub(1, std::memory_order_relaxed);
    return old_head;
}

void* lengthfixed_mtmp::realloc(void *p, size_t old_sz, size_t new_sz)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(nullptr != p && _granularity == old_sz && _granularity == new_sz);
    UNUSED(old_sz);
    UNUSED(new_sz);
    return p;
}

void lengthfixed_mtmp::free(void *p, size_t sz)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(nullptr != p && _granularity == (std::max)(sz, sizeof(void*)));

    if (_free_num.load(std::memory_order_relaxed) >= (int) MAX_FREE_NUM)
    {
        ma_free(_alloc, p, _granularity);
        return;
    }

    *reinterpret_cast<void**>(p) = _head.load(std::memory_order_relaxed);
    while (!_head.compare_exchange_weak(
               *reinterpret_cast<void**>(p), p,
               std::memory_order_release, std::memory_order_relaxed))
    {}

    _free_num.fetch_add(1, std::memory_order_relaxed);
}

}
