
#include <assert.h>
#include <time.h>
#include <algorithm>

#include <nut/platform/platform.h>

#include "timer_manager.h"

namespace nut
{

TimerManager::~TimerManager()
{
    for (size_t i = 0, sz = _timers.size(); i < sz; ++i)
    {
        Timer *t = _timers.at(i);
        assert(nullptr != t);
        delete_timer(t);
    }
    _timers.clear();
}

TimerManager::Timer* TimerManager::new_timer()
{
    Timer *t = (Timer*) ::malloc(sizeof(Timer));
    assert(nullptr != t);
    new (t) Timer;
    return t;
}

void TimerManager::delete_timer(Timer *t)
{
    assert(nullptr != t);
    t->~Timer();
    ::free(t);
}

bool TimerManager::timer_greater_than(const Timer *t1, const Timer *t2)
{
    assert(nullptr != t1 && nullptr != t2);
    return t1->time > t2->time;
}

/**
 * 添加定时器
 */
TimerManager::timer_id_type TimerManager::add_timer(const TimeDiff& interval, const timer_task_type& task)
{
    assert(task);

    std::lock_guard<std::mutex> g(_lock);

    Timer *t = new_timer();
    t->task = task;
    t->id = _next_id++;
    t->time = DateTime::now() + interval;

    // 通知条件变量
    if (_timers.size() == 0 || t->time < _timers[0]->time)
        _cond.notify_one();

    // 插入小头堆
    _timers.push_back(t);
    std::push_heap(_timers.begin(), _timers.end(), &timer_greater_than);

    return t->id;
}

bool TimerManager::cancel_timer(timer_id_type id)
{
    std::lock_guard<std::mutex> g(_lock);
    size_t i = 0;
    while (i < _timers.size() && _timers[i]->id != id)
        ++i;
    if (i >= _timers.size())
        return false;
    else if (0 == i)
        _cond.notify_one(); // 通知条件变量

    // 从堆中移除
    _timers[i]->time.set(0,0); // 使之成为最小的元素
    std::push_heap(_timers.begin(), _timers.begin() + i + 1, &timer_greater_than);
    std::pop_heap(_timers.begin(), _timers.end(), &timer_greater_than);
    Timer* t = *_timers.rbegin();
    assert(nullptr != t);
    _timers.pop_back();

    // 删除内存
    delete_timer(t);

    return true;
}

void TimerManager::interupt()
{
    std::lock_guard<std::mutex> g(_lock);
    _stopping = true;
    _cond.notify_one();
}

/**
 * 主定时器线程，将阻塞线程，直到 interupt() 被调用
 */
void TimerManager::run()
{
    // 允许的定时误差，防止定时线程抖动厉害
    const TimeDiff min_interval(0, 10000);

    std::unique_lock<std::mutex> unique_guard(_lock);
    _stopping = false;
    while (!_stopping)
    {
        const DateTime now = DateTime::now();
        if (_timers.empty())
        {
            _cond.wait(unique_guard);
        }
        else if (_timers[0]->time > now + min_interval)
        {
            const TimeDiff wait = _timers[0]->time - now;
            _cond.wait_for(
                unique_guard,
                std::chrono::milliseconds(
                    wait.get_seconds() * 1000 + wait.get_useconds() / 1000));
        }
        else
        {
            Timer *t = _timers[0];
            std::pop_heap(_timers.begin(), _timers.end(), &timer_greater_than);
            _timers.pop_back();
            t->task(t->id); // 定时完成
            delete t;
        }
    }
}

}
