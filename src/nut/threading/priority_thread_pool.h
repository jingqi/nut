﻿
#ifndef ___HEADFILE_6F76EF90_9121_442B_AC9B_88A4404497F8_
#define ___HEADFILE_6F76EF90_9121_442B_AC9B_88A4404497F8_

#include <assert.h>
#include <list>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <nut/rc/rc_new.h>

#include "../nut_config.h"


namespace nut
{

/**
 * 线程池
 */
class NUT_API PriorityThreadPool
{
    NUT_REF_COUNTABLE

public:
    typedef int priority_type;
    typedef std::function<void()> task_type;

private:
    class Task
    {
    public:
        explicit Task(task_type&& t, priority_type p = 0)
            : task(t), priority(p)
        {}

        explicit Task(const task_type& t, priority_type p = 0)
            : task(t), priority(p)
        {}

        bool operator<(const Task& x) const
        {
            return priority < x.priority;
        }

    public:
        task_type task;
        priority_type priority; // 数值越大，优先级越高
    };

public:
    /**
     * @param max_thread_number 最大线程数; 0 表示无限个
     * @param max_sleep_seconds 线程空闲多长时间后自我终止; 0 表示无限长时间
     */
    explicit PriorityThreadPool(size_t max_thread_number = 0,
                                unsigned max_sleep_seconds = 300);
    ~PriorityThreadPool();

    size_t get_max_thread_number() const;
    void set_max_thread_number(size_t max_thread_number);

    size_t get_busy_thread_number();

    unsigned get_max_sleep_seconds() const;
    void set_max_sleep_seconds(unsigned max_sleep_seconds);

    /**
     * 添加一个任务; 可能会启动新线程
     */
    bool add_task(task_type&& task, priority_type priority = 0);
    bool add_task(const task_type& task, priority_type priority = 0);

    /**
     * 阻塞，直到所有线程都空闲
     */
    void wait_until_all_idle();

    /**
     * 给所有线程发送中断信号
     */
    void interrupt();

    /**
     * 等待所有线程退出
     */
    void join();

private:
    PriorityThreadPool(const PriorityThreadPool& x) = delete;
    PriorityThreadPool& operator=(const PriorityThreadPool& x) = delete;

    void thread_process();
    void thread_finalize();
    void clean_dead_threads();

private:
    // 最大线程数，0 表示无限
    size_t volatile _max_thread_number = 0;

    // 线程空闲多长时间后自我终止, 0 表示不自我终止
    unsigned volatile _max_sleep_seconds = 0;

    // 活动线程
    typedef std::list<std::thread> thread_list_type;
    typedef typename thread_list_type::iterator thread_iter_type;
    thread_list_type _threads;

    // 线程数
    size_t volatile _alive_number = 0;
    size_t volatile _idle_number = 0;

    // 是否正在被中断
    bool volatile _interrupted = false;

    // 任务队列和同步工具
    std::priority_queue<Task> _task_queue; // 默认最大元素出现在 top() 位置
    std::mutex _lock;
    std::condition_variable _wake_condition, _all_idle_condition;
};

}

#endif
