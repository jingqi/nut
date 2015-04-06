
#include <assert.h>
#include <time.h>
#include <algorithm>

#include <nut/platform/platform.h>
#include <nut/threading/sync/guard.hpp>
#include <nut/threading/thread.h>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#else
#   include <sys/time.h>
#endif

#include "timer_manager.h"

namespace nut
{

TimerManager::TimerManager()
    : m_next_id(1), m_stopping(false)
{}

TimerManager::~TimerManager()
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
int TimerManager::add_timer(const TimeVal& interval, timer_func_type func, void *arg)
{
    assert(NULL != func);

    Guard<lock_type> g(&m_lock);
    Timer *ti = new Timer();
    ti->func = func;
    ti->arg = arg;
    ti->id = m_next_id++;
    ti->time = TimeVal::now() + interval;

    // 通知条件变量
    if (m_timers.size() == 0 || ti->time < m_timers[0]->time)
        m_cond.signal();

    // 插入小头堆
    m_timers.push_back(ti);
    std::push_heap(m_timers.begin(), m_timers.end(), TimerPtrComparor());

    return ti->id;
}

bool TimerManager::cancel_timer(int id)
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

void TimerManager::interupt()
{
    Guard<lock_type> g(&m_lock);
    m_stopping = true;
    m_cond.signal();
}

/**
 * 主定时器线程，将阻塞线程，直到 interupt() 被调用
 */
void TimerManager::run()
{
    // 允许的定时误差，防止定时线程抖动厉害
    const TimeVal min_interval(0, 10000);

    Guard<lock_type> g(&m_lock);
    m_stopping = false;
    while (!m_stopping)
    {
        const TimeVal now = TimeVal::now();
        if (m_timers.empty())
        {
            m_cond.wait(&m_lock);
        }
        else if (m_timers[0]->time > now + min_interval)
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

}
