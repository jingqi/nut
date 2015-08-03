
#ifndef ___HEADFILE_93CDBEEC_8BDC_4AE9_A2D1_717CAC0ECD85_
#define ___HEADFILE_93CDBEEC_8BDC_4AE9_A2D1_717CAC0ECD85_

#include <nut/platform/platform.h>
#include <nut/rc/rc_ptr.h>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#else
#   include <pthread.h>
#endif

namespace nut
{

class Thread
{
    NUT_REF_COUNTABLE

public:
    typedef void (*thread_process_type)(void*);

    virtual void run(void *arg)
    {
        (void)arg;
    }

private:
#if defined(NUT_PLATFORM_OS_WINDOWS)
    HANDLE _handle = NULL;
    DWORD _tid = 0;
#else
    pthread_t _pthread;
#endif

    thread_process_type _thread_process = NULL;
    void *_thread_arg = NULL;
    bool _has_started = false;
    bool mutable _has_finished = false;

#if defined(NUT_PLATFORM_OS_WINDOWS)
    static DWORD WINAPI thread_entry(LPVOID p);
#else
    static void* thread_entry(void *p);
#endif

private:
    Thread(const Thread& x);
    Thread& operator=(const Thread& x);

public:
    Thread(thread_process_type process = NULL, void *arg = NULL);
    virtual ~Thread();

    void set_thread_process(thread_process_type process)
    {
        _thread_process = process;
    }

    void set_thread_arg(void *arg)
    {
        _thread_arg = arg;
    }

    bool has_started() const
    {
        return _has_started;
    }

    bool has_finished() const;

public:
    bool start();
    void join();
    void terminate();
};

}

#endif
