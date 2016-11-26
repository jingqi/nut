
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
    typedef Thread::thread_process_type thread_process_type;

private:
    class Task
    {
    public:
        thread_process_type process = NULL;
        void *arg = NULL;

        Task(thread_process_type p = NULL, void *a = NULL)
            : process(p), arg(a)
        {}
    };

    size_t _thread_count = 1;
    std::vector<rc_ptr<Thread> > _threads;
    bool volatile _interrupt = false;
    std::queue<Task> _task_queue;
    Condition::condition_lock_type _lock;
    Condition _condition;

public:
    ThreadPool(size_t thread_count);

    ~ThreadPool();

    bool add_task(thread_process_type process, void* arg = NULL);

    void start();

    void interrupt();

    void join();

    void terminate();

private:
    static void thread_process(void *p);
};

}

#endif
