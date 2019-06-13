
#include <algorithm>

#include "../platform/platform.h"
#include "lengthfixed_mp.h"


namespace nut
{

lengthfixed_stmp::lengthfixed_stmp(size_t granularity, memory_allocator *ma) noexcept
    : _alloc(ma), _granularity(std::max<size_t>(granularity, sizeof(void*)))
{}

lengthfixed_stmp::~lengthfixed_stmp() noexcept
{
    clear();
}

bool lengthfixed_stmp::is_empty() const noexcept
{
    return 0 == _free_num;
}

void lengthfixed_stmp::clear() noexcept
{
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

void* lengthfixed_stmp::alloc(size_t sz) noexcept
{
    assert(_granularity == std::max<size_t>(sz, sizeof(void*)));

    if (nullptr == _head)
        return ma_alloc(_alloc, _granularity);

    void *p = _head;
    _head = *reinterpret_cast<void**>(_head);
    --_free_num;
    return p;
}

void* lengthfixed_stmp::realloc(void *p, size_t old_sz, size_t new_sz) noexcept
{
    assert(nullptr != p && _granularity == old_sz && _granularity == new_sz);
    UNUSED(old_sz);
    UNUSED(new_sz);
    return p;
}

void lengthfixed_stmp::free(void *p, size_t sz) noexcept
{
    assert(nullptr != p && _granularity == std::max<size_t>(sz, sizeof(void*)));

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

lengthfixed_mtmp::lengthfixed_mtmp(size_t granularity, memory_allocator *ma) noexcept
    : _alloc(ma), _granularity(std::max<size_t>(granularity, sizeof(void*)))
{}

lengthfixed_mtmp::~lengthfixed_mtmp() noexcept
{
    clear();
}

bool lengthfixed_mtmp::is_empty() const noexcept
{
    return 0 == _free_num.load(std::memory_order_relaxed);
}

void lengthfixed_mtmp::clear() noexcept
{
    void *p = _head.exchange(nullptr, std::memory_order_relaxed);
    while (nullptr != p)
    {
        void *next = *reinterpret_cast<void**>(p);
        ma_free(_alloc, p, _granularity);
        _free_num.fetch_sub(1, std::memory_order_relaxed);
        p = next;
    }
}

void* lengthfixed_mtmp::alloc(size_t sz) noexcept
{
    assert(_granularity == std::max<size_t>(sz, sizeof(void*)));

    void *old_head = _head.load(std::memory_order_acquire);
    while (nullptr != old_head && !_head.compare_exchange_weak(
               old_head, *reinterpret_cast<void**>(old_head),
               std::memory_order_relaxed, std::memory_order_acquire))
    {}

    if (nullptr == old_head)
        return ma_alloc(_alloc, _granularity);

    _free_num.fetch_sub(1, std::memory_order_relaxed);
    return old_head;
}

void* lengthfixed_mtmp::realloc(void *p, size_t old_sz, size_t new_sz) noexcept
{
    assert(nullptr != p && _granularity == old_sz && _granularity == new_sz);
    UNUSED(old_sz);
    UNUSED(new_sz);
    return p;
}

void lengthfixed_mtmp::free(void *p, size_t sz) noexcept
{
    assert(nullptr != p && _granularity == std::max<size_t>(sz, sizeof(void*)));

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
