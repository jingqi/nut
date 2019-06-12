
#ifndef ___HEADFILE___37C18F74_55B3_430D_B169_1B0AA23A8A08_
#define ___HEADFILE___37C18F74_55B3_430D_B169_1B0AA23A8A08_

#include <assert.h>


namespace nut
{

template <typename T>
class LockGuard
{
public:
    explicit LockGuard(T *lock, bool lock_now = true) noexcept
        : _lock(lock)
    {
        assert(nullptr != lock);
        if (lock_now)
            _lock->lock();
    }

    ~LockGuard() noexcept
    {
        release();
    }

    void set_need_unlock(bool need_unlock) noexcept
    {
        _need_unlock = need_unlock;
    }

    void lock() noexcept
    {
        if (nullptr != _lock)
            _lock->lock();
        _need_unlock = true;
    }

    void unlock() noexcept
    {
        if (nullptr != _lock)
            _lock->unlock();
        _need_unlock = false;
    }

    void release() noexcept
    {
        if (nullptr != _lock && _need_unlock)
            _lock->unlock();
        _lock = nullptr;
        _need_unlock = false;
    }

private:
    LockGuard(const LockGuard<T>&) = delete;
    LockGuard& operator=(const LockGuard<T>&) = delete;

private:
    T *_lock = nullptr;
    bool _need_unlock = true;
};

}

#endif /* head file guarder */
