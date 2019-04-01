
#include <assert.h>
#include <time.h>
#include <algorithm>

#include <nut/platform/platform.h>

#include "timer_heap.h"


namespace nut
{

TimerHeap::Timer::Timer(timer_id_type id_, const DateTime& when_, timer_task_type&& task_)
    : id(id_), when(when_), task(std::forward<timer_task_type>(task_))
{}

TimerHeap::Timer::Timer(timer_id_type id_, const DateTime& when_, const timer_task_type& task_)
    : id(id_), when(when_), task(task_)
{}

TimerHeap::~TimerHeap()
{
    for (size_t i = 0, sz = _timers.size(); i < sz; ++i)
    {
        Timer *t = _timers.at(i);
        assert(nullptr != t);
        t->~Timer();
        ::free(t);
    }
    _timers.clear();
}

bool TimerHeap::timer_greater_than(const Timer *t1, const Timer *t2)
{
    assert(nullptr != t1 && nullptr != t2);
    return t1->when > t2->when;
}

void TimerHeap::add_timer(Timer *t)
{
    assert(nullptr != t);

    // 通知条件变量
    if (_timers.size() == 0 || t->when < _timers[0]->when)
        _cond.notify_one();

    // 插入小头堆
    _timers.push_back(t);
    std::push_heap(_timers.begin(), _timers.end(), &timer_greater_than);
}

TimerHeap::timer_id_type TimerHeap::add_timer(const TimeDiff& interval, timer_task_type&& task)
{
    assert(task);

    std::lock_guard<std::mutex> g(_lock);

    Timer *t = (Timer*) ::malloc(sizeof(Timer));
    assert(nullptr != t);
    new (t) Timer(_next_id++, DateTime::now() + interval, std::forward<timer_task_type>(task));

    add_timer(t);

    return t->id;
}

TimerHeap::timer_id_type TimerHeap::add_timer(const TimeDiff& interval, const timer_task_type& task)
{
    assert(task);

    std::lock_guard<std::mutex> g(_lock);

    Timer *t = (Timer*) ::malloc(sizeof(Timer));
    assert(nullptr != t);
    new (t) Timer(_next_id++, DateTime::now() + interval, task);

    add_timer(t);

    return t->id;
}

bool TimerHeap::cancel_timer(timer_id_type id)
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
    _timers[i]->when.set(0, 0); // 使之成为最小的元素
    std::push_heap(_timers.begin(), _timers.begin() + i + 1, &timer_greater_than);
    std::pop_heap(_timers.begin(), _timers.end(), &timer_greater_than);
    Timer* t = *_timers.rbegin();
    assert(nullptr != t);
    _timers.pop_back();

    // 删除内存
    t->~Timer();
    ::free(t);

    return true;
}

void TimerHeap::interupt()
{
    std::lock_guard<std::mutex> g(_lock);
    _stopping = true;
    _cond.notify_one();
}

/**
 * 主定时器线程，将阻塞线程，直到 interupt() 被调用
 */
void TimerHeap::run()
{
    // 允许的定时误差，防止定时线程抖动厉害
    const TimeDiff min_interval(0, RESOLUTION_MS * 1000000);

    std::unique_lock<std::mutex> unique_guard(_lock);
    _stopping = false;
    while (!_stopping)
    {
        const DateTime now = DateTime::now();
        if (_timers.empty())
        {
            _cond.wait(unique_guard);
        }
        else if (_timers[0]->when > now + min_interval)
        {
            const TimeDiff wait = _timers[0]->when - now;
            _cond.wait_for(
                unique_guard,
                std::chrono::milliseconds(
                    wait.get_seconds() * 1000 + wait.get_nanoseconds() / 1000000));
        }
        else
        {
            Timer *t = _timers[0];
            std::pop_heap(_timers.begin(), _timers.end(), &timer_greater_than);
            _timers.pop_back();
            t->task(t->id, now - t->when); // 定时完成

            t->~Timer();
            ::free(t);
        }
    }
}

}
