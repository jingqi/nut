
#ifndef ___HEADFILE_FC64239C_3F1F_4969_A9D7_1D52A7BEAF6E_
#define ___HEADFILE_FC64239C_3F1F_4969_A9D7_1D52A7BEAF6E_

#include <stdint.h>
#include <time.h>

#include <nut/nut_config.h>
#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif

namespace nut
{

/**
 * 时间轮
 */
class NUT_API TimeWheel
{
public:
    typedef void *timer_id_t; // 定时器 id 类型
    typedef void (*timer_func_t)(timer_id_t,void*,uint64_t); // 定时器回调函数类型

    enum
    {
        TICK_GRANULARITY_MS = 10, // 每个 tick 粒度，单位: 毫秒
    };

private:
    enum
    {
        BUCKETS_PER_WHERE = 256, // 每个 wheel 包含的 bucket 数
        WHEEL_COUNT = 5,         // wheel 数
    };

    class Timer
    {
    public:
        uint32_t valid_mask = 0;

        uint64_t when_ms = 0;
        uint64_t repeat_ms = 0;   // 重复时间，单位: 毫秒

        timer_func_t func = NULL; // 定时器回调函数
        void *arg = NULL;         // 定时器用户参数

        Timer *next = NULL;       // 链指针
    };

    class Wheel
    {
    public:
        uint64_t cursor = 0; // NOTE cursor 取宽字是为了避免加减法时溢出
        Timer *buckets[BUCKETS_PER_WHERE];
    };

    Wheel _wheels[WHEEL_COUNT];

    // 计时起点
#if NUT_PLATFORM_OS_WINDOWS
    LARGE_INTEGER _clock_freq, _first_clock;
#else
    struct timespec _first_clock;
#endif

    uint64_t _last_tick = 0;  // 上一个 tick
    size_t _size = 0;         // 定时器数量
    bool _ticking = false;
    std::vector<Timer*> _to_be_canceled; // 延迟取消列表

private:
    TimeWheel(const TimeWheel&);
    TimeWheel& operator=(const TimeWheel&);

    static Timer* new_timer(uint64_t when_ms, uint64_t repeat_ms,
                            timer_func_t func, void *arg);
    static void delete_timer(Timer *t);

    timer_id_t add_timer(Timer *t);
    bool do_cancel_timer(Timer *t);

    static bool timer_less(const Timer *t1, const Timer *t2);
    static Timer* reverse_link(Timer *t);

public:
    TimeWheel();
    ~TimeWheel();

    size_t size() const;
    void clear();

    /**
     * @param interval 延时间隔，单位毫秒
     * @param repeat 重复间隔，单位毫秒, 0 表示不延时
     */
    timer_id_t add_timer(uint64_t interval, uint64_t repeat,
                         timer_func_t func, void *arg = NULL);

    /**
     * 注意，如果当前已经处于 tick() 过程中，取消操作不会对当前 tick 生效
     */
    void cancel_timer(timer_id_t timer_id);

    void tick();
};

}

#endif
