
#include <assert.h>
#include <vector>

#include "thread_pool.h"
#include "sync/guard.h"


namespace nut
{

ThreadPool::ThreadPool(size_t max_thread_number, unsigned max_sleep_seconds)
    : _max_thread_number(max_thread_number), _max_sleep_seconds(max_sleep_seconds)
{}

ThreadPool::~ThreadPool()
{
    // 为避免内存问题，必须等所有线程退出后再析构
    wait_until_all_idle();
    interrupt();
    join();
}

void ThreadPool::set_max_thread_number(size_t max_thread_number)
{
    _max_thread_number = max_thread_number;
    _wake_condition.broadcast();
}

void ThreadPool::set_max_sleep_seconds(unsigned max_sleep_seconds)
{
    _max_sleep_seconds = max_sleep_seconds;
    _wake_condition.broadcast();
}

bool ThreadPool::add_task(const task_type& task)
{
    assert(task);

    if (_interrupted)
        return false;

    Guard<Condition::condition_lock_type> guard(&_lock);

    // 将任务入队
    _task_queue.push(task);
    _wake_condition.signal();

    // 启动新线程
    if (!_interrupted &&
        (0 == _max_thread_number || _threads.size() < _max_thread_number) &&
        0 == _idle_number)
    {
        rc_ptr<Thread> thread = rc_new<Thread>();
        thread->set_thread_task([=] { thread_process(thread); });
        _threads.insert(thread);
        thread->start();
    }

    return true;
}

void ThreadPool::wait_until_all_idle()
{
    Guard<Condition::condition_lock_type> guard(&_lock);
    while (_threads.size() != _idle_number)
        _all_idle_condition.wait(&_lock);
}

void ThreadPool::interrupt()
{
    _interrupted = true;
    _wake_condition.broadcast();
}

void ThreadPool::join()
{
    while (true)
    {
        rc_ptr<Thread> t;

        {
            Guard<Condition::condition_lock_type> guard(&_lock);
            if (_threads.empty())
                return;
            t = *_threads.begin();
        }

        t->join();
    }
}

void ThreadPool::terminate()
{
    _interrupted = true;

    Guard<Condition::condition_lock_type> guard(&_lock);

    for (thread_container_type::const_iterator iter = _threads.begin(),
         end = _threads.end(); iter != end; ++iter)
    {
        (*iter)->terminate();
    }
    _threads.clear();
    _idle_number = 0;
}

void ThreadPool::thread_process(const thread_handle_type& thread)
{
    while (true)
    {
        task_type task;

        {
            Guard<Condition::condition_lock_type> guard(&_lock);

            // Wait for conditions
            while (!_interrupted &&
                   (0 == _max_thread_number || _threads.size() <= _max_thread_number) &&
                   _task_queue.empty())
            {
                ++_idle_number;
                if (_threads.size() == _idle_number)
                    _all_idle_condition.signal();

                if (0 == _max_sleep_seconds)
                {
                    _wake_condition.wait(&_lock);
                    --_idle_number;
                }
                else
                {
                    const bool rs = _wake_condition.timedwait(&_lock, _max_sleep_seconds);
                    --_idle_number;
                    if (!rs)
                    {
                        // Idle timeout, thread should be released
                        release_thread(thread);
                        return;
                    }
                }
            }

            // Wake by interruption or max thread number changed
            if (_interrupted ||
                (0 != _max_thread_number && _threads.size() > _max_thread_number))
            {
                release_thread(thread);
                return;
            }

            // Wake by new task
            assert(!_task_queue.empty());
            task = _task_queue.front();
            _task_queue.pop();
        }

        task();
    }
}

void ThreadPool::release_thread(const thread_handle_type& thread)
{
    // Should run under protection of lock
    _threads.erase(_threads.find(thread));

    if (_threads.size() == _idle_number)
        _all_idle_condition.signal();
}

}
