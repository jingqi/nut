
#ifndef ___HEADFILE_C26E9AEE_1281_403B_A44C_5CF82E61DD5C_
#define ___HEADFILE_C26E9AEE_1281_403B_A44C_5CF82E61DD5C_

#include <assert.h>
#include <vector>
#include <time.h>

#include <nut/platform/platform.h>
#include <nut/threading/sync/condition.h>
#include <nut/threading/thread.h>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#else
#   include <sys/time.h>
#endif

#include "time_val.h"

namespace nut
{

/**
 * 不太精确的用户级定时器
 */
class TimerManager
{
public:
    // 定时器回调函数类型
    typedef void (*timer_func_type)(int,void*);

public:
    // 定时器
    struct Timer
    {
        int id;
        TimeVal time;
        void *arg;
        timer_func_type func;
    };

    // 用于维护小头堆
    struct TimerPtrComparor
    {
        bool operator()(const Timer *t1, const Timer *t2)
        {
            assert(NULL != t1 && NULL != t2);
            return t1->time > t2->time; // 用于小头堆算法，故使用">"
        }
    };

    int volatile m_next_id;  // 用于生成不重复的 timer id
    std::vector<Timer*> m_timers; // 小头堆
    Condition m_cond;
    typedef Condition::condition_lock_type lock_type;
    lock_type m_lock;
    bool volatile m_stopping; // 是否停止所有计时器

public:
    TimerManager();
    ~TimerManager();

    /**
     * 添加定时器
     *
     * @param t 距离现在时间的间隔,单位毫秒
     * @return 定时器id
     */
    int add_timer(const TimeVal& interval, timer_func_type func, void *arg = NULL);

    bool cancel_timer(int id);

    void interupt();

    /**
     * 主定时器线程，将阻塞线程，直到 interupt() 被调用
     */
    void run();
};

}

#endif
