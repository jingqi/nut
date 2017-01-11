
#ifndef ___HEADFILE_49E4EA4F_3D1E_4E4E_A406_0186EBB688D9_
#define ___HEADFILE_49E4EA4F_3D1E_4E4E_A406_0186EBB688D9_

#include <assert.h>
#include <stdlib.h>

namespace nut
{

class FreeGuard
{
    void *_ptr = nullptr;

private:
    // Non-copyable
    FreeGuard(const FreeGuard&) = delete;
    FreeGuard& operator=(const FreeGuard&) = delete;

public:
    explicit FreeGuard(void *p = nullptr)
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
        _ptr = nullptr;
    }

    void release()
    {
        if (nullptr != _ptr)
            ::free(_ptr);
        _ptr = nullptr;
    }

    ~FreeGuard()
    {
        release();
    }
};

template <typename T>
class DeleteGuard
{
    T *_ptr = nullptr;

public:
    DeleteGuard(T *p = nullptr)
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
        _ptr = nullptr;
    }

    void release()
    {
        if (nullptr != _ptr)
            delete _ptr;
        _ptr = nullptr;
    }

    ~DeleteGuard()
    {
        release();
    }
};

};

#endif
