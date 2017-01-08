
#ifndef ___HEADFILE_49E4EA4F_3D1E_4E4E_A406_0186EBB688D9_
#define ___HEADFILE_49E4EA4F_3D1E_4E4E_A406_0186EBB688D9_

#include <assert.h>
#include <stdlib.h>

namespace nut
{

class FreeGuard
{
    void *_ptr = NULL;

private:
    // Invalid methods
    FreeGuard(const FreeGuard&);
    FreeGuard& operator=(const FreeGuard&);

public:
    explicit FreeGuard(void *p = NULL)
        : _ptr(p)
    {}

    void* get() const
    {
        return _ptr;
    }

    void set(void *p)
    {
        _ptr = p;
    }

    void clear()
    {
        _ptr = NULL;
    }

    void release()
    {
        if (NULL != _ptr)
            ::free(_ptr);
        _ptr = NULL;
    }

    ~FreeGuard()
    {
        release();
    }
};

template <typename T>
class DeleteGuard
{
    T *_ptr = NULL;

public:
    DeleteGuard(T *p = NULL)
        : _ptr(p)
    {}

    T* get() const
    {
        return _ptr;
    }

    void set(T *p)
    {
        _ptr = p;
    }

    void clear()
    {
        _ptr = NULL;
    }

    void release()
    {
        if (NULL != _ptr)
            delete _ptr;
        _ptr = NULL;
    }

    ~DeleteGuard()
    {
        release();
    }
};

};

#endif
