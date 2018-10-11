
#ifndef ___HEADFILE___37C18F74_55B3_430D_B169_1B0AA23A8A08_
#define ___HEADFILE___37C18F74_55B3_430D_B169_1B0AA23A8A08_

namespace nut
{

template <typename T>
class Guard
{
public:
    /**
     * @param lock
     *      a pointer to a lock, nullptr will be ignored
     * @param need_lock
     *      whether need to lock it
     * @param need_unlock
     *      whether need to unlock it
     */
    explicit Guard(T *lock, bool need_lock = true, bool need_unlock = true)
        : _lock(lock), _need_unlock(need_unlock)
    {
        if (nullptr != _lock && need_lock)
            _lock->lock();
    }

    ~Guard()
    {
        if (nullptr != _lock && _need_unlock)
            _lock->unlock();
    }

private:
    // Non-copyable
    Guard(const Guard<T>&) = delete;
    Guard& operator=(const Guard<T>&) = delete;

private:
    T *_lock = nullptr;
    bool _need_unlock = true;
};

}

#endif /* head file guarder */
