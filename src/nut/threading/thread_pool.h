
#ifndef ___HEADFILE_143AFA59_BBAB_4738_ADED_C980E5313152_
#define ___HEADFILE_143AFA59_BBAB_4738_ADED_C980E5313152_

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
class NUT_API ThreadPool
{
    NUT_REF_COUNTABLE

public:
    typedef std::function<void()> task_type;

public:
    /**
     * @param max_thread_number 最大线程数; 0 表示无限个
     * @param max_sleep_seconds 线程空闲多长时间后自我终止; 0 表示无限长时间
     */
    explicit ThreadPool(size_t max_thread_number = 0,
                        unsigned max_sleep_seconds = 300);
    ~ThreadPool();

    size_t get_max_thread_number() const;
    void set_max_thread_number(size_t max_thread_number);

    size_t get_busy_thread_number();

    unsigned get_max_sleep_seconds() const;
    void set_max_sleep_seconds(unsigned max_sleep_seconds);

    /**
     * 添加一个任务; 可能会启动新线程
     */
    bool add_task(task_type&& task);
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

private:
    // Non-copyable
    ThreadPool(const ThreadPool& x) = delete;
    ThreadPool& operator=(const ThreadPool& x) = delete;

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
    std::queue<task_type> _task_queue;
    std::mutex _lock;
    std::condition_variable _wake_condition, _all_idle_condition;
};

}

#endif
