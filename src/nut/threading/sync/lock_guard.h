
#ifndef ___HEADFILE___37C18F74_55B3_430D_B169_1B0AA23A8A08_
#define ___HEADFILE___37C18F74_55B3_430D_B169_1B0AA23A8A08_

namespace nut
{

template <typename T>
class LockGuard
{
public:
    /**
     * @param lock       A pointer to a lock, nullptr will be ignored
     * @param defer_lock Don't lock now, and don't unlock on destruction if lock()
     *                   is never called
     */
    explicit LockGuard(T *lock, bool defer_lock = false)
        : _lock(lock), _own_the_lock(!defer_lock)
    {
        if (nullptr != _lock && !defer_lock)
            _lock->lock();
    }

    ~LockGuard()
    {
        release();
    }

    void lock()
    {
        assert(!_own_the_lock);
        if (nullptr != _lock)
            _lock->lock();
        _own_the_lock = true;
    }

    void unlock()
    {
        assert(_own_the_lock);
        if (nullptr != _lock)
            _lock->unlock();
        _own_the_lock = false;
    }

    void release()
    {
        if (nullptr != _lock && _own_the_lock)
            _lock->unlock();
        _lock = nullptr;
        _own_the_lock = false;
    }

private:
    LockGuard(const LockGuard<T>&) = delete;
    LockGuard& operator=(const LockGuard<T>&) = delete;

private:
    T *_lock = nullptr;
    bool _own_the_lock = true;
};

}

#endif /* head file guarder */
