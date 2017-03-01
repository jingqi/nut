
#include <assert.h>

#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#elif NUT_PLATFORM_OS_LINUX
#   include <pthread.h>
#   include <signal.h> // for ::pthread_kill()
#   include <unistd.h> // for ::syscall()
#   include <sys/syscall.h> // for definition of __NR_*
#else
#   include <pthread.h>
#   include <signal.h> // for ::pthread_kill()
#   include <unistd.h> // for ::usleep()
#endif

#include "thread.h"
#include "threading.h"

namespace nut
{

#if NUT_PLATFORM_OS_WINDOWS || NUT_PLATFORM_OS_LINUX
static NUT_THREAD_LOCAL Thread::tid_type s_cached_tid = 0;
#else
// pthread_t should not be initialized by manually
static NUT_THREAD_LOCAL bool s_cached_tid_initialized = false;
static NUT_THREAD_LOCAL Thread::tid_type s_cached_tid;
#endif

#if NUT_PLATFORM_OS_LINUX
static pid_t gettid()
{
    return ::syscall(__NR_gettid);
}
#endif

Thread::Thread(const task_type& task)
    : _task(task)
{}

Thread::~Thread()
{
    // 避免回收资源导致内存异常
    if (_has_started)
    {
        join();
#if NUT_PLATFORM_OS_WINDOWS
        ::CloseHandle(_handle);
#else
        ::pthread_detach(_pthread);
#endif
    }
}

#if NUT_PLATFORM_OS_WINDOWS
DWORD WINAPI Thread::thread_entry(LPVOID p)
#else
void* Thread::thread_entry(void *p)
#endif
{
    assert(nullptr != p);
    Thread *pthis = (Thread*) p;
#if NUT_PLATFORM_OS_LINUX
    pthis->_tid = gettid();
#endif
    if (pthis->_task)
        pthis->_task();
    else
        pthis->run();
    pthis->_has_finished = true;

#if NUT_PLATFORM_OS_WINDOWS
    return 0;
#else
    return nullptr;
#endif
}

void Thread::set_thread_task(const task_type& task)
{
    _task = task;
}

bool Thread::has_started() const
{
    return _has_started;
}

bool Thread::has_finished() const
{
    if (!_has_started)
        return false;
    if (_has_finished)
        return true;

#if NUT_PLATFORM_OS_WINDOWS
    DWORD exit_code = 0;
    ::GetExitCodeThread(_handle, &exit_code);
    if (exit_code != STILL_ACTIVE)
        _has_finished = true;
#else
    // send the signal 0 will just check the state, not really "kill"
    if (::pthread_kill(_pthread, 0) != 0)
        _has_finished = true;
#endif
    return _has_finished;
}

Thread::tid_type Thread::get_tid() const
{
    assert(_has_started);
#if NUT_PLATFORM_OS_WINDOWS || NUT_PLATFORM_OS_LINUX
    return _tid;
#else
    return _pthread;
#endif
}

bool Thread::start()
{
    if (_has_started || _has_finished)
        return false;
    _has_started = true;

#if NUT_PLATFORM_OS_WINDOWS
    _handle = ::CreateThread(nullptr,      // Default security attributes
                             0,            // Use default stack size
                             thread_entry, // thread function
                             this,         // Argument to thread
                             0,            // Use default cration flags
                             &_tid);       // Thread identifier
    if (nullptr == _handle)
    {
        _has_finished = true;
        return false;
    }
    return true;
#else
    const int rs = ::pthread_create(&_pthread, nullptr, thread_entry, this);
    if (0 != rs)
    {
        _has_finished = true;
        return false;
    }
    return true;
#endif
}

void Thread::join()
{
    assert(_has_started);
    if (_has_finished)
        return;

#if NUT_PLATFORM_OS_WINDOWS
    ::WaitForSingleObject(_handle, INFINITE);
#else
    ::pthread_join(_pthread, nullptr);
#endif
}

void Thread::terminate()
{
    assert(_has_started);
    if (_has_finished)
        return;

#if NUT_PLATFORM_OS_WINDOWS
    ::TerminateThread(_handle, 0);
#else
    ::pthread_cancel(_pthread);
#endif

    _has_finished = true;
}

Thread::tid_type Thread::current_thread_id()
{
#if NUT_PLATFORM_OS_WINDOWS
    if (0 == s_cached_tid)
        s_cached_tid = ::GetCurrentThreadId();
    return s_cached_tid;
#elif NUT_PLATFORM_OS_LINUX
    if (0 == s_cached_tid)
        s_cached_tid = gettid();
    return s_cached_tid;
#else
    if (!s_cached_tid_initialized)
    {
        s_cached_tid = ::pthread_self();
        s_cached_tid_initialized = true;
    }
    return s_cached_tid;
#endif
}

bool Thread::tid_equals(const tid_type &t1, const tid_type &t2)
{
#if NUT_PLATFORM_OS_WINDOWS || NUT_PLATFORM_OS_LINUX
    return t1 == t2;
#else
    return ::pthread_equal(t1, t2);
#endif
}

void Thread::sleep(unsigned long long ms)
{
#if NUT_PLATFORM_OS_WINDOWS
    ::Sleep(ms);
#else
    ::usleep(ms * 1000);
#endif
}

}
