﻿
#include <assert.h>
#include <time.h>

#include <nut/platform/platform.h>

#include "sem.h"

namespace nut
{

Semaphore::Semaphore(int init_value)
{
#if defined(NUT_PLATFORM_OS_WINDOWS)
    m_sema = ::CreateSemaphoreA(
        NULL, // security attributes
        init_value, // initial count
        0x7fffffff, // maximum value
        NULL); // name
    assert(NULL != m_sema);
#else
    const int rs = ::sem_init(&m_sem, 0, init_value);
    assert(0 == rs);
#endif
}

Semaphore::~Semaphore()
{
#if defined(NUT_PLATFORM_OS_WINDOWS)
    const BOOL rs = ::CloseHandle(m_sema);
    assert(FALSE != rs);
#else
    int rs = ::sem_destroy(&m_sem);
    assert(0 == rs);
#endif
}

void Semaphore::wait()
{
#if defined(NUT_PLATFORM_OS_WINDOWS)
    const DWORD rs = ::WaitForSingleObject(m_sema, INFINITE);
    assert(WAIT_OBJECT_0 == rs);
#else
    const int rs = ::sem_wait(&m_sem);
    assert(0 == rs);
#endif
}

void Semaphore::post()
{
#if defined(NUT_PLATFORM_OS_WINDOWS)
    const BOOL rs = ::ReleaseSemaphore(m_sema, 1, NULL);
    assert(FALSE != rs);
#else
    const int rs = ::sem_post(&m_sem);
    assert(0 == rs);
#endif
}

bool Semaphore::trywait()
{
#if defined(NUT_PLATFORM_OS_WINDOWS)
    return WAIT_OBJECT_0 == ::WaitForSingleObject(m_sema, 0);
#else
    return 0 == ::sem_trywait(&m_sem);
#endif
}

bool Semaphore::timedwait(unsigned s, unsigned ms)
{
#if defined(NUT_PLATFORM_OS_WINDOWS)
    const DWORD dw_milliseconds = s * 1000 + ms;
    return WAIT_OBJECT_0 == ::WaitForSingleObject(m_sema, dw_milliseconds);
#elif defined(NUT_PLATFORM_OS_MAC)
#   warning FIXME MAC 不支持sem_timedwait()
    return 0 == ::sem_trywait(&m_sem);
#else
    struct timespec abstime;
    ::clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_sec += s;
    abstime.tv_nsec += ((long)ms) * 1000 * 1000;
    return 0 == ::sem_timedwait(&m_sem, &abstime);
#endif
}

}