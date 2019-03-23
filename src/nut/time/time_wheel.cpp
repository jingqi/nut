/**
 * 参见 http://www.cnblogs.com/zhongwencool/p/timing_wheel.html
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
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
#   if NUT_PLATFORM_OS_LINUX
#       define GET_CLOCK(name) \
            struct timespec name; \
            ::clock_gettime(CLOCK_MONOTONIC_RAW, &name)
#   else
#       define GET_CLOCK(name) \
            struct timespec name; \
            ::clock_gettime(CLOCK_MONOTONIC, &name)
#   endif

#   define CLOCK_IS_ZERO(c) (0 == (c).tv_sec && 0 == (c).tv_nsec)
#   define CLOCK_TO_MS(c) ((c).tv_sec * 1000 + (c).tv_nsec / 1000000)
#   define CLOCK_DIFF_TO_MS(a,b) (((a).tv_sec - (b).tv_sec) * 1000 + ((a).tv_nsec - (b).tv_nsec) / 1000000)
#endif

namespace nut
{

TimeWheel::Timer::Timer(uint64_t when_ms_, uint64_t repeat_ms_, timer_task_type&& task_)
    : valid_mask(VALID_MASK), when_ms(when_ms_), repeat_ms(repeat_ms_),
      task(std::forward<timer_task_type>(task_))
{}

TimeWheel::Timer::Timer(uint64_t when_ms_, uint64_t repeat_ms_, const timer_task_type& task_)
    : valid_mask(VALID_MASK), when_ms(when_ms_), repeat_ms(repeat_ms_),
      task(task_)
{}

TimeWheel::Wheel::Wheel()
{
    ::memset(bucket_heads, 0, sizeof(Timer*) * BUCKETS_PER_WHEEL);
    ::memset(bucket_tails, 0, sizeof(Timer*) * BUCKETS_PER_WHEEL);
}

TimeWheel::Timer::~Timer()
{
    assert(VALID_MASK == valid_mask);
    valid_mask = 0;
    prev = nullptr;
    next = nullptr;
}

TimeWheel::TimeWheel()
{
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

    for (unsigned i = 0; i < _wheel_count; ++i)
        (_wheels + i)->~Wheel();
    _wheel_count = 0;
}

void TimeWheel::ensure_wheel(uint64_t future_tick)
{
    uint64_t tick = _last_tick;
    unsigned need_wheel_count = 1;
    while (future_tick > BUCKETS_PER_WHEEL)
    {
        future_tick += tick % BUCKETS_PER_WHEEL;
        future_tick /= BUCKETS_PER_WHEEL;
        assert(future_tick > 0);
        tick /= BUCKETS_PER_WHEEL;
        ++need_wheel_count;
    }
    assert(need_wheel_count <= MAX_WHEEL_COUNT);

    if (_wheel_count >= need_wheel_count)
        return;

    for (unsigned i = _wheel_count; i < need_wheel_count; ++i)
        new (_wheels + i) Wheel;
    _wheel_count = need_wheel_count;
}

TimeWheel::Timer* TimeWheel::new_timer(uint64_t when_ms, uint64_t repeat_ms,
                                       timer_task_type&& task)
{
    Timer *t = (Timer*) ::malloc(sizeof(Timer));
    assert(nullptr != t);
    new (t) Timer(when_ms, repeat_ms, std::forward<timer_task_type>(task));
    return t;
}

TimeWheel::Timer* TimeWheel::new_timer(uint64_t when_ms, uint64_t repeat_ms,
                                       const timer_task_type& task)
{
    Timer *t = (Timer*) ::malloc(sizeof(Timer));
    assert(nullptr != t);
    new (t) Timer(when_ms, repeat_ms, task);
    return t;
}

void TimeWheel::delete_timer(Timer *t)
{
    assert(nullptr != t);
    t->~Timer();
    ::free(t);
}

size_t TimeWheel::size() const
{
    return _size;
}

void TimeWheel::clear()
{
    for (unsigned i = 0; i <_wheel_count; ++i)
    {
        Wheel& w = _wheels[i];
        for (unsigned j = 0; j < BUCKETS_PER_WHEEL; ++j)
        {
            Timer *t = w.bucket_heads[j];
            while (nullptr != t)
            {
                Timer *next = t->next;
                delete_timer(t);
                t = next;
            }
            w.bucket_heads[j] = nullptr;
            w.bucket_tails[i] = nullptr;
        }
    }
    _size = 0;

    _cancel_later.clear();
}

void TimeWheel::add_timer(Timer *t)
{
    assert(nullptr != t);

    const uint64_t when_tick = t->when_ms / TICK_GRANULARITY_MS;
    uint64_t future_tick = (when_tick > _last_tick ? when_tick - _last_tick : 1);
    assert(future_tick > 0); // 最近也需要放到下一个 tick
    ensure_wheel(future_tick);

    uint64_t tick = _last_tick;
    for (unsigned i = 0; i < _wheel_count; ++i)
    {
        Wheel& w = _wheels[i];
        const unsigned cursor = tick % BUCKETS_PER_WHEEL;

        if (future_tick <= BUCKETS_PER_WHEEL)
        {
            // 插入双链表尾部
            const unsigned bucket_index = (cursor + future_tick) % BUCKETS_PER_WHEEL;
            t->prev = w.bucket_tails[bucket_index];
            t->next = nullptr;
            if (nullptr == t->prev)
            {
                assert(nullptr == w.bucket_heads[bucket_index]);
                w.bucket_heads[bucket_index] = t;
            }
            else
            {
                t->prev->next = t;
            }
            w.bucket_tails[bucket_index] = t;
            ++_size;
            return;
        }
        future_tick += cursor;
        future_tick /= BUCKETS_PER_WHEEL;
        assert(future_tick > 0);
        tick /= BUCKETS_PER_WHEEL;
    }
    assert(false);
}

TimeWheel::timer_id_type TimeWheel::add_timer(uint64_t interval, uint64_t repeat,
                                              timer_task_type&& task)
{
    assert(task);

    GET_CLOCK(now_clock);

    if (CLOCK_IS_ZERO(_first_clock))
        _first_clock = now_clock;

    const uint64_t when_ms = CLOCK_DIFF_TO_MS(now_clock, _first_clock) + interval;
    Timer *t = new_timer(when_ms, repeat, std::forward<timer_task_type>(task));
    assert(nullptr != t);
    add_timer(t);
    return t;
}

TimeWheel::timer_id_type TimeWheel::add_timer(uint64_t interval, uint64_t repeat,
                                              const timer_task_type& task)
{
    assert(task);

    GET_CLOCK(now_clock);

    if (CLOCK_IS_ZERO(_first_clock))
        _first_clock = now_clock;

    const uint64_t when_ms = CLOCK_DIFF_TO_MS(now_clock, _first_clock) + interval;
    Timer *t = new_timer(when_ms, repeat, task);
    assert(nullptr != t);
    add_timer(t);
    return t;
}

bool TimeWheel::do_cancel_timer(Timer *t)
{
    assert(nullptr != t && !_in_invoking_callback);
    if (VALID_MASK != t->valid_mask)
        return false;

    if (nullptr != t->prev && nullptr != t->next)
    {
        // 非链表头部或者尾部，可直接从双链表删除
        t->prev->next = t->next;
        t->next->prev = t->prev;
        delete_timer(t);
        --_size;
        return true;
    }

    const uint64_t when_tick = t->when_ms / TICK_GRANULARITY_MS;
    uint64_t future_tick = (when_tick > _last_tick ? when_tick - _last_tick : 1);
    assert(future_tick > 0); // 最近也是下一个 tick
    uint64_t tick = _last_tick;
    for (unsigned i = 0; i < _wheel_count; ++i)
    {
        Wheel& w = _wheels[i];
        const unsigned cursor = tick % BUCKETS_PER_WHEEL;

        if (future_tick <= BUCKETS_PER_WHEEL)
        {
            // 在链表头部或者尾部
            const unsigned bucket_index = (cursor + future_tick) % BUCKETS_PER_WHEEL;
            if (t == w.bucket_heads[bucket_index])
            {
                // 从链表头部删除
                assert(nullptr == t->prev);
                w.bucket_heads[bucket_index] = t->next;
                if (nullptr == t->next)
                {
                    assert(t == w.bucket_tails[bucket_index]);
                    w.bucket_tails[bucket_index] = nullptr;
                }
                else
                {
                    t->next->prev = nullptr;
                }
                delete_timer(t);
                --_size;
                return true;
            }
            else if (t == w.bucket_tails[bucket_index])
            {
                // 从链表尾部删除
                assert(nullptr == t->next);
                w.bucket_tails[bucket_index] = t->prev;
                if (nullptr == t->prev)
                {
                    assert(t == w.bucket_heads[bucket_index]);
                    w.bucket_heads[bucket_index] = nullptr;
                }
                else
                {
                    t->prev->next = nullptr;
                }
                delete_timer(t);
                --_size;
                return true;
            }
            assert(cursor + future_tick >= BUCKETS_PER_WHEEL);
        }
        future_tick += cursor;
        future_tick /= BUCKETS_PER_WHEEL;
        assert(future_tick > 0);
        tick /= BUCKETS_PER_WHEEL;
    }
    return false;
}

void TimeWheel::cancel_timer(timer_id_type timer_id)
{
    if (nullptr == timer_id)
        return;
    Timer *t = (Timer*) timer_id;
    if (VALID_MASK != t->valid_mask)
        return;
    if (_in_invoking_callback)
        _cancel_later.insert(t);
    else
        do_cancel_timer(t);
}

void TimeWheel::tick()
{
    if (CLOCK_IS_ZERO(_first_clock) || 0 == _size)
        return;

    GET_CLOCK(now_clock);

    const uint64_t now_ms = CLOCK_DIFF_TO_MS(now_clock, _first_clock);
    const uint64_t now_tick = now_ms / TICK_GRANULARITY_MS;
    uint64_t elapse_tick = now_tick - _last_tick;
    if (0 == elapse_tick)
        return;

    uint64_t old_tick = _last_tick;
    _last_tick = now_tick;

    // 找到所有需要处理的定时器
    Timer *pickout_timers = nullptr; // 环状双链表
    for (unsigned i = 0; i < _wheel_count; ++i)
    {
        Wheel& w = _wheels[i];
        const unsigned old_cursor = old_tick % BUCKETS_PER_WHEEL;

        for (unsigned j = 1; j <= BUCKETS_PER_WHEEL && j <= elapse_tick; ++j)
        {
            const unsigned bucket_index = (old_cursor + j) % BUCKETS_PER_WHEEL;
            if (nullptr == w.bucket_heads[bucket_index])
            {
                assert(nullptr == w.bucket_tails[bucket_index]);
                continue;
            }
            assert(nullptr != w.bucket_tails[bucket_index]);

            if (nullptr == pickout_timers)
            {
                pickout_timers = w.bucket_heads[bucket_index];
                pickout_timers->prev = w.bucket_tails[bucket_index];
                pickout_timers->prev->next = pickout_timers;
            }
            else
            {
                w.bucket_heads[bucket_index]->prev = pickout_timers->prev;
                w.bucket_tails[bucket_index]->next = pickout_timers;
                pickout_timers->prev->next = w.bucket_heads[bucket_index];
                pickout_timers->prev = w.bucket_tails[bucket_index];
            }
            w.bucket_heads[bucket_index] = nullptr;
            w.bucket_tails[bucket_index] = nullptr;
        }

        const uint64_t new_cursor = old_cursor + elapse_tick; // NOTE 使用 uint64_t 避免溢出错误
        if (new_cursor < BUCKETS_PER_WHEEL)
            break;
        elapse_tick = new_cursor / BUCKETS_PER_WHEEL;
        old_tick /= BUCKETS_PER_WHEEL;
    }

    // 处置上面收集的定时器：切换时间轮、调用用户回调函数、重复设置定时器
    std::vector<Timer*> callback_later;
    Timer *t = pickout_timers;
    if (nullptr != t)
        t->prev->next = nullptr; // 解开环形，以便遍历
    while (nullptr != t)
    {
        --_size;

        Timer *next = t->next;
        if (t->when_ms > now_ms)
        {
            // 对于需要切换时间轮的定时器，重新插入到时间轮中
            add_timer(t);
        }
        else
        {
            // 调用定时器回调函数
            callback_later.push_back(t);

            // 对于周期性定时器，重新插入到时间轮中
            if (t->repeat_ms > 0)
            {
                t->when_ms += t->repeat_ms;
                add_timer(t);
            }
        }
        t = next;
    }

    // 调用用户的回调函数
    _in_invoking_callback = true;
    for (size_t i = 0, sz = callback_later.size(); i < sz; ++i)
    {
        Timer *t = callback_later.at(i);
        std::set<Timer*>::iterator iter = _cancel_later.find(t);
        if (iter == _cancel_later.end())
        {
            // 定时器回调，用户在回调函数中可能会调用 cancel_timer()，从而导致
            // '_cancel_later' 发生变化
            if (t->repeat_ms > 0)
            {
                t->task(t, now_ms + t->repeat_ms - t->when_ms);
            }
            else
            {
                t->task(t, now_ms - t->when_ms);
                delete_timer(t);
            }
        }
        else
        {
            // - 重复定时器：等着 'cancel_later' 生效即可
            // - 非重复定时器：去除 'cancel_later'，然后 delete 即可
            if (t->repeat_ms <= 0)
            {
                _cancel_later.erase(iter);
                delete_timer(t);
            }
        }
    }
    _in_invoking_callback = false;

    for (std::set<Timer*>::const_iterator iter = _cancel_later.begin(),
             end = _cancel_later.end(); iter != end; ++iter)
    {
        do_cancel_timer(*iter);
    }
    _cancel_later.clear();
}

}
