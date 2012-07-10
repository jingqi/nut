/**
 * @file -
 * @author jingqi
 * @date 2012-07-10
 * @last-edit 2012-07-10 19:34:28 jingqi
 */

#ifndef ___HEADFILE_143AFA59_BBAB_4738_ADED_C980E5313152_
#define ___HEADFILE_143AFA59_BBAB_4738_ADED_C980E5313152_

#include <assert.h>
#include <vector>
#include <queue>

#include <nut/gc/gc.hpp>

#include "thread.hpp"
#include "sync/condition.hpp"
#include "sync/guard.hpp"

namespace nut
{

/**
 * Ïß³Ì³Ø
 */
class ThreadPool
{
    NUT_GC_REFERABLE

public:
    typedef Thread::thread_process_type thread_process_type;

    struct Task
    {
        thread_process_type process;
        void *arg;
        Task(thread_process_type p = NULL, void *a = NULL) : process(p), arg(a) {}
    };

    size_t m_thread_count;
    std::vector<ref<Thread> > m_threads;
    bool volatile m_interupt;
    std::queue<Task> m_task_queue;
    Condition::condition_lock_type m_lock;
    Condition m_condition;

public:
    ThreadPool(size_t thread_count)
        : m_thread_count(thread_count), m_interupt(false)
    {
        assert(thread_count > 0);
    }

    void add_task(thread_process_type process, void* arg = NULL)
    {
        assert(NULL != process);
        Guard<Condition::condition_lock_type> guard(&m_lock);
        m_task_queue.push(Task(process, arg));
        m_condition.signal();
    }

    void start()
    {
        for (size_t i = m_threads.size(); i < m_thread_count; ++i)
        {
            ref<Thread> t = gc_new<Thread>(thread_process, this);
            m_threads.push_back(t);
            t->start();
        }
    }

    void interupt()
    {
        m_interupt = true;
    }

private:
    static void thread_process(void *p)
    {
        assert(NULL != p);
        ThreadPool *pthis = (ThreadPool*) p;

        do
        {
            Task t;
            {
                Guard<Condition::condition_lock_type> guard(&(pthis->m_lock));
                while (pthis->m_task_queue.empty())
                    pthis->m_condition.wait(&(pthis->m_lock));
                t = pthis->m_task_queue.front();
                pthis->m_task_queue.pop();
            }

            t.process(t.arg);
        } while (!(pthis->m_interupt));
    }
};

}

#endif

