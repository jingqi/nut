
#include <assert.h>
#include <stdlib.h> // for malloc() and so on
#include <string.h> // for memset()

#include <nut/debugging/destroy_checker.h>

#include "sys_ma.h"

#define UNINIT_BYTE 0xCC
#define FREED_BYTE 0xFE

namespace nut
{

sys_ma::sys_ma()
{
#ifndef NDEBUG
    for (size_t i = 0; i < sizeof(_left_tag); ++i)
        reinterpret_cast<uint8_t*>(&_left_tag)[i] = (uint8_t) ::rand();
    for (size_t i = 0; i < sizeof(_right_tag); ++i)
        reinterpret_cast<uint8_t*>(&_right_tag)[i] = (uint8_t) ::rand();
#endif
}

#ifndef NDEBUG
sys_ma::~sys_ma()
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    assert(_alloc_count == _free_count);
    assert(_total_alloc_cb == _total_free_cb);
}
#endif

void* sys_ma::realloc(void *p, size_t cb)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(cb > 0);

#ifndef NDEBUG
    if (NULL == p)
    {
        const size_t total_cb = cb + sizeof(uint32_t) * 3;
        void* ret = ::malloc(total_cb);
        assert(NULL != ret);
        *(uint32_t*) ret = (uint32_t) cb;
        ((uint32_t*) ret)[1] = _left_tag;
        *(uint32_t*) (((uint8_t*) ret) + sizeof(uint32_t) * 2 + cb) = _right_tag;
        ::memset(((uint32_t*) ret) + 2, UNINIT_BYTE, cb);
        ++_alloc_count;
        _total_alloc_cb += cb;
        return ((uint32_t*) ret) + 2;
    }
    else
    {
        const size_t old_cb = ((uint32_t*) p)[-2];
        assert(_left_tag == ((uint32_t*) p)[-1]);
        assert(_right_tag == *(uint32_t*)(((uint8_t*) p) + old_cb));
        ((uint32_t*) p)[-1] = 0;
        *(uint32_t*)(((uint8_t*) p) + old_cb) = 0;

        const size_t total_cb = cb + sizeof(uint32_t) * 3;
        void *ret = ::realloc(((uint32_t*) p) - 2, total_cb);
        assert(NULL != ret);
        *(uint32_t*) ret = (uint32_t) cb;
        ((uint32_t*) ret)[1] = _left_tag;
        *(uint32_t*) (((uint8_t*) ret) + sizeof(uint32_t) * 2 + cb) = _right_tag;
        if (cb > old_cb)
            ::memset(((uint8_t*) ret) + sizeof(uint32_t) * 2 + old_cb, UNINIT_BYTE, cb - old_cb);
        ++_free_count;
        ++_alloc_count;
        _total_free_cb += old_cb;
        _total_alloc_cb += cb;
        return ((uint32_t*) ret) + 2;
    }
#else
    return ::realloc(p, cb);
#endif
}

void sys_ma::free(void *p)
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(NULL != p);

#ifndef NDEBUG
    const size_t cb = ((uint32_t*) p)[-2];
    assert(_left_tag == ((uint32_t*) p)[-1]);
    assert(_right_tag == *(uint32_t*)(((uint8_t*) p) + cb));
    ((uint32_t*) p)[-1] = 0;
    *(uint32_t*)(((uint8_t*) p) + cb) = 0;
    ::memset(p, FREED_BYTE, cb);
    ::free(((uint32_t*) p) - 2);
    ++_free_count;
    _total_free_cb += cb;
#else
    ::free(p);
#endif
}

#ifndef NDEBUG
size_t sys_ma::get_alloc_count() const
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    return _alloc_count;
}

size_t sys_ma::get_free_count() const
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    return _free_count;
}

size_t sys_ma::get_total_alloc_size() const
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    return _total_alloc_cb;
}

size_t sys_ma::get_total_free_size() const
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    return _total_free_cb;
}
#endif

}
