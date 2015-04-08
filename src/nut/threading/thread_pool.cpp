
#include <assert.h>

#include "thread_pool.h"
#include "sync/guard.hpp"

namespace nut
{

ThreadPool::ThreadPool(size_t thread_count)
    : m_thread_count(thread_count), m_interupt(false)
{
    assert(thread_count > 0);
}

ThreadPool::~ThreadPool()
{
    // 避免内存问题
    join();
}

bool ThreadPool::add_task(thread_process_type process, void* arg)
{
    assert(NULL != process);
    Guard<Condition::condition_lock_type> guard(&m_lock);
    if (m_interupt)
        return false;

    m_task_queue.push(Task(process, arg));
    m_condition.signal();
    return true;
}

void ThreadPool::start()
{
    for (size_t i = m_threads.size(); i < m_thread_count; ++i)
    {
        rc_ptr<Thread> t = RC_NEW(NULL, Thread, thread_process, this);
        m_threads.push_back(t);
        t->start();
    }
}

void ThreadPool::interupt()
{
    m_interupt = true;
    m_condition.broadcast();
}

void ThreadPool::join()
{
    for (size_t i = 0; i < m_threads.size(); ++i)
        m_threads[i]->join();
}

void ThreadPool::terminate()
{
    m_interupt = true;
    for (size_t i = 0; i < m_threads.size(); ++i)
        m_threads[i]->terminate();
}

void ThreadPool::thread_process(void *p)
{
    assert(NULL != p);
    ThreadPool *pthis = (ThreadPool*) p;

    do
    {
        Task t;
        {
            Guard<Condition::condition_lock_type> guard(&(pthis->m_lock));
            while (!pthis->m_interupt && pthis->m_task_queue.empty())
                pthis->m_condition.wait(&(pthis->m_lock));
            if (pthis->m_interupt)
                continue;
            t = pthis->m_task_queue.front();
            pthis->m_task_queue.pop();
        }

        t.process(t.arg);
    } while (!(pthis->m_interupt));
}

}
