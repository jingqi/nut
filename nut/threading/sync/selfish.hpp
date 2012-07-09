/**
 * @file -
 * @author jingqi
 * @date 2010-12-06
 * @brief
 * last-edit : jingqi 2011-01-21 16:58:57
 */

#ifndef ___THREADING___SELFISH_H_
#define ___THREADING___SELFISH_H_

#include <vector>

#include "Mutex.h"

namespace threading
{

/**
 * selfish to token all locks or lost all
 */
class Selfish
{
    typedef std::vector<Mutex*> vec_type;
    vec_type m_guards;
    bool m_failed;

public :
    Selfish() : m_failed(false) {}
    ~Selfish() { release_all_mutex(); }

    void lock(Mutex *l)
    {
        if (NULL == l || m_failed)
            return;

        l->lock();
        m_guards.push_back(l);
    }

    void trylock(Mutex *l)
    {
        if (NULL == l || m_failed)
            return;

        if (l->trylock())
        {
            m_guards.push_back(l);
        }
        else
        {
            release_all_mutex();
            m_failed = true;
        }
    }

    void timedlock(Mutex *l, unsigned s, unsigned ms = 0)
    {
        if (NULL == l || m_failed)
            return;

        if (l->timedlock(s, ms))
        {
            m_guards.push_back(l);
        }
        else
        {
            release_all_mutex();
            m_failed = true;
        }
    }

    void release_all_mutex()
    {
        for (vec_type::const_reverse_iterator it = m_guards.rbegin(), ite = m_guards.rend();
            it != ite; ++it)
            (*it)->unlock();
        m_guards.clear();
    }

    bool release_mutex(Mutex *l)
    {
        for (vec_type::iterator it = m_guards.begin(), ite = m_guards.end();
            it != ite; ++it)
        {
            if ((*it) == l)
            {
                l->unlock();
                m_guards.erase(it);
                return true;
            }
        }
        return false;
    }

    void reset()
    {
        release_all_mutex();
        m_failed = false;
    }

    size_t owned_mutex_count() const { return m_guards.size(); }

    bool failed() const { return m_failed; }
};

}

#endif /* head file guarder */

