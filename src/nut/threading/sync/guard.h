
#ifndef ___HEADFILE___37C18F74_55B3_430D_B169_1B0AA23A8A08_
#define ___HEADFILE___37C18F74_55B3_430D_B169_1B0AA23A8A08_

namespace nut
{

template <typename T>
class Guard
{
    T *m_lock;
    bool m_need_unlock;

public :
    /**
     * @param lock
     *      a pointer to a lock, NULL will be ignored
     * @param need_lock
     *      whether need to lock it
     * @param need_unlock
     *      whether need to unlock it
     */
    Guard(T *lock, bool need_lock = true, bool need_unlock = true)
        : m_lock(lock), m_need_unlock(need_unlock)
    {
        if (NULL != m_lock && need_lock)
            m_lock->lock();
    }

    ~Guard()
    {
        if (NULL != m_lock && m_need_unlock)
            m_lock->unlock();
    }
};

}

#endif /* head file guarder */
