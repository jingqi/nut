
#ifndef ___HEADFILE_49E4EA4F_3D1E_4E4E_A406_0186EBB688D9_
#define ___HEADFILE_49E4EA4F_3D1E_4E4E_A406_0186EBB688D9_

#include <assert.h>
#include <stdlib.h>


namespace nut
{

class FreeGuard
{
public:
    explicit FreeGuard(void *p = nullptr) noexcept
        : _ptr(p)
    {}

    ~FreeGuard() noexcept
    {
        release();
    }

    void* get() const noexcept
    {
        return _ptr;
    }

    void set(void *p) noexcept
    {
        _ptr = p;
    }

    void clear() noexcept
    {
        _ptr = nullptr;
    }

    void release() noexcept
    {
        if (nullptr != _ptr)
            ::free(_ptr);
        _ptr = nullptr;
    }

private:
    FreeGuard(const FreeGuard&) = delete;
    FreeGuard& operator=(const FreeGuard&) = delete;

private:
    void *_ptr = nullptr;
};

template <typename T>
class DeleteGuard
{
public:
    explicit DeleteGuard(T *p = nullptr) noexcept
        : _ptr(p)
    {}

    ~DeleteGuard() noexcept
    {
        release();
    }

    T* get() const noexcept
    {
        return _ptr;
    }

    void set(T *p) noexcept
    {
        _ptr = p;
    }

    void clear() noexcept
    {
        _ptr = nullptr;
    }

    void release() noexcept
    {
        if (nullptr != _ptr)
            delete _ptr;
        _ptr = nullptr;
    }

private:
    DeleteGuard(const DeleteGuard<T>&) = delete;
    DeleteGuard<T>& operator=(const DeleteGuard<T>&) = delete;

private:
    T *_ptr = nullptr;
};

}

#endif
