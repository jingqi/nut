/**
 * @file -
 * @author jingqi
 * @date 2012-07-09
 * @last-edit 2015-01-06 23:55:58 jingqi
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
    bool m_has_started;
    bool mutable m_has_finished;

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
        pthis->m_has_finished = true;

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
        m_has_started(false), m_has_finished(false)
    {}

    virtual ~Thread()
    {
        // 避免回收资源导致内存异常
        if (m_has_started)
        {
            join();
#if defined(NUT_PLATFORM_OS_WINDOWS)
            ::CloseHandle(m_handle);
#endif
        }
    }

    void set_thread_process(thread_process_type process)
    {
        m_thread_process = process;
    }

    void set_thread_arg(void *arg)
    {
        m_thread_arg = arg;
    }

    bool has_started() const
    {
        return m_has_started;
    }

    bool has_finished() const
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

public:
    bool start()
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
        int rs = ::pthread_create(&m_pthread, NULL, thread_entry, this);
        if (rs != 0)
        {
            m_has_finished = true;
            return false;
        }
        return true;
#endif
    }

    void join()
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

    void terminate()
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
};

}

#endif
