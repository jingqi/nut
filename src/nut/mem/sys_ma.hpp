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

namespace nut
{

/**
 * 系统内存分配器(system memory allocator)
 */
class sys_ma
{
#ifndef NDEBUG
    enum { LEFT_TAG = 0x87A4B4C4, RIGHT_TAG = 0x15BF0D3C };
    size_t m_alloc_count, m_free_count;
    size_t m_total_alloc_cb, m_total_free_cb;
#endif
    int volatile m_ref_count;

private:
    explicit sys_ma(const sys_ma&);
    sys_ma& operator=(const sys_ma&);

    sys_ma()
        : m_ref_count(0)
    {}

    virtual ~sys_ma() {}

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
        return atomic_add(&m_ref_count, 1) + 1;
    }

    int rls_ref()
    {
        const int ret = atomic_add(&m_ref_count, -1) - 1;
        if (0 == ret)
        {
            this->~sys_ma();
            ::free(this);
        }
        return ret;
    }

public:
    void* alloc(size_t cb)
    {
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
        return m_alloc_count;
    }

    size_t get_free_count() const
    {
        return m_free_count;
    }

    size_t get_total_alloc_size() const
    {
        return m_total_alloc_cb;
    }

    size_t get_total_free_size() const
    {
        return m_total_free_cb;
    }
#endif
};

}

#endif /* head file guarder */
