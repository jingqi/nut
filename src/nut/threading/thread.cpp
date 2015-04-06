
#include <assert.h>
#include <nut/platform/platform.h>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#else
#   include <pthread.h>
#   include <signal.h> // for pthread_kill()
#endif

#include "thread.h"

namespace nut
{

#if defined(NUT_PLATFORM_OS_WINDOWS)
DWORD WINAPI Thread::thread_entry(LPVOID p)
#else
void* Thread::thread_entry(void *p)
#endif
{
    assert(NULL != p);
    Thread *pthis = (Thread*) p;
    if (pthis->m_thread_process != NULL)
        pthis->m_thread_process(pthis->m_thread_arg);
    else
        pthis->run(pthis->m_thread_arg);
    pthis->m_has_finished = true;

#if defined(NUT_PLATFORM_OS_WINDOWS)
    return 0;
#else
    return NULL;
#endif
}

Thread::Thread(thread_process_type process, void *arg)
    :
#if defined(NUT_PLATFORM_OS_WINDOWS)
    m_handle(NULL), m_tid(0),
#endif
    m_thread_process(process), m_thread_arg(arg),
    m_has_started(false), m_has_finished(false)
{}

Thread::~Thread()
{
    // 避免回收资源导致内存异常
    if (m_has_started)
    {
        join();
#if defined(NUT_PLATFORM_OS_WINDOWS)
        ::CloseHandle(m_handle);
#else
        ::pthread_detach(m_pthread);
#endif
    }
}

bool Thread::has_finished() const
{
    if (!m_has_started)
        return false;
    if (m_has_finished)
        return true;

#if defined(NUT_PLATFORM_OS_WINDOWS)
    DWORD exit_code = 0;
    ::GetExitCodeThread(m_handle, &exit_code);
    if (exit_code != STILL_ACTIVE)
        m_has_finished = true;
#else
    // send the signal 0 will just check the state, not really "kill"
    if (::pthread_kill(m_pthread, 0) != 0)
        m_has_finished = true;
#endif
    return m_has_finished;
}

bool Thread::start()
{
    if (m_has_started || m_has_finished)
        return false;
    m_has_started = true;

#if defined(NUT_PLATFORM_OS_WINDOWS)
    m_handle = ::CreateThread(NULL, // default security attributes
                              0, // use default stack size
                              thread_entry, // thread function
                              this, // argument to thread
                              0, // use default cration flags
                              &m_tid); // thread identifier
    if (m_handle == NULL)
    {
        m_has_finished = true;
        return false;
    }
    return true;
#else
    const int rs = ::pthread_create(&m_pthread, NULL, thread_entry, this);
    if (rs != 0)
    {
        m_has_finished = true;
        return false;
    }
    return true;
#endif
}

void Thread::join()
{
    assert(m_has_started);
    if (m_has_finished)
        return;

#if defined(NUT_PLATFORM_OS_WINDOWS)
    ::WaitForSingleObject(m_handle, INFINITE);
#else
    ::pthread_join(m_pthread, NULL);
#endif
}

void Thread::terminate()
{
    assert(m_has_started);
    if (m_has_finished)
        return;

#if defined(NUT_PLATFORM_OS_WINDOWS)
    ::TerminateThread(m_handle, 0);
#else
    ::pthread_cancel(m_pthread);
#endif

    m_has_finished = true;
}

}
