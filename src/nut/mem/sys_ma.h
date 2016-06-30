
#ifndef ___HEADFILE_635293EA_4990_413E_A43D_3868BE4E99C7_
#define ___HEADFILE_635293EA_4990_413E_A43D_3868BE4E99C7_

#include <assert.h>
#include <stdint.h>

#include <nut/debugging/destroy_checker.h>

#include "memory_allocator.h"

namespace nut
{

/**
 * 系统内存分配器(system memory allocator)
 */
class sys_ma : public memory_allocator
{

#ifndef NDEBUG
    uint32_t _left_tag = 0, _right_tag = 0;
    size_t _alloc_count = 0, _free_count = 0;
    size_t _total_alloc_sz = 0, _total_free_sz = 0;
    NUT_DEBUGGING_DESTROY_CHECKER
#endif

private:
    explicit sys_ma(const sys_ma&);
    sys_ma& operator=(const sys_ma&);

public:
    sys_ma();

#ifndef NDEBUG
    ~sys_ma();
#endif

public:
    virtual void* alloc(size_t sz) override;
    virtual void* realloc(void *p, size_t old_sz, size_t new_sz) override;
    virtual void free(void *p, size_t sz) override;

#ifndef NDEBUG
    size_t get_alloc_count() const;
    size_t get_free_count() const;

    size_t get_total_alloc_size() const;
    size_t get_total_free_size() const;
#endif
};

}

#endif /* head file guarder */
