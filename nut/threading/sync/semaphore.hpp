/**
 * @file -
 * @author jingqi
 * @date 2012-07-10
 * @last-edit 2012-07-10 21:23:00 jingqi
 */

#ifndef ___HEADFILE_49083D01_04DA_4385_A1BD_6D1F2902FA7A_
#define ___HEADFILE_49083D01_04DA_4385_A1BD_6D1F2902FA7A_

#include <assert.h>
#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#else
#   include <semaphore.h>
#endif

namespace nut
{

class Semaphore
{
#if defined(NUT_PLATFORM_OS_WINDOWS)
    HANDLE m_sema;
#else
    sem_t m_sem;
#endif

public:
    Semaphore(int initValue)
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        m_sema = ::CreateSemaphoreA(
            NULL, // security attributes
            initValue, // initial count
            0x7fffffff, // maximum value
            NULL); // name
        assert(NULL != m_sema);
#else
        int rs = ::sem_init(&m_sem, 0, initValue);
        assert(0 == rs);
#endif
    }

    ~Semaphore()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        ::CloseHandle(m_sema);
#else
        int rs = ::sem_destroy(&m_sem);
        assert(0 == rs);
#endif
    }

    void wait()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        DWORD rs = ::WaitForSingleObject(m_sema, INFINITE);
        assert(WAIT_OBJECT_0 == rs);
#else
        int rs = ::sem_wait(&m_sem);
        assert(0 == rs);
#endif
    }

    void post()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        BOOL rs = ::ReleaseSemaphore(m_sema, 1, NULL);
        assert(rs);
#else
        int rs = ::sem_post(&m_sem);
        assert(0 == rs);
#endif
    }

    bool trywait()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return WAIT_OBJECT_0 == ::WaitForSingleObject(m_sema, 0);
#else
        return 0 == ::sem_trywait(&m_sem);
#endif
    }

    bool timedwait(unsigned s, unsigned ms)
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        DWORD dwMilliseconds = s * 1000 + ms;
        return WAIT_OBJECT_0 == ::WaitForSingleObject(m_sema, dwMilliseconds);
#else
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += s;
        abstime.tv_nsec += ((long)ms) * 1000 * 1000;
        return 0 == ::sem_timedwait(&m_sem, &abstime);
#endif
    }
};

}

#endif

