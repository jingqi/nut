
#ifndef ___HEADFILE_C26E9AEE_1281_403B_A44C_5CF82E61DD5C_
#define ___HEADFILE_C26E9AEE_1281_403B_A44C_5CF82E61DD5C_

#include <assert.h>
#include <time.h>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "../nut_config.h"
#include "../platform/platform.h"
#include "date_time.h"


namespace nut
{

/**
 * 不太精确的用户级定时器
 */
class NUT_API TimerHeap
{
public:
    // 定时器 id 类型
    typedef int timer_id_type;
    // 定时器任务类型
    typedef std::function<void(timer_id_type,const TimeDiff&)> timer_task_type;

    // 定时器精度，单位: 毫秒
    static constexpr uint64_t RESOLUTION_MS = 2;

private:
    // 定时器
    class Timer
    {
    public:
        Timer(timer_id_type id_, const DateTime& when_, timer_task_type&& task_) noexcept;
        Timer(timer_id_type id_, const DateTime& when_, const timer_task_type& task_) noexcept;

    public:
        timer_id_type id = 0;
        DateTime when;
        timer_task_type task;
    };

public:
    TimerHeap() = default;
    ~TimerHeap() noexcept;

    /**
     * 添加定时器
     *
     * @param interval 距离现在时间的间隔
     * @return 定时器 id
     */
    timer_id_type add_timer(const TimeDiff& interval, timer_task_type&& task) noexcept;
    timer_id_type add_timer(const TimeDiff& interval, const timer_task_type& task) noexcept;

    bool cancel_timer(timer_id_type id) noexcept;

    void interupt() noexcept;

    /**
     * 主定时器线程，将阻塞线程，直到 interupt() 被调用
     */
    void run() noexcept;

private:
    TimerHeap(const TimerHeap& x) = delete;
    TimerHeap& operator=(const TimerHeap& x) = delete;

    // NOTE 应该在 mutex 保护下运行
    void add_timer(Timer *t) noexcept;

    // Greater-Than 操作符用于维护小头堆算法
    static bool timer_greater_than(const Timer *t1, const Timer *t2) noexcept;

private:
    timer_id_type _next_id = 1;  // 用于生成不重复的 timer id
    std::vector<Timer*> _timers; // 小头堆
    std::mutex _lock;
    std::condition_variable _cond;
    bool _stopping = false; // 是否停止所有计时器
};

}

#endif
