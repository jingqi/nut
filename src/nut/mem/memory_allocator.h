
#ifndef ___HEADFILE_8FCF356E_92C5_457D_A13B_AB79C21B9E4D_
#define ___HEADFILE_8FCF356E_92C5_457D_A13B_AB79C21B9E4D_

#include <assert.h>
#include <stdlib.h>

#include <nut/rc/rc_ptr.h>

namespace nut
{

class memory_allocator
{
    memory_allocator(const memory_allocator&);
    memory_allocator& operator=(const memory_allocator&);

public:
    NUT_REF_COUNTABLE

    memory_allocator()
    {}

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

// _MSC_VER == 1700 for Visual Studio 2012
#if (defined(_MSC_VER) && _MSC_VER > 1700) || (!defined(_MSC_VER) && !defined(_LIBCPP_HAS_NO_VARIADICS))
template <typename T, typename ...Args>
T* ma_new(memory_allocator *ma, Args&& ...args)
{
    T *p = (T*) ma_alloc(ma, sizeof(T));
    assert(NULL != p);
    new (p) T(args...);
    return p;
}
#else
template <typename T>
T* ma_new(memory_allocator *ma)
{
    T *p = (T*) ma_alloc(ma, sizeof(T));
    assert(NULL != p);
    new (p) T();
    return p;
}

template <typename T, typename Arg1>
T* ma_new(memory_allocator *ma, Arg1&& arg1)
{
    T *p = (T*) ma_alloc(ma, sizeof(T));
    assert(NULL != p);
    new (p) T(arg1);
    return p;
}

template <typename T, typename Arg1, typename Arg2>
T* ma_new(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2)
{
    T *p = (T*) ma_alloc(ma, sizeof(T));
    assert(NULL != p);
    new (p) T(arg1, arg2);
    return p;
}

template <typename T, typename Arg1, typename Arg2, typename Arg3>
T* ma_new(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
{
    T *p = (T*) ma_alloc(ma, sizeof(T));
    assert(NULL != p);
    new (p) T(arg1, arg2, arg3);
    return p;
}

template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
T* ma_new(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
{
    T *p = (T*) ma_alloc(ma, sizeof(T));
    assert(NULL != p);
    new (p) T(arg1, arg2, arg3, arg4);
    return p;
}

template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
T* ma_new(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
{
    T *p = (T*) ma_alloc(ma, sizeof(T));
    assert(NULL != p);
    new (p) T(arg1, arg2, arg3, arg4, arg5);
    return p;
}

template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
          typename Arg6>
T* ma_new(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
          Arg6&& arg6)
{
    T *p = (T*) ma_alloc(ma, sizeof(T));
    assert(NULL != p);
    new (p) T(arg1, arg2, arg3, arg4, arg5, arg6);
    return p;
}

template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
          typename Arg6, typename Arg7>
T* ma_new(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
          Arg6&& arg6, Arg7&& arg7)
{
    T *p = (T*) ma_alloc(ma, sizeof(T));
    assert(NULL != p);
    new (p) T(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    return p;
}

template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
          typename Arg6, typename Arg7, typename Arg8>
T* ma_new(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
          Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
{
    T *p = (T*) ma_alloc(ma, sizeof(T));
    assert(NULL != p);
    new (p) T(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    return p;
}

template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
          typename Arg6, typename Arg7, typename Arg8, typename Arg9>
T* ma_new(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
          Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
{
    T *p = (T*) ma_alloc(ma, sizeof(T));
    assert(NULL != p);
    new (p) T(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    return p;
}

template <typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
          typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
T* ma_new(memory_allocator *ma, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5,
          Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9, Arg10&& arg10)
{
    T *p = (T*) ma_alloc(ma, sizeof(T));
    assert(NULL != p);
    new (p) T(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    return p;
}
#endif

template <typename T>
void ma_delete(memory_allocator *ma, T *p)
{
    assert(NULL != p);
    p->~T();
    ma_free(ma, p);
}

}

#endif
