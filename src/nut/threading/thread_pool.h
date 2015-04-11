
#ifndef ___HEADFILE_143AFA59_BBAB_4738_ADED_C980E5313152_
#define ___HEADFILE_143AFA59_BBAB_4738_ADED_C980E5313152_

#include <assert.h>
#include <vector>
#include <queue>

#include <nut/rc/rc_new.h>

#include "thread.h"
#include "sync/condition.h"

namespace nut
{

/**
 * 线程池
 */
class ThreadPool
{
    NUT_REF_COUNTABLE

public:
    typedef Thread::thread_process_type thread_process_type;

private:
    struct Task
    {
        thread_process_type process;
        void *arg;

        Task(thread_process_type p = NULL, void *a = NULL)
            : process(p), arg(a)
        {}
    };

    size_t m_thread_count;
    std::vector<rc_ptr<Thread> > m_threads;
    bool volatile m_interupt;
    std::queue<Task> m_task_queue;
    Condition::condition_lock_type m_lock;
    Condition m_condition;

public:
    ThreadPool(size_t thread_count);

    ~ThreadPool();

    bool add_task(thread_process_type process, void* arg = NULL);

    void start();

    void interupt();

    void join();

    void terminate();

private:
    static void thread_process(void *p);
};

}

#endif
