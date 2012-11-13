/**
 * @file -
 * @author jingqi
 * @date 2012-07-09
 * @last-edit 2012-11-13 22:16:35 jingqi
 */

#ifndef ___HEADFILE_93CDBEEC_8BDC_4AE9_A2D1_717CAC0ECD85_
#define ___HEADFILE_93CDBEEC_8BDC_4AE9_A2D1_717CAC0ECD85_

#include <assert.h>
#include <nut/platform/platform.hpp>
#include <nut/gc/ref.hpp>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#else
#   include <pthread.h>
#   include <signal.h> // for pthread_kill()
#endif

namespace nut
{

class Thread
{
    NUT_GC_REFERABLE

public:
    typedef void (*thread_process_type)(void*);
    virtual void run(void *arg) { (void)arg; }

private:
#if defined(NUT_PLATFORM_OS_WINDOWS)
    HANDLE m_handle;
    DWORD m_tid;
#else
    pthread_t m_pthread;
#endif

    thread_process_type m_thread_process;
    void *m_thread_arg;
    bool m_hasStarted;
    bool mutable m_hasFinished;

#if defined(NUT_PLATFORM_OS_WINDOWS)
    static DWORD WINAPI thread_entry(LPVOID p)
#else
    static void* thread_entry(void *p)
#endif
    {
        assert(NULL != p);
        Thread *pthis = (Thread*) p;
        if (pthis->m_thread_process != NULL)
            pthis->m_thread_process(pthis->m_thread_arg);
        else
            pthis->run(pthis->m_thread_arg);
        pthis->m_hasFinished = true;

#if defined(NUT_PLATFORM_OS_WINDOWS)
        return 0;
#else
        return NULL;
#endif
    }

private:
    Thread(const Thread& x);
    Thread& operator=(const Thread& x);

public:
    Thread(thread_process_type process = NULL, void *arg = NULL)
        :
#if defined(NUT_PLATFORM_OS_WINDOWS)
        m_handle(NULL), m_tid(0),
#endif
        m_thread_process(process), m_thread_arg(arg),
        m_hasStarted(false), m_hasFinished(false)
    {}

    virtual ~Thread()
    {
        // 避免回收资源导致内存异常
        if (m_hasStarted)
        {
            join();
#if defined(NUT_PLATFORM_OS_WINDOWS)
            ::CloseHandle(m_handle);
#endif
        }
    }

    void setThreadProcess(thread_process_type process)
    {
        m_thread_process = process;
    }

    void setThreadArg(void *arg)
    {
        m_thread_arg = arg;
    }

    bool hasStarted() const
    {
        return m_hasStarted;
    }

    bool hasFinished() const
    {
        if (!m_hasStarted)
            return false;
        if (m_hasFinished)
            return true;

#if defined(NUT_PLATFORM_OS_WINDOWS)
        DWORD exitCode = 0;
        ::GetExitCodeThread(m_handle, &exitCode);
        if (exitCode != STILL_ACTIVE)
            m_hasFinished = true;
#else
        // send the signal 0 will just check the state, not really "kill"
        if (::pthread_kill(m_pthread, 0) != 0)
            m_hasFinished = true;
#endif
        return m_hasFinished;
    }

public:
    bool start()
    {
        if (m_hasStarted || m_hasFinished)
            return false;
        m_hasStarted = true;

#if defined(NUT_PLATFORM_OS_WINDOWS)
        m_handle = ::CreateThread(NULL, // default security attributes
                                  0, // use default stack size
                                  thread_entry, // thread function
                                  this, // argument to thread
                                  0, // use default cration flags
                                  &m_tid); // thread identifier
        if (m_handle == NULL)
        {
            m_hasFinished = true;
            return false;
        }
        return true;
#else
        int rs = ::pthread_create(&m_pthread, NULL, thread_entry, this);
        if (rs != 0)
        {
            m_hasFinished = true;
            return false;
        }
        return true;
#endif
    }

    void join()
    {
        assert(m_hasStarted);
        if (m_hasFinished)
            return;

#if defined(NUT_PLATFORM_OS_WINDOWS)
        ::WaitForSingleObject(m_handle, INFINITE);
#else
        ::pthread_join(m_pthread, NULL);
#endif
    }

    void terminate()
    {
        assert(m_hasStarted);
        if (m_hasFinished)
            return;

#if defined(NUT_PLATFORM_OS_WINDOWS)
        ::TerminateThread(m_handle, 0);
#else
        ::pthread_cancel(m_pthread);
#endif

        m_hasFinished = true;
    }
};

}

#endif

