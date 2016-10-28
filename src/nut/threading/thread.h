
#ifndef ___HEADFILE_93CDBEEC_8BDC_4AE9_A2D1_717CAC0ECD85_
#define ___HEADFILE_93CDBEEC_8BDC_4AE9_A2D1_717CAC0ECD85_

#include <nut/nut_config.h>
#include <nut/platform/platform.h>
#include <nut/rc/rc_ptr.h>

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#elif NUT_PLATFORM_OS_LINUX
#   include <pthread.h>
#   include <sys/types.h> // for tid_t
#else
#   include <pthread.h>
#endif

namespace nut
{

class NUT_API Thread
{
    NUT_REF_COUNTABLE

public:
    typedef void (*thread_process_type)(void*);

    // should be override by subclass
    virtual void run(void *arg)
    {
        UNUSED(arg);
        // default do nothing
    }

#if NUT_PLATFORM_OS_WINDOWS
    typedef DWORD tid_type;
#elif NUT_PLATFORM_OS_LINUX
    typedef tid_t tid_type;
#else
    typedef pthread_t tid_type;
#endif

private:
#if NUT_PLATFORM_OS_WINDOWS
    HANDLE _handle = NULL;
    DWORD _tid = 0;
#elif NUT_PLATFORM_OS_LINUX
    pthread_t _pthread;
    tid_t _tid = 0;
#else
    pthread_t _pthread;
#endif

    thread_process_type _thread_process = NULL;
    void *_thread_arg = NULL;
    bool _has_started = false;
    bool mutable _has_finished = false;

private:
#if NUT_PLATFORM_OS_WINDOWS
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

    void set_thread_process(thread_process_type process);
    void set_thread_arg(void *arg);
    
    bool has_started() const;
    bool has_finished() const;

    /**
     * 注意，只有线程启动后才可获取
     */
    tid_type get_tid() const;

    bool start();
    void join();
    void terminate();
    
    static tid_type current_thread_id();
};

}

#endif
