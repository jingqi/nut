/**
 * 参见 http://www.cnblogs.com/zhongwencool/p/timing_wheel.html
 */

#include <assert.h>
#include <stdlib.h>
#include <vector>

#include "time_wheel.h"


#define VALID_MASK 0x5011de5a

// ms -> clock
#define MS_TO_CLOCK(ms) ((ms) * CLOCKS_PER_SEC / 1000)
// clock -> ms
#define CLOCK_TO_MS(c) ((c) * 1000 / CLOCKS_PER_SEC)

namespace nut
{

TimeWheel::TimeWheel()
{
    for (int i = 0; i < WHEEL_COUNT; ++i)
    {
        Wheel& w = _wheels[i];
        for (int j = 0; j < BUCKETS_PER_WHERE; ++j)
            w.buckets[j] = NULL;
    }
}

TimeWheel::~TimeWheel()
{
    clear();
}

TimeWheel::Timer* TimeWheel::new_timer(uint64_t when, uint64_t repeat_ms,
                                       timer_func_t func, void *arg)
{
    Timer *t = (Timer*) ::malloc(sizeof(Timer));
    assert(NULL != t);
    t->valid_mask = VALID_MASK;
    t->when = when;
    t->repeat_ms = repeat_ms;
    t->func = func;
    t->arg = arg;
    t->next = NULL;
    return t;
}

void TimeWheel::delete_timer(Timer *t)
{
    assert(NULL != t);
    t->valid_mask = 0;
    ::free(t);
}

size_t TimeWheel::size() const
{
    assert(!_in_tick);
    return _size;
}

void TimeWheel::clear()
{
    assert(!_in_tick);

    for (int i = 0; i < WHEEL_COUNT; ++i)
    {
        Wheel& w = _wheels[i];
        for (int j = 0; j < BUCKETS_PER_WHERE; ++j)
        {
            Timer *t = w.buckets[j];
            while (NULL != t)
            {
                Timer *next = t->next;
                delete_timer(t);
                t = next;
            }
            w.buckets[j] = NULL;
        }
    }
    _size = 0;
}

TimeWheel::timer_id_t TimeWheel::add_timer(Timer *t)
{
    assert(NULL != t);

    uint64_t expires = (t->when > _last_tick ? t->when - _last_tick : 1);
    assert(expires > 0); // Min expires is 1
    for (int i = 0; i < WHEEL_COUNT; ++i)
    {
        Wheel& w = _wheels[i];

        if (expires <= BUCKETS_PER_WHERE)
        {
            const int bucket_index = (w.cursor + expires) % BUCKETS_PER_WHERE;
            t->next = w.buckets[bucket_index];
            w.buckets[bucket_index] = t;
            ++_size;
            return t;
        }
        expires += w.cursor;
        expires /= BUCKETS_PER_WHERE;
    }

    // Time out of range
    return NULL;
}

TimeWheel::timer_id_t TimeWheel::add_timer(uint64_t interval, uint64_t repeat,
                                           timer_func_t func, void *arg)
{
    assert(NULL != func);

    const clock_t now_clock = ::clock();
    if (0 == _first_clock)
        _first_clock = now_clock;

    const uint64_t when = (CLOCK_TO_MS(now_clock - _first_clock) +
                           interval) / TICK_INTERVAL_MS;

    Timer *t = new_timer(when, repeat, func, arg);
    assert(NULL != t);
    timer_id_t rs = add_timer(t);
    if (NULL == rs)
        delete_timer(t);
    return rs;
}

bool TimeWheel::do_cancel_timer(Timer *t)
{
    assert(NULL != t && !_in_tick);
    if (VALID_MASK != t->valid_mask)
        return false;

    uint64_t expires = (t->when > _last_tick ? t->when - _last_tick : 1);
    assert(expires > 0); // Min expires is 1
    for (int i = 0; i < WHEEL_COUNT; ++i)
    {
        Wheel& w = _wheels[i];

        if (expires <= BUCKETS_PER_WHERE)
        {
            const int bucket_index = (w.cursor + expires) % BUCKETS_PER_WHERE;
            Timer *pre = NULL, *p = w.buckets[bucket_index];
            while (NULL != p)
            {
                if (t == p)
                {
                    if (NULL == pre)
                        w.buckets[bucket_index] = t->next;
                    else
                        pre->next = t->next;
                    delete_timer(t);
                    --_size;
                    return true;
                }
                pre = p;
                p = p->next;
            }
        }
        expires += w.cursor;
        expires /= BUCKETS_PER_WHERE;
        if (0 == expires)
            break;
    }
    return false;
}

void TimeWheel::cancel_timer(timer_id_t timer_id)
{
    if (NULL == timer_id)
        return;
    Timer *t = (Timer*) timer_id;
    if (VALID_MASK != t->valid_mask)
        return;
    if (_in_tick)
        _to_be_canceled.push_back(t);
    else
        do_cancel_timer(t);
}

bool TimeWheel::timer_less(const Timer *t1, const Timer *t2)
{
    assert(NULL != t1 && NULL != t2);
    return t1->when < t2->when;
}

TimeWheel::Timer* TimeWheel::reverse_link(Timer *t)
{
    Timer *l = NULL;
    while (NULL != t)
    {
        Timer *next = t->next;
        t->next = l;
        l = t;
        t = next;
    }
    return l;
}

void TimeWheel::tick()
{
    assert(!_in_tick);
    if (0 == _first_clock || 0 == _size)
        return;
    const uint64_t now = CLOCK_TO_MS(::clock() - _first_clock) / TICK_INTERVAL_MS;
    uint64_t elapse = now - _last_tick;
    if (0 == elapse)
        return;
    _in_tick = true;
    _last_tick = now;

    // 找到所有需要操作的定时器
    Timer *timers = NULL; // 单链表
    for (int i = 0; i < WHEEL_COUNT; ++i)
    {
        Wheel& w = _wheels[i];

        for (int j = 1; j <= BUCKETS_PER_WHERE && j <= elapse; ++j)
        {
            const int bucket_index = (w.cursor + j) % BUCKETS_PER_WHERE;
            Timer *t = w.buckets[bucket_index];
            while (NULL != t)
            {
                Timer *next = t->next;
                t->next = timers;
                timers = t;
                --_size;
                t = next;
            }
            w.buckets[bucket_index] = NULL;
        }

        w.cursor += elapse;
        if (w.cursor < BUCKETS_PER_WHERE)
            break;
        elapse = w.cursor / BUCKETS_PER_WHERE;
        w.cursor %= BUCKETS_PER_WHERE;
    }

    // NOTE 一次处理多个 tick 的情况下，为了性能考虑，仅保持的大致调用顺序，不做精确排序
    timers = reverse_link(timers);

    Timer *t = timers;
    while (NULL != t)
    {
        Timer *next = t->next;
        if (t->when > now)
        {
            // 对于需要切换时间轮的定时器，重新插入到时间轮中
            add_timer(t);
        }
        else
        {
            // 调用定时器回调函数
            t->func(t, t->arg, (now - t->when) * TICK_INTERVAL_MS);

            // 对于周期性定时器，重新插入到时间轮中
            if (t->repeat_ms > 0)
            {
                t->when += t->repeat_ms / TICK_INTERVAL_MS;
                add_timer(t);
            }
            else
            {
                delete_timer(t);
            }
        }
        t = next;
    }

    _in_tick = false;

    // 删掉延迟删除的定时器
    for (size_t i = 0, size = _to_be_canceled.size(); i < size; ++i)
        do_cancel_timer(_to_be_canceled.at(i));
    _to_be_canceled.clear();
}

}
