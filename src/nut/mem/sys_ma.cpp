
#include <assert.h>
#include <stdlib.h> // for malloc() and so on
#include <string.h> // for memset()

#include <nut/debugging/destroy_checker.h>

#include "sys_ma.h"

namespace nut
{

#ifndef NDEBUG
sys_ma::sys_ma()
    : m_left_tag(0), m_right_tag(0), m_alloc_count(0), m_free_count(0),
      m_total_alloc_cb(0), m_total_free_cb(0)
{
    for (size_t i = 0; i < sizeof(m_left_tag); ++i)
        reinterpret_cast<uint8_t*>(&m_left_tag)[i] = (uint8_t) ::rand();
    for (size_t i = 0; i < sizeof(m_right_tag); ++i)
        reinterpret_cast<uint8_t*>(&m_right_tag)[i] = (uint8_t) ::rand();
}
#else
sys_ma::sys_ma()
{}
#endif

#ifndef NDEBUG
sys_ma::~sys_ma()
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    assert(m_alloc_count == m_free_count);
    assert(m_total_alloc_cb == m_total_free_cb);
}
#endif

void* sys_ma::alloc(size_t cb)
{
#ifndef NDEBUG
    NUT_DEBUGGING_ASSERT_ALIVE;
    const size_t total_cb = cb + sizeof(uint32_t) * 3;
    void* ret = ::malloc(total_cb);
    assert(NULL != ret);
    *(uint32_t*) ret = (uint32_t) cb;
    ((uint32_t*) ret)[1] = m_left_tag;
    *(uint32_t*) (((uint8_t*) ret) + sizeof(uint32_t) * 2 + cb) = m_right_tag;
    ::memset(((uint32_t*) ret) + 2, 0xCC, cb);
    ++m_alloc_count;
    m_total_alloc_cb += cb;
    return ((uint32_t*) ret) + 2;
#else
    return ::malloc(cb);
#endif
}

void* sys_ma::realloc(void *p, size_t new_cb)
{
#ifndef NDEBUG
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(NULL != p);
    const size_t cb = ((uint32_t*) p)[-2];
    assert(m_left_tag == ((uint32_t*) p)[-1]);
    assert(m_right_tag == *(uint32_t*)(((uint8_t*) p) + cb));
    ((uint32_t*) p)[-1] = 0;
    *(uint32_t*)(((uint8_t*) p) + cb) = 0;

    const size_t total_cb = new_cb + sizeof(uint32_t) * 3;
    void *ret = ::realloc(((uint32_t*) p) - 2, total_cb);
    assert(NULL != ret);
    *(uint32_t*) ret = (uint32_t) new_cb;
    ((uint32_t*) ret)[1] = m_left_tag;
    *(uint32_t*) (((uint8_t*) ret) + sizeof(uint32_t) * 2 + new_cb) = m_right_tag;
    if (new_cb > cb)
        ::memset(((uint8_t*) ret) + sizeof(uint32_t) * 2 + cb, 0xCC, new_cb - cb);
    ++m_free_count;
    ++m_alloc_count;
    m_total_free_cb += cb;
    m_total_alloc_cb += new_cb;
    return ((uint32_t*) ret) + 2;
#else
    return ::realloc(p, new_cb);
#endif
}

void sys_ma::free(void *p)
{
#ifndef NDEBUG
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(NULL != p);
    const size_t cb = ((uint32_t*) p)[-2];
    assert(m_left_tag == ((uint32_t*) p)[-1]);
    assert(m_right_tag == *(uint32_t*)(((uint8_t*) p) + cb));
    ((uint32_t*) p)[-1] = 0;
    *(uint32_t*)(((uint8_t*) p) + cb) = 0;
    ::memset(p, 0xFE, cb);
    ::free(((uint32_t*) p) - 2);
    ++m_free_count;
    m_total_free_cb += cb;
#else
    ::free(p);
#endif
}

#ifndef NDEBUG
size_t sys_ma::get_alloc_count() const
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    return m_alloc_count;
}

size_t sys_ma::get_free_count() const
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    return m_free_count;
}

size_t sys_ma::get_total_alloc_size() const
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    return m_total_alloc_cb;
}

size_t sys_ma::get_total_free_size() const
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    return m_total_free_cb;
}
#endif

}
