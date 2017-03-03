
#ifndef ___HEADFILE_143AFA59_BBAB_4738_ADED_C980E5313152_
#define ___HEADFILE_143AFA59_BBAB_4738_ADED_C980E5313152_

#include <assert.h>
#include <set>
#include <queue>

#include <nut/rc/rc_new.h>

#include "../nut_config.h"
#include "thread.h"
#include "sync/condition.h"


namespace nut
{

/**
 * 线程池
 */
class NUT_API ThreadPool
{
    NUT_REF_COUNTABLE

public:
    typedef Thread::task_type task_type;

private:
    // 最大线程数，0 表示无限
    size_t _max_thread_number = 0;
    // 线程空闲多长时间后自我终止, 0 表示不自我终止
    unsigned _max_sleep_seconds = 0;
    // 已经启动的线程
    typedef  std::set<rc_ptr<Thread> > threads_set_t;
    threads_set_t _threads;
    // 当前空闲线程数
    size_t volatile _idle_number = 0;
    // 是否正在被中断
    bool volatile _interrupted = false;
    // 任务队列和同步工具
    std::queue<task_type> _task_queue;
    Condition::condition_lock_type _lock;
    Condition _wake_condition, _all_idle_condition;

private:
    // Non-copyable
    ThreadPool(const ThreadPool& x) = delete;
    ThreadPool& operator=(const ThreadPool& x) = delete;

public:
    /**
     * @param max_thread_number 最大线程数; 0 表示无限
     * @param max_sleep_seconds 线程空闲多长时间后自我终止; 0 表示一直等待
     */
    explicit ThreadPool(size_t max_thread_number = 0,
                        unsigned max_sleep_seconds = 0);
    ~ThreadPool();

    size_t get_max_thread_number() const
    {
        return _max_thread_number;
    }

    void set_max_thread_number(size_t max_thread_number);

    unsigned get_max_sleep_seconds() const
    {
        return _max_sleep_seconds;
    }

    void set_max_sleep_seconds(unsigned max_sleep_seconds);

    /**
     * 添加一个任务; 可能会启动新线程
     */
    bool add_task(const task_type& task);

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

    /**
     * 强制终止所有线程
     */
    void terminate();

private:
    void thread_process(const rc_ptr<Thread>& thread);
    void release_thread(const rc_ptr<Thread>& thread);
};

}

#endif
