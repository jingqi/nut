
#ifndef ___HEADFILE_93CDBEEC_8BDC_4AE9_A2D1_717CAC0ECD85_
#define ___HEADFILE_93CDBEEC_8BDC_4AE9_A2D1_717CAC0ECD85_

#include <functional>

#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#elif NUT_PLATFORM_OS_LINUX
#   include <pthread.h>
#   include <sys/types.h> // for tid_t
#else
#   include <pthread.h>
#endif

#include <nut/rc/rc_ptr.h>

#include "../nut_config.h"


namespace nut
{

class NUT_API Thread
{
    NUT_REF_COUNTABLE

public:
    typedef std::function<void()> task_type;

    // Can be override by subclass
    virtual void run()
    {
        // Default do nothing
    }

#if NUT_PLATFORM_OS_WINDOWS
    typedef DWORD tid_type;
#elif NUT_PLATFORM_OS_LINUX
    typedef pid_t tid_type;
#else
    typedef pthread_t tid_type;
#endif

private:
#if NUT_PLATFORM_OS_WINDOWS
    HANDLE _handle = nullptr;
    DWORD _tid = 0;
#elif NUT_PLATFORM_OS_LINUX
    pthread_t _pthread;
    pid_t _tid = 0;
#else
    pthread_t _pthread;
#endif

    task_type _task;
    bool _has_started = false;
    bool mutable _has_finished = false;

private:
#if NUT_PLATFORM_OS_WINDOWS
    static DWORD WINAPI thread_entry(LPVOID p);
#else
    static void* thread_entry(void *p);
#endif

private:
    // Non-copyable
    Thread(const Thread& x) = delete;
    Thread& operator=(const Thread& x) = delete;

public:
    explicit Thread(const task_type& task = task_type());
    virtual ~Thread();

    void set_thread_task(const task_type& task);

    bool has_started() const;
    bool has_finished() const;

    /**
     * NOTE 只有线程启动后才可获取
     */
    tid_type get_tid() const;

    bool start();
    void join();
    void terminate();

    /**
     * 获取当前线程的 tid
     */
    static tid_type current_thread_id();

    /**
     * 比较 tid 是否相等
     *
     * NOTE 由于 tid 可复用(包括 pthread_t)，请尽量保证 tid 指向当前有效的线程
     */
    static bool tid_equals(const tid_type& t1, const tid_type& t2);
};

}

#endif
