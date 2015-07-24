
#include <assert.h>

#include "thread_pool.h"
#include "sync/guard.h"

namespace nut
{

ThreadPool::ThreadPool(size_t thread_count)
    : _thread_count(thread_count)
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
    Guard<Condition::condition_lock_type> guard(&_lock);
    if (_interrupt)
        return false;

    _task_queue.push(Task(process, arg));
    _condition.signal();
    return true;
}

void ThreadPool::start()
{
    for (size_t i = _threads.size(); i < _thread_count; ++i)
    {
        rc_ptr<Thread> t = rc_new<Thread>(thread_process, this);
        _threads.push_back(t);
        t->start();
    }
}

void ThreadPool::interupt()
{
    _interrupt = true;
    _condition.broadcast();
}

void ThreadPool::join()
{
    for (size_t i = 0; i < _threads.size(); ++i)
        _threads[i]->join();
}

void ThreadPool::terminate()
{
    _interrupt = true;
    for (size_t i = 0; i < _threads.size(); ++i)
        _threads[i]->terminate();
}

void ThreadPool::thread_process(void *p)
{
    assert(NULL != p);
    ThreadPool *pthis = (ThreadPool*) p;

    do
    {
        Task t;
        {
            Guard<Condition::condition_lock_type> guard(&(pthis->_lock));
            while (!pthis->_interrupt && pthis->_task_queue.empty())
                pthis->_condition.wait(&(pthis->_lock));
            if (pthis->_interrupt)
                continue;
            t = pthis->_task_queue.front();
            pthis->_task_queue.pop();
        }

        t.process(t.arg);
    } while (!(pthis->_interrupt));
}

}
