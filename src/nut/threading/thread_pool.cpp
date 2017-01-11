
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

bool ThreadPool::add_task(const task_type& task)
{
    assert(task);
    Guard<Condition::condition_lock_type> guard(&_lock);
    if (_interrupt)
        return false;

    _task_queue.push(task);
    _condition.signal();
    return true;
}

void ThreadPool::start()
{
    for (size_t i = _threads.size(); i < _thread_count; ++i)
    {
        rc_ptr<Thread> thread = rc_new<Thread>([=] { thread_process(); });
        _threads.push_back(thread);
        thread->start();
    }
}

void ThreadPool::interrupt()
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

void ThreadPool::thread_process()
{
    do
    {
        task_type task;
        {
            Guard<Condition::condition_lock_type> guard(&_lock);
            while (!_interrupt && _task_queue.empty())
                _condition.wait(&_lock);
            if (_interrupt)
                continue;
            task = _task_queue.front();
            _task_queue.pop();
        }

        task();
    } while (!_interrupt);
}

}
