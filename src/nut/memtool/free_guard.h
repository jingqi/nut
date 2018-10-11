
#ifndef ___HEADFILE_49E4EA4F_3D1E_4E4E_A406_0186EBB688D9_
#define ___HEADFILE_49E4EA4F_3D1E_4E4E_A406_0186EBB688D9_

#include <assert.h>
#include <stdlib.h>

namespace nut
{

class FreeGuard
{
public:
    explicit FreeGuard(void *p = nullptr)
        : _ptr(p)
    {}

    ~FreeGuard()
    {
        release();
    }

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

private:
    // Non-copyable
    FreeGuard(const FreeGuard&) = delete;
    FreeGuard& operator=(const FreeGuard&) = delete;

private:
    void *_ptr = nullptr;
};

template <typename T>
class DeleteGuard
{
public:
    explicit DeleteGuard(T *p = nullptr)
        : _ptr(p)
    {}

    ~DeleteGuard()
    {
        release();
    }

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

private:
    T *_ptr = nullptr;
};

};

#endif
