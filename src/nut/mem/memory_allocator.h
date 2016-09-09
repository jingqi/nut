
#ifndef ___HEADFILE_8FCF356E_92C5_457D_A13B_AB79C21B9E4D_
#define ___HEADFILE_8FCF356E_92C5_457D_A13B_AB79C21B9E4D_

#include <assert.h>
#include <stdlib.h>
#include <utility>

#include <nut/nut_config.h>
#include <nut/rc/rc_ptr.h>

namespace nut
{

class NUT_API memory_allocator
{
    memory_allocator(const memory_allocator&);
    memory_allocator& operator=(const memory_allocator&);

public:
    NUT_REF_COUNTABLE

    memory_allocator() {}
    virtual ~memory_allocator() {}

    /**
     * @param sz Should greater than 0
     */
    virtual void* alloc(size_t sz) = 0;

    /**
     * @param p Should not be NULL
     * @param new_sz Should greater then 0
     * @param old_sz Should greater then 0
     */
    virtual void* realloc(void *p, size_t old_sz, size_t new_sz) = 0;

    /**
     * @param p Should not be NULL
     * @param sz Should greater then 0
     */
    virtual void free(void *p, size_t sz) = 0;
};


inline void* ma_alloc(memory_allocator *ma, size_t sz)
{
    assert(sz > 0);
    if (NULL == ma)
        return ::malloc(sz);
    return ma->alloc(sz);
}

inline void* ma_realloc(memory_allocator *ma, void *p, size_t old_sz, size_t new_sz)
{
    assert(NULL != p && old_sz > 0 && new_sz > 0);
    if (NULL == ma)
        return ::realloc(p, new_sz);
    return ma->realloc(p, old_sz, new_sz);
}

inline void ma_free(memory_allocator *ma, void *p, size_t sz)
{
    assert(NULL != p && sz > 0);
    if (NULL == ma)
        return ::free(p);
    return ma->free(p, sz);
}

}

#endif
