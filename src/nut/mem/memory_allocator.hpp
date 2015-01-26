/**
 * @file -
 * @author jingqi
 * @date 2015-01-26
 * @last-edit
 * @brief
 */

#ifndef ___HEADFILE_8FCF356E_92C5_457D_A13B_AB79C21B9E4D_
#define ___HEADFILE_8FCF356E_92C5_457D_A13B_AB79C21B9E4D_

#include <nut/gc/ref.hpp>

namespace nut
{

class memory_allocator
{
    memory_allocator(const memory_allocator&);
    memory_allocator& operator=(const memory_allocator&);

public:
    NUT_GC_REFERABLE

    memory_allocator() {}

    virtual void* alloc(size_t cb) = 0;
    virtual void* realloc(void *p, size_t cb) = 0;
    virtual void free(void *p) = 0;
};

};

#endif
