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

inline void* ma_alloc(memory_allocator *ma, size_t cb)
{
    if (NULL != ma)
        return ma->alloc(cb);
    else
        return ::malloc(cb);
}

inline void* ma_realloc(memory_allocator *ma, void *p, size_t cb)
{
    if (NULL != ma)
        return ma->realloc(p, cb);
    else
        return ::realloc(p, cb);
}

inline void ma_free(memory_allocator *ma, void *p)
{
    if (NULL != ma)
        ma->free(p);
    else
        ::free(p);
}

}

// 使用 C99 的宏可变参数形式
#define MA_NEW(ma, type, ...) (new ((type*) nut::ma_alloc((ma), sizeof(type))) (type)(__VA_ARGS__))

#define MA_DELETE(ma, p, type) \
    do \
    { \
        (p)->~type(); \
        nut::ma_free(ma, (p)); \
    } while (false)

#endif
