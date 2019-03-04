
#ifndef ___HEADFILE_C26E9AEE_1281_403B_A44C_5CF82E61DD5C_
#define ___HEADFILE_C26E9AEE_1281_403B_A44C_5CF82E61DD5C_

#include <assert.h>
#include <time.h>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>

#include <nut/platform/platform.h>

#include "../nut_config.h"
#include "date_time.h"


namespace nut
{

/**
 * 不太精确的用户级定时器
 */
class NUT_API TimerManager
{
public:
    // 定时器 id 类型
    typedef int timer_id_type;
    // 定时器任务类型
    typedef std::function<void(timer_id_type)> timer_task_type;

private:
    // 定时器
    struct Timer
    {
        timer_id_type id = 0;
        DateTime time;
        timer_task_type task;
    };

public:
    TimerManager() = default;
    ~TimerManager();

    /**
     * 添加定时器
     *
     * @param interval 距离现在时间的间隔
     * @return 定时器 id
     */
    timer_id_type add_timer(const TimeDiff& interval, const timer_task_type& task);

    bool cancel_timer(timer_id_type id);

    void interupt();

    /**
     * 主定时器线程，将阻塞线程，直到 interupt() 被调用
     */
    void run();

private:
    TimerManager(const TimerManager& x) = delete;
    TimerManager& operator=(const TimerManager& x) = delete;

    Timer* new_timer();
    void delete_timer(Timer *t);

    // Greater-Than 操作符用于维护小头堆算法
    static bool timer_greater_than(const Timer *t1, const Timer *t2);

private:
    timer_id_type volatile _next_id = 1;  // 用于生成不重复的 timer id
    std::vector<Timer*> _timers; // 小头堆
    std::mutex _lock;
    std::condition_variable _cond;
    bool volatile _stopping = false; // 是否停止所有计时器
};

}

#endif
