
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
#endif

#include "thread.h"

namespace nut
{

#if NUT_PLATFORM_OS_LINUX
pid_t gettid()
{
    return ::syscall(__NR_gettid);
}
#endif

Thread::Thread(thread_process_type process, void *arg)
    : _thread_process(process), _thread_arg(arg)
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
    assert(NULL != p);
    Thread *pthis = (Thread*) p;
#if NUT_PLATFORM_OS_LINUX
    pthis->_tid = gettid();
#endif
    if (pthis->_thread_process != NULL)
        pthis->_thread_process(pthis->_thread_arg);
    else
        pthis->run(pthis->_thread_arg);
    pthis->_has_finished = true;

#if NUT_PLATFORM_OS_WINDOWS
    return 0;
#else
    return NULL;
#endif
}

void Thread::set_thread_process(thread_process_type process)
{
    _thread_process = process;
}

void Thread::set_thread_arg(void *arg)
{
    _thread_arg = arg;
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
    _handle = ::CreateThread(NULL, // default security attributes
                             0, // use default stack size
                             thread_entry, // thread function
                             this, // argument to thread
                             0, // use default cration flags
                             &_tid); // thread identifier
    if (_handle == NULL)
    {
        _has_finished = true;
        return false;
    }
    return true;
#else
    const int rs = ::pthread_create(&_pthread, NULL, thread_entry, this);
    if (rs != 0)
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
    ::pthread_join(_pthread, NULL);
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
    return ::GetCurrentThreadId();
#elif NUT_PLATFORM_OS_LINUX
    return gettid();
#else
    return ::pthread_self();
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

}
