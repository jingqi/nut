
#ifndef ___HEADFILE_FC64239C_3F1F_4969_A9D7_1D52A7BEAF6E_
#define ___HEADFILE_FC64239C_3F1F_4969_A9D7_1D52A7BEAF6E_

#include <stdint.h>
#include <time.h>
#include <set>
#include <limits>
#include <functional>

#include "../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#endif

#include "../nut_config.h"


// 无效定时器 id
#define NUT_INVALID_TIMER_ID nullptr

namespace nut
{

/**
 * 时间轮
 */
class NUT_API TimeWheel
{
public:
    // 定时器 id 类型
    typedef void *timer_id_type;
    // 定时器任务类型
    typedef std::function<void(timer_id_type,int64_t)> timer_task_type;

    // 定时器精度，单位: 毫秒
    static constexpr uint64_t RESOLUTION_MS = 5;

private:
    // 每个 wheel 包含的 bucket 数
    static constexpr unsigned BUCKETS_PER_WHEEL = 256;

    // 最大 wheel 数，能容纳所有 uint64_t 毫秒定时器 (256**8 = 2**64)
    static constexpr unsigned MAX_WHEEL_COUNT = 8;

    class Timer
    {
    public:
        Timer(uint64_t when_ms_, uint64_t repeat_ms_, timer_task_type&& task_) noexcept;
        Timer(uint64_t when_ms_, uint64_t repeat_ms_, const timer_task_type& task_) noexcept;
        ~Timer() noexcept;

    public:
        uint32_t valid_mask = 0; // 是否有效，避免内存被释放后依然被错误使用

        uint64_t when_ms = 0;
        uint64_t repeat_ms = 0; // 重复时间，单位: 毫秒

        timer_task_type task; // 定时器任务

        Timer *prev = nullptr, *next = nullptr; // bucket 双链表
    };

    class Wheel
    {
    public:
        Wheel() noexcept;

    public:
        Timer *bucket_heads[BUCKETS_PER_WHEEL];
        Timer *bucket_tails[BUCKETS_PER_WHEEL];
    };

public:
    TimeWheel() noexcept;
    ~TimeWheel() noexcept;

    size_t size() const noexcept;
    void clear() noexcept;

    /**
     * @param interval 延时间隔，单位毫秒
     * @param repeat 重复间隔，单位毫秒, 0 表示不延时
     */
    timer_id_type add_timer(uint64_t interval, uint64_t repeat,
                            timer_task_type&& task) noexcept;
    timer_id_type add_timer(uint64_t interval, uint64_t repeat,
                            const timer_task_type& task) noexcept;

    /**
     * 取消定时器
     */
    void cancel_timer(timer_id_type timer_id) noexcept;

    /**
     * 获取从现在开始的可空闲时间，单位毫秒
     *
     * @return UINT64_MAX 表示无限等待
     */
    uint64_t get_idle() const noexcept;

    void tick() noexcept;

private:
    TimeWheel(const TimeWheel&) = delete;
    TimeWheel& operator=(const TimeWheel&) = delete;

    void ensure_wheel(uint64_t future_tick) noexcept;

    static Timer* new_timer(uint64_t when_ms, uint64_t repeat_ms,
                            timer_task_type&& task) noexcept;
    static Timer* new_timer(uint64_t when_ms, uint64_t repeat_ms,
                            const timer_task_type& task) noexcept;
    static void delete_timer(Timer *t) noexcept;

    void add_timer(Timer *t) noexcept;
    bool do_cancel_timer(Timer *t) noexcept;

    /**
     * 搜索最近的定时器
     *
     * @param future_tick 开始搜索的位置
     */
    uint64_t search_min_timer_tick(uint64_t future_tick = 0) const noexcept;

private:
    // 时间轮
    Wheel _wheels[MAX_WHEEL_COUNT];
    unsigned _wheel_count = 0;

    // 计时起点
#if NUT_PLATFORM_OS_WINDOWS
    LARGE_INTEGER _clock_freq, _first_clock;
#else
    struct timespec _first_clock;
#endif

    // 上一个 tick
    uint64_t _last_tick = 0;

    // 定时器数量
    size_t _size = 0;

    // 是否正在集中处理用户定时器回调。
    // 集中处理用户定时器回调时，do_cancel_timer() 对需要回调的 timer 是失效的
    bool _in_invoking_callback = false;
    // 延迟取消列表，原因同上
    std::set<Timer*> _cancel_later;

    // 最小 timer 对应的 tick (只会偏小，不会偏大)
    uint64_t _min_timer_tick = UINT64_MAX;
};

}

#endif
