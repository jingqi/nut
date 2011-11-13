/**
 * @file -
 * @author jingqi
 * @date 2010-7-9
 */

#ifndef ___HEADFILE___37C18F74_55B3_430D_B169_1B0AA23A8A08_
#define ___HEADFILE___37C18F74_55B3_430D_B169_1B0AA23A8A08_

namespace nut
{

template <typename T>
class Guard
{
    T *m_lock;
    bool m_needUnlock;

public :
    /**
     * @param lock, a pointer to a lock, NULL will be ignored
     * @param need_lock, true for lock the lock
     * @param need_unlock, true for unlock the lock
     */
    Guard(T *lock, bool need_lock = true, bool need_unlock = true)
        : m_lock(lock), m_needUnlock(need_unlock)
    {
        if (m_lock != NULL && need_lock)
            m_lock->lock();
    }

    ~Guard()
    {
        if (m_lock != NULL && m_needUnlock)
            m_lock->unlock();
    }
};

}

#endif /* head file guarder */

