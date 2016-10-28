/**
 * 参见 http://www.cnblogs.com/zhongwencool/p/timing_wheel.html
 */

#include <assert.h>
#include <stdlib.h>
#include <vector>

#include "time_wheel.h"


#define VALID_MASK 0x5011de5a

#if NUT_PLATFORM_OS_WINDOWS
#   define GET_CLOCK(name) \
        LARGE_INTEGER name; \
        ::QueryPerformanceCounter(&name)

#   define CLOCK_IS_ZERO(c) (0 == (c).QuadPart)
#   define CLOCK_TO_MS(c) ((c).QuadPart * 1000 / _clock_freq.QuadPart)
#   define CLOCK_DIFF_TO_MS(a,b) (((a).QuadPart - (b).QuadPart) * 1000 / _clock_freq.QuadPart)
#else
    // 使用 CLOCK_MONOTONIC_RAW 或者 CLOCK_MONOTONIC_RAW_APPROX
#   define GET_CLOCK(name) \
        struct timespec name; \
        ::clock_gettime(CLOCK_MONOTONIC_RAW, &name)

#   define CLOCK_IS_ZERO(c) (0 == (c).tv_sec && 0 == (c).tv_nsec)
#   define CLOCK_TO_MS(c) ((c).tv_sec * 1000 + (c).tv_nsec / 1000000)
#   define CLOCK_DIFF_TO_MS(a,b) (((a).tv_sec - (b).tv_sec) * 1000 + ((a).tv_nsec - (b).tv_nsec) / 1000000)
#endif

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

#if NUT_PLATFORM_OS_WINDOWS
    ::QueryPerformanceFrequency(&_clock_freq);
    _first_clock.QuadPart = 0;
#else
    ::memset(&_first_clock, 0, sizeof(_first_clock));
#endif
}

TimeWheel::~TimeWheel()
{
    clear();
}

TimeWheel::Timer* TimeWheel::new_timer(uint64_t when_ms, uint64_t repeat_ms,
                                       timer_func_type func, void *arg)
{
    Timer *t = (Timer*) ::malloc(sizeof(Timer));
    assert(NULL != t);
    t->valid_mask = VALID_MASK;
    t->when_ms = when_ms;
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
    assert(!_ticking);
    return _size;
}

void TimeWheel::clear()
{
    assert(!_ticking);

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

TimeWheel::timer_id_type TimeWheel::add_timer(Timer *t)
{
    assert(NULL != t);

    const uint64_t when_tick = t->when_ms / TICK_GRANULARITY_MS;
    uint64_t expires_tick = (when_tick > _last_tick ? when_tick - _last_tick : 1);
    assert(expires_tick > 0); // Min expires tick is 1
    for (int i = 0; i < WHEEL_COUNT; ++i)
    {
        Wheel& w = _wheels[i];

        if (expires_tick <= BUCKETS_PER_WHERE)
        {
            const int bucket_index = (w.cursor + expires_tick) % BUCKETS_PER_WHERE;
            t->next = w.buckets[bucket_index];
            w.buckets[bucket_index] = t;
            ++_size;
            return t;
        }
        expires_tick += w.cursor;
        expires_tick /= BUCKETS_PER_WHERE;
    }

    // Time out of range
    return NULL;
}

TimeWheel::timer_id_type TimeWheel::add_timer(uint64_t interval, uint64_t repeat,
                                              timer_func_type func, void *arg)
{
    assert(NULL != func);

    GET_CLOCK(now_clock);

    if (CLOCK_IS_ZERO(_first_clock))
        _first_clock = now_clock;

    const uint64_t when_ms = CLOCK_DIFF_TO_MS(now_clock, _first_clock) + interval;
    Timer *t = new_timer(when_ms, repeat, func, arg);
    assert(NULL != t);
    timer_id_type rs = add_timer(t);
    if (NULL == rs)
        delete_timer(t);
    return rs;
}

bool TimeWheel::do_cancel_timer(Timer *t)
{
    assert(NULL != t && !_ticking);
    if (VALID_MASK != t->valid_mask)
        return false;

    const uint64_t when_tick = t->when_ms / TICK_GRANULARITY_MS;
    uint64_t expires_tick = (when_tick > _last_tick ? when_tick - _last_tick : 1);
    assert(expires_tick > 0); // Min expires tick is 1
    for (int i = 0; i < WHEEL_COUNT; ++i)
    {
        Wheel& w = _wheels[i];

        if (expires_tick <= BUCKETS_PER_WHERE)
        {
            const int bucket_index = (w.cursor + expires_tick) % BUCKETS_PER_WHERE;
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
        expires_tick += w.cursor;
        expires_tick /= BUCKETS_PER_WHERE;
        if (0 == expires_tick)
            break;
    }
    return false;
}

void TimeWheel::cancel_timer(timer_id_type timer_id)
{
    if (NULL == timer_id)
        return;
    Timer *t = (Timer*) timer_id;
    if (VALID_MASK != t->valid_mask)
        return;
    if (_ticking)
        _to_be_canceled.push_back(t);
    else
        do_cancel_timer(t);
}

bool TimeWheel::timer_less(const Timer *t1, const Timer *t2)
{
    assert(NULL != t1 && NULL != t2);
    return t1->when_ms < t2->when_ms;
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
    assert(!_ticking);
    if (CLOCK_IS_ZERO(_first_clock) || 0 == _size)
        return;

    GET_CLOCK(now_clock);

    const uint64_t now_ms = CLOCK_DIFF_TO_MS(now_clock, _first_clock);
    const uint64_t now_tick = now_ms / TICK_GRANULARITY_MS;
    uint64_t elapse_tick = now_tick - _last_tick;
    if (0 == elapse_tick)
        return;

    _ticking = true;
    _last_tick = now_tick;

    // 找到所有需要操作的定时器
    Timer *timers = NULL; // 单链表
    for (int i = 0; i < WHEEL_COUNT; ++i)
    {
        Wheel& w = _wheels[i];

        for (int j = 1; j <= BUCKETS_PER_WHERE && j <= elapse_tick; ++j)
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

        w.cursor += elapse_tick;
        if (w.cursor < BUCKETS_PER_WHERE)
            break;
        elapse_tick = w.cursor / BUCKETS_PER_WHERE;
        w.cursor %= BUCKETS_PER_WHERE;
    }

    // NOTE 一次处理多个 tick 的情况下，为了性能考虑，仅保持的大致调用顺序，不做精确排序
    timers = reverse_link(timers);

    Timer *t = timers;
    while (NULL != t)
    {
        Timer *next = t->next;
        if (t->when_ms > now_ms)
        {
            // 对于需要切换时间轮的定时器，重新插入到时间轮中
            add_timer(t);
        }
        else
        {
            // 调用定时器回调函数
            t->func(t, t->arg, now_ms - t->when_ms);

            // 对于周期性定时器，重新插入到时间轮中
            if (t->repeat_ms > 0)
            {
                t->when_ms += t->repeat_ms;
                add_timer(t);
            }
            else
            {
                delete_timer(t);
            }
        }
        t = next;
    }

    _ticking = false;

    // 删掉延迟删除的定时器
    for (size_t i = 0, size = _to_be_canceled.size(); i < size; ++i)
        do_cancel_timer(_to_be_canceled.at(i));
    _to_be_canceled.clear();
}

}
