
#include <assert.h>
#include <stdlib.h> // for malloc() and so on
#include <string.h> // for memset()

#include "../debugging/destroy_checker.h"
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
    assert(_total_alloc_sz == _total_free_sz);
}
#endif

void* sys_ma::alloc(size_t sz)
{
    assert(sz > 0);
    NUT_DEBUGGING_ASSERT_ALIVE;

#ifndef NDEBUG
    const size_t total_sz = sz + sizeof(uint32_t) * 3;
    void* ret = ::malloc(total_sz);
    assert(nullptr != ret);
    *(uint32_t*) ret = (uint32_t) sz;
    ((uint32_t*) ret)[1] = _left_tag;
    *(uint32_t*) (((uint8_t*) ret) + sizeof(uint32_t) * 2 + sz) = _right_tag;
    ::memset(((uint32_t*) ret) + 2, UNINIT_BYTE, sz);
    ++_alloc_count;
    _total_alloc_sz += sz;
    return ((uint32_t*) ret) + 2;
#else
    return ::malloc(sz);
#endif
}

void* sys_ma::realloc(void *p, size_t old_sz, size_t new_sz)
{
    assert(nullptr != p && old_sz > 0 && new_sz > 0);
    NUT_DEBUGGING_ASSERT_ALIVE;

#ifndef NDEBUG
    const size_t rec_old_sz = ((uint32_t*) p)[-2];
    assert(rec_old_sz == old_sz);
    assert(_left_tag == ((uint32_t*) p)[-1]);
    assert(_right_tag == *(uint32_t*)(((uint8_t*) p) + old_sz));
    ((uint32_t*) p)[-1] = 0;
    *(uint32_t*)(((uint8_t*) p) + old_sz) = 0;

    const size_t total_sz = new_sz + sizeof(uint32_t) * 3;
    void *ret = ::realloc(((uint32_t*) p) - 2, total_sz);
    assert(nullptr != ret);
    *(uint32_t*) ret = (uint32_t) new_sz;
    ((uint32_t*) ret)[1] = _left_tag;
    *(uint32_t*) (((uint8_t*) ret) + sizeof(uint32_t) * 2 + new_sz) = _right_tag;
    if (new_sz > old_sz)
        ::memset(((uint8_t*) ret) + sizeof(uint32_t) * 2 + old_sz, UNINIT_BYTE, new_sz - old_sz);
    ++_free_count;
    ++_alloc_count;
    _total_free_sz += old_sz;
    _total_alloc_sz += new_sz;
    return ((uint32_t*) ret) + 2;
#else
    return ::realloc(p, new_sz);
#endif
}

void sys_ma::free(void *p, size_t sz)
{
    assert(nullptr != p && sz > 0);
    NUT_DEBUGGING_ASSERT_ALIVE;

#ifndef NDEBUG
    const size_t rec_sz = ((uint32_t*) p)[-2];
    assert(rec_sz == sz);
    assert(_left_tag == ((uint32_t*) p)[-1]);
    assert(_right_tag == *(uint32_t*)(((uint8_t*) p) + sz));
    ((uint32_t*) p)[-1] = 0;
    *(uint32_t*)(((uint8_t*) p) + sz) = 0;
    ::memset(p, FREED_BYTE, sz);
    ::free(((uint32_t*) p) - 2);
    ++_free_count;
    _total_free_sz += sz;
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
    return _total_alloc_sz;
}

size_t sys_ma::get_total_free_size() const
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    return _total_free_sz;
}
#endif

}
