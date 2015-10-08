
#include <assert.h>
#include <time.h>
#include <algorithm>

#include <nut/platform/platform.h>
#include <nut/threading/sync/guard.h>
#include <nut/threading/thread.h>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#else
#   include <sys/time.h>
#endif

#include "timer_manager.h"

namespace nut
{

namespace
{

// 用于维护小头堆
struct TimerPtrComparor
{
    bool operator()(const TimerManager::Timer *t1, const TimerManager::Timer *t2)
    {
        assert(NULL != t1 && NULL != t2);
        return t1->time > t2->time; // 用于小头堆算法，故使用">"
    }
};

}

TimerManager::TimerManager()
{}

TimerManager::~TimerManager()
{
    for (size_t i = 0, sz = _timers.size(); i < sz; ++i)
        delete _timers.at(i);
    _timers.clear();
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

    Guard<lock_type> g(&_lock);
    Timer *ti = new Timer();
    ti->func = func;
    ti->arg = arg;
    ti->id = _next_id++;
    ti->time = TimeVal::now() + interval;

    // 通知条件变量
    if (_timers.size() == 0 || ti->time < _timers[0]->time)
        _cond.signal();

    // 插入小头堆
    _timers.push_back(ti);
    std::push_heap(_timers.begin(), _timers.end(), TimerPtrComparor());

    return ti->id;
}

bool TimerManager::cancel_timer(int id)
{
    Guard<lock_type> g(&_lock);
    size_t i = 0;
    while (i < _timers.size() && _timers[i]->id != id)
        ++i;
    if (i >= _timers.size())
        return false;
    else if (0 == i)
        _cond.signal(); // 通知条件变量

    // 从堆中移除
    _timers[i]->time.set(0,0); // 使之成为最小的元素
    std::push_heap(_timers.begin(), _timers.begin() + i + 1, TimerPtrComparor());
    std::pop_heap(_timers.begin(), _timers.end(), TimerPtrComparor());
    Timer* t = *_timers.rbegin();
    assert(NULL != t);
    _timers.pop_back();

    // 删除内存
    delete t;

    return true;
}

void TimerManager::interupt()
{
    Guard<lock_type> g(&_lock);
    _stopping = true;
    _cond.signal();
}

/**
 * 主定时器线程，将阻塞线程，直到 interupt() 被调用
 */
void TimerManager::run()
{
    // 允许的定时误差，防止定时线程抖动厉害
    const TimeVal min_interval(0, 10000);

    Guard<lock_type> g(&_lock);
    _stopping = false;
    while (!_stopping)
    {
        const TimeVal now = TimeVal::now();
        if (_timers.empty())
        {
            _cond.wait(&_lock);
        }
        else if (_timers[0]->time > now + min_interval)
        {
            const TimeVal wait = _timers[0]->time - now;
            _cond.timedwait(&_lock, (unsigned) wait.sec, (unsigned) (wait.usec / 1000));
        }
        else
        {
            Timer *t = _timers[0];
            std::pop_heap(_timers.begin(), _timers.end(), TimerPtrComparor());
            _timers.pop_back();
            t->func(t->id, t->arg); // 定时完成
            delete t;
        }
    }
}

}
