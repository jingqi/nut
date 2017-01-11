
#ifndef ___HEADFILE_143AFA59_BBAB_4738_ADED_C980E5313152_
#define ___HEADFILE_143AFA59_BBAB_4738_ADED_C980E5313152_

#include <assert.h>
#include <vector>
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
    size_t _thread_count = 1;
    std::vector<rc_ptr<Thread> > _threads;
    bool volatile _interrupt = false;
    std::queue<task_type> _task_queue;
    Condition::condition_lock_type _lock;
    Condition _condition;

private:
    // Non-copyable
    ThreadPool(const ThreadPool& x) = delete;
    ThreadPool& operator=(const ThreadPool& x) = delete;

public:
    explicit ThreadPool(size_t thread_count);
    ~ThreadPool();

    bool add_task(const task_type& task);

    void start();

    void interrupt();

    void join();

    void terminate();

private:
    void thread_process();
};

}

#endif
