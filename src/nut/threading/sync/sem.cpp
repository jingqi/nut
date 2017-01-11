
#include <assert.h>
#include <time.h>

#include <nut/platform/platform.h>

#include "sem.h"

namespace nut
{

Semaphore::Semaphore(int init_value)
{
#if NUT_PLATFORM_OS_WINDOWS
    _sem = ::CreateSemaphoreA(
        nullptr, // security attributes
        init_value, // initial count
        0x7fffffff, // maximum value
        nullptr); // name
    assert(nullptr != _sem);
#else
    const int rs = ::sem_init(&_sem, 0, init_value);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

Semaphore::~Semaphore()
{
#if NUT_PLATFORM_OS_WINDOWS
    const BOOL rs = ::CloseHandle(_sem);
    assert(FALSE != rs);
    UNUSED(rs);
#else
    int rs = ::sem_destroy(&_sem);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

void Semaphore::wait()
{
#if NUT_PLATFORM_OS_WINDOWS
    const DWORD rs = ::WaitForSingleObject(_sem, INFINITE);
    assert(WAIT_OBJECT_0 == rs);
    UNUSED(rs);
#else
    const int rs = ::sem_wait(&_sem);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

void Semaphore::post()
{
#if NUT_PLATFORM_OS_WINDOWS
    const BOOL rs = ::ReleaseSemaphore(_sem, 1, nullptr);
    assert(FALSE != rs);
    UNUSED(rs);
#else
    const int rs = ::sem_post(&_sem);
    assert(0 == rs);
    UNUSED(rs);
#endif
}

bool Semaphore::trywait()
{
#if NUT_PLATFORM_OS_WINDOWS
    return WAIT_OBJECT_0 == ::WaitForSingleObject(_sem, 0);
#else
    return 0 == ::sem_trywait(&_sem);
#endif
}

bool Semaphore::timedwait(unsigned s, unsigned ms)
{
#if NUT_PLATFORM_OS_WINDOWS
    const DWORD dw_milliseconds = s * 1000 + ms;
    return WAIT_OBJECT_0 == ::WaitForSingleObject(_sem, dw_milliseconds);
#elif NUT_PLATFORM_OS_MAC
    UNUSED(s);
    UNUSED(ms);
#   warning FIXME MAC 不支持sem_timedwait()
    return 0 == ::sem_trywait(&_sem);
#else
    struct timespec abstime;
    ::clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_sec += s;
    abstime.tv_nsec += ((long)ms) * 1000 * 1000;
    return 0 == ::sem_timedwait(&_sem, &abstime);
#endif
}

}
