/**
 * @file -
 * @author jingqi
 * @date 2012-04-05
 * @last-edit 2015-01-06 22:28:46 jingqi
 */

#ifndef ___HEADFILE_635293EA_4990_413E_A43D_3868BE4E99C7_
#define ___HEADFILE_635293EA_4990_413E_A43D_3868BE4E99C7_

#include <assert.h>
#include <stdlib.h> // for malloc() and so on
#include <stdint.h>

#include <nut/threading/lockfree/atomic.hpp>
#include <nut/debugging/destroy_checker.hpp>

namespace nut
{

/**
 * 系统内存分配器(system memory allocator)
 */
class sys_ma
{
    int volatile m_ref_count;

#ifndef NDEBUG
    enum { LEFT_TAG = 0x87A4B4C4, RIGHT_TAG = 0x15BF0D3C };
    size_t m_alloc_count, m_free_count;
    size_t m_total_alloc_cb, m_total_free_cb;
#endif
    NUT_DEBUGGING_DESTROY_CHECKER

private:
    explicit sys_ma(const sys_ma&);
    sys_ma& operator=(const sys_ma&);

    sys_ma()
        : m_ref_count(0)
#ifndef NDEBUG
        , m_alloc_count(0), m_free_count(0), m_total_alloc_cb(0), m_total_free_cb(0)
#endif
    {}

    virtual ~sys_ma()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;

#ifndef NDEBUG
        assert(m_alloc_count == m_free_count);
        assert(m_total_alloc_cb == m_total_free_cb);
#endif
    }

public:
    static sys_ma* create()
    {
        sys_ma *const ret = (sys_ma*) ::malloc(sizeof(sys_ma));
        assert(NULL != ret);
        new (ret) sys_ma;
        ret->add_ref();
        return ret;
    }

    int add_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return atomic_add(&m_ref_count, 1) + 1;
    }

    int rls_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        const int ret = atomic_add(&m_ref_count, -1) - 1;
        if (0 == ret)
        {
            this->~sys_ma();
            ::free(this);
        }
        return ret;
    }

    int get_ref() const
    {
        return m_ref_count;
    }

public:
    void* alloc(size_t cb)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
#ifndef NDEBUG
        const size_t total_cb = cb + sizeof(uint32_t) * 3;
        void* ret = ::malloc(total_cb);
        assert(NULL != ret);
        *(uint32_t*) ret = cb;
        ((uint32_t*) ret)[1] = LEFT_TAG;
        *(uint32_t*) (((uint8_t*) ret) + sizeof(uint32_t) * 2 + cb) = RIGHT_TAG;
        ++m_alloc_count;
        m_total_alloc_cb += cb;
        return ((uint32_t*) ret) + 2;
#else
        return ::malloc(cb);
#endif
    }

    void* realloc(void *p, size_t new_cb)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
#ifndef NDEBUG
        assert(NULL != p);
        const size_t cb = ((uint32_t*) p)[-2];
        assert(LEFT_TAG == ((uint32_t*) p)[-1]);
        assert(RIGHT_TAG == *(uint32_t*)(((uint8_t*) p) + cb));

        const size_t total_cb = new_cb + sizeof(uint32_t) * 3;
        void *ret = ::realloc(((uint32_t*) p) - 2, total_cb);
        assert(NULL != ret);
        *(uint32_t*) ret = new_cb;
        ((uint32_t*) ret)[1] = LEFT_TAG;
        *(uint32_t*) (((uint8_t*) ret) + sizeof(uint32_t) * 2 + new_cb) = RIGHT_TAG;
        ++m_alloc_count;
        ++m_free_count;
        m_total_alloc_cb += new_cb;
        m_total_free_cb += cb;
        return ((uint32_t*) ret) + 2;
#else
        return ::realloc(p, new_cb);
#endif
    }

    void free(void *p)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
#ifndef NDEBUG
        assert(NULL != p);
        const size_t cb = ((uint32_t*) p)[-2];
        assert(LEFT_TAG == ((uint32_t*) p)[-1]);
        assert(RIGHT_TAG == *(uint32_t*)(((uint8_t*) p) + cb));
        ::free(((uint32_t*) p) - 2);
        ++m_free_count;
        m_total_free_cb += cb;
#else
        ::free(p);
#endif
    }

#ifndef NDEBUG
    size_t get_alloc_count() const
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return m_alloc_count;
    }

    size_t get_free_count() const
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return m_free_count;
    }

    size_t get_total_alloc_size() const
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return m_total_alloc_cb;
    }

    size_t get_total_free_size() const
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return m_total_free_cb;
    }
#endif
};

template <typename MemAlloc>
void* ma_alloc(MemAlloc *ma, size_t cb)
{
    if (NULL != ma)
        return ma->alloc(cb);
    else
        return ::malloc(cb);
}

template <typename MemAlloc>
void* ma_realloc(MemAlloc *ma, void *p, size_t cb)
{
    if (NULL != ma)
        return ma->realloc(p, cb);
    else
        return ::realloc(p, cb);
}

template <typename MemAlloc>
void ma_free(MemAlloc *ma, void *p)
{
    if (NULL != ma)
        ma->free(p);
    else
        ::free(p);
}

}

#endif /* head file guarder */
