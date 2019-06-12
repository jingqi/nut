﻿
#include <assert.h>
#include <vector>

#include "priority_thread_pool.h"


namespace nut
{

PriorityThreadPool::PriorityThreadPool(size_t max_thread_number, unsigned max_sleep_seconds) noexcept
    : _max_thread_number(max_thread_number), _max_sleep_seconds(max_sleep_seconds)
{}

PriorityThreadPool::~PriorityThreadPool() noexcept
{
    // 为避免内存问题，必须等所有线程退出后再析构
    wait_until_all_idle();
    interrupt();
    join();
}

size_t PriorityThreadPool::get_max_thread_number() const noexcept
{
    return _max_thread_number;
}

void PriorityThreadPool::set_max_thread_number(size_t max_thread_number) noexcept
{
    _max_thread_number = max_thread_number;
    _wake_condition.notify_all();
}

size_t PriorityThreadPool::get_busy_thread_number() noexcept
{
    std::lock_guard<std::mutex> guard(_lock);
    return _alive_number - _idle_number;
}

unsigned PriorityThreadPool::get_max_sleep_seconds() const noexcept
{
    return _max_sleep_seconds;
}

void PriorityThreadPool::set_max_sleep_seconds(unsigned max_sleep_seconds) noexcept
{
    _max_sleep_seconds = max_sleep_seconds;
    _wake_condition.notify_all();
}

bool PriorityThreadPool::add_task(task_type&& task, priority_type priority) noexcept
{
    assert(task);

    if (_interrupted)
        return false;

    std::lock_guard<std::mutex> guard(_lock);

    // 将任务入队
    _task_queue.emplace(std::forward<task_type>(task), priority);
    _wake_condition.notify_one();

    // 启动新线程
    if (!_interrupted && 0 == _idle_number &&
        (0 == _max_thread_number || _alive_number < _max_thread_number))
    {
        if (_threads.size() > _alive_number * 2 + 10)
            clean_dead_threads();

        _threads.emplace_back([=] { thread_process(); });
        ++_alive_number;
    }

    return true;
}

bool PriorityThreadPool::add_task(const task_type& task, priority_type priority) noexcept
{
    assert(task);

    if (_interrupted)
        return false;

    std::lock_guard<std::mutex> guard(_lock);

    // 将任务入队
    _task_queue.emplace(task, priority);
    _wake_condition.notify_one();

    // 启动新线程
    if (!_interrupted && 0 == _idle_number &&
        (0 == _max_thread_number || _alive_number < _max_thread_number))
    {
        if (_threads.size() > _alive_number * 2 + 10)
            clean_dead_threads();

        _threads.emplace_back([=] { thread_process(); });
        ++_alive_number;
    }

    return true;
}

void PriorityThreadPool::wait_until_all_idle() noexcept
{
    std::unique_lock<std::mutex> unique_guard(_lock);
    _all_idle_condition.wait(
        unique_guard, [=] {return _alive_number == _idle_number;});
}

void PriorityThreadPool::interrupt() noexcept
{
    _interrupted = true;
    _wake_condition.notify_all();
}

void PriorityThreadPool::join() noexcept
{
    for (thread_iter_type iter = _threads.begin(),
             end = _threads.end(); iter != end; ++iter)
    {
        if (iter->joinable())
            iter->join();
    }
}

void PriorityThreadPool::thread_process() noexcept
{
    while (true)
    {
        task_type task;

        {
            std::unique_lock<std::mutex> unique_guard(_lock);

            // Wait for conditions
            while (!_interrupted && _task_queue.empty() &&
                   (0 == _max_thread_number || _alive_number <= _max_thread_number))
            {
                if (_alive_number == ++_idle_number)
                    _all_idle_condition.notify_all();

                if (0 == _max_sleep_seconds)
                {
                    _wake_condition.wait(unique_guard);
                    --_idle_number;
                }
                else
                {
                    const std::cv_status rs = _wake_condition.wait_for(
                        unique_guard, std::chrono::milliseconds(_max_sleep_seconds * 1000));
                    --_idle_number;
                    if (std::cv_status::timeout == rs)
                    {
                        // Idle timeout, thread should be released
                        thread_finalize();
                        return;
                    }
                }
            }

            // Wake by interruption or max thread number changed
            if (_interrupted ||
                (0 != _max_thread_number && _alive_number > _max_thread_number))
            {
                thread_finalize();
                return;
            }

            // Wake by new task
            assert(!_task_queue.empty());
            task = _task_queue.top().task;
            _task_queue.pop();
        }

        task();
    }
}

void PriorityThreadPool::thread_finalize() noexcept
{
    if (--_alive_number == _idle_number)
        _all_idle_condition.notify_all();
}

void PriorityThreadPool::clean_dead_threads() noexcept
{
    for (thread_iter_type iter = _threads.begin(),
             end =_threads.end(); iter != end;)
    {
        if (iter->joinable())
            ++iter;
        else
            _threads.erase(iter++);
    }
}

}
