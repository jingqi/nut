
#ifndef ___HEADFILE_C26E9AEE_1281_403B_A44C_5CF82E61DD5C_
#define ___HEADFILE_C26E9AEE_1281_403B_A44C_5CF82E61DD5C_

#include <vector>
#include <algorithm>
#include <time.h>
#include <stdint.h>

#include <nut/platform/platform.hpp>
#include <nut/threading/sync/condition.hpp>
#include <nut/threading/sync/guard.hpp>
#include <nut/threading/thread.hpp>

#include "timeval.hpp"

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#else
#   include <sys/time.h>
#endif

namespace nut
{

class TimerManager
{
public:
    typedef void (*timer_func_type)(int,void*);

public:
    struct Timer
    {
        int id;
        TimeVal time;
        void *arg;
        timer_func_type func;
    };

    struct TimerPtrComparor
    {
        bool operator()(const Timer *t1, const Timer *t2)
        {
            assert(NULL != t1 && NULL != t2);
            return t1->time > t2->time; // 用于小头堆算法，故使用">"
        }
    };

    int volatile m_next_id;
    std::vector<Timer*> m_timers;
    Condition m_cond;
    typedef Condition::condition_lock_type lock_type;
    lock_type m_lock;
    bool volatile m_stopping;

public:
    TimerManager()
        : m_next_id(1), m_stopping(false)
    {}

    ~TimerManager()
    {
        for (int i = 0, size = m_timers.size(); i < size; ++i)
            delete m_timers.at(i);
        m_timers.clear();
    }

    /**
     * 添加定时器
     *
     * @param t 距离现在时间的间隔,单位毫秒
     * @return 定时器id
     */
    int addTimer(const TimeVal& interval, timer_func_type func, void *arg = NULL)
    {
        Guard<lock_type> g(&m_lock);
        Timer *ti = new Timer();
        ti->func = func;
        ti->arg = arg;
        ti->id = m_next_id++;
        ti->time = TimeVal::now() + interval;
        if (m_timers.size() == 0 || ti->time < m_timers[0]->time)
            m_cond.signal(); // 通知条件变量

        // 插入小头堆
        m_timers.push_back(ti);
        std::push_heap(m_timers.begin(), m_timers.end(), TimerPtrComparor());

        // 通知条件变量
        m_cond.signal();
        return ti->id;
    }

    bool cancelTimer(int id)
    {
        Guard<lock_type> g(&m_lock);
        size_t i = 0;
        while (i < m_timers.size() && m_timers[i]->id != id)
            ++i;
        if (i >= m_timers.size())
            return false;
        else if (0 == i)
            m_cond.signal(); // 通知条件变量

        // 从堆中移除
        m_timers[i]->time.set(0,0); // 使之成为最小的元素
        std::push_heap(m_timers.begin(), m_timers.begin() + i + 1, TimerPtrComparor());
        std::pop_heap(m_timers.begin(), m_timers.end(), TimerPtrComparor());
        Timer* t = *m_timers.rbegin();
        assert(NULL != t);
        m_timers.pop_back();

        // 删除内存
        delete t;

        return true;
    }

    void interupt()
    {
        Guard<lock_type> g(&m_lock);
        m_stopping = true;
        m_cond.signal();
    }

    void run()
    {
        Guard<lock_type> g(&m_lock);
        m_stopping = false;
        while (!m_stopping)
        {
            const TimeVal now = TimeVal::now();
            if (m_timers.empty())
            {
                m_cond.wait(&m_lock);
            }
            else if (m_timers[0]->time > now)
            {
                const TimeVal wait = m_timers[0]->time - now;
                m_cond.timedwait(&m_lock, wait.sec, wait.usec / 1000);
            }
            else
            {
                Timer *t = m_timers[0];
                std::pop_heap(m_timers.begin(), m_timers.end(), TimerPtrComparor());
                m_timers.pop_back();
                t->func(t->id, t->arg); // 定时完成
                delete t;
            }
        }
    }
};

}

#endif

