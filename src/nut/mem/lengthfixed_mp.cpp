
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
    return 0 == _free_num;
}

void lengthfixed_mtmp::clear()
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

void* lengthfixed_mtmp::alloc(size_t sz)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(_granularity == (std::max)(sz, sizeof(void*)));

    while (true)
    {
        void *old_head = _head;

        if (nullptr == old_head)
            return ma_alloc(_alloc, _granularity);

        void *next = *reinterpret_cast<void**>(old_head);
        if (_head.compare_exchange_weak(old_head, next))
        {
            _free_num = (std::max)(0, _free_num - 1); // NOTE _free_num 在多线程下并不可靠
            return old_head;
        }
    }
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

    while(true)
    {
        if (_free_num >= (int) MAX_FREE_NUM) // NOTE _free_num 只起参考作用
        {
            ma_free(_alloc, p, _granularity);
            return;
        }

        void *old_head = _head;
        *reinterpret_cast<void**>(p) = old_head;
        if (_head.compare_exchange_weak(old_head, p))
        {
            // NOTE _free_num 在多线程下并不可靠
            if (nullptr == old_head)
                _free_num = 1;
            else
                ++_free_num;
            return;
        }
    }
}

}
