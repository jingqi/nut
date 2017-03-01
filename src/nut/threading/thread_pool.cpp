
#include <assert.h>
#include <vector>

#include "thread_pool.h"
#include "sync/guard.h"


namespace nut
{

ThreadPool::ThreadPool(size_t max_thread_count, unsigned max_sleep_seconds)
    : _max_thread_count(max_thread_count), _max_sleep_seconds(max_sleep_seconds)
{}

ThreadPool::~ThreadPool()
{
    // 避免内存问题
    join();
}

bool ThreadPool::add_task(const task_type& task)
{
    assert(task);

    if (_interrupt)
        return false;

    Guard<Condition::condition_lock_type> guard(&_lock);

    // 将任务入队
    _task_queue.push(task);
    _condition.signal();

    // 启动新线程
    if (!_interrupt && !_joined &&
        (0 == _max_thread_count || _threads.size() < _max_thread_count) &&
        0 == _idle_count)
    {
        rc_ptr<Thread> thread = rc_new<Thread>();
        thread->set_thread_task([=] { thread_process(thread); });
        _threads.insert(thread);
        thread->start();
    }

    return true;
}

void ThreadPool::interrupt()
{
    _interrupt = true;
    _condition.broadcast();
}

void ThreadPool::join()
{
    std::vector<rc_ptr<Thread> > threads;
    {
        Guard<Condition::condition_lock_type> guard(&_lock);
        for (threads_set_t::const_iterator iter = _threads.begin(),
             end = _threads.end(); iter != end; ++iter)
        {
            threads.push_back(*iter);
        }
    }

    _joined = true;
    _condition.broadcast();
    for (size_t i = 0; i < threads.size(); ++i)
        threads[i]->join();
    _joined = false;
}

void ThreadPool::terminate()
{
    _interrupt = true;

    Guard<Condition::condition_lock_type> guard(&_lock);

    for (threads_set_t::const_iterator iter = _threads.begin(),
         end = _threads.end(); iter != end; ++iter)
    {
        (*iter)->terminate();
    }
    _threads.clear();
    _idle_count = 0;
}

void ThreadPool::thread_process(const rc_ptr<Thread>& thread)
{
    while (true)
    {
        task_type task;

        {
            Guard<Condition::condition_lock_type> guard(&_lock);

            // Max thread count may have been shrank
            if (_interrupt ||
                (_joined && _task_queue.empty()) ||
                (0 != _max_thread_count && _threads.size() > _max_thread_count))
            {
                _threads.erase(_threads.find(thread));
                return;
            }

            // Wait for conditions
            ++_idle_count;
            while (!_interrupt && !_joined && _task_queue.empty())
            {
                if (0 == _max_sleep_seconds)
                {
                    _condition.wait(&_lock);
                }
                else
                {
                    const bool rs = _condition.timedwait(&_lock, _max_sleep_seconds);
                    if (!rs)
                    {
                        // Sleep timeout, thread should be released
                        --_idle_count;
                        _threads.erase(_threads.find(thread));
                        return;
                    }
                }
            }
            --_idle_count;

            // Wake by interruption or joined(and there are no new tasks)
            if (_interrupt || _task_queue.empty())
            {
                assert(_interrupt || _joined);
                _threads.erase(_threads.find(thread));
                return;
            }

            // Wake by new task
            task = _task_queue.front();
            _task_queue.pop();
        }

        task();
    }
}

}
