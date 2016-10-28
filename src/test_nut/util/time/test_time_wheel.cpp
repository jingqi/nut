
#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_LINUX || NUT_PLATFORM_OS_MAC
#   include <unistd.h>
#else
#   include <windows.h>
#endif

#include <stdio.h>
#include <iostream>

#include <nut/unittest/unit_test.h>

#include <nut/util/time/time_wheel.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestTimeWheel)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoke)
    NUT_CASE(test_time_func)
    NUT_CASES_END()

    TimeWheel tw;
    TimeWheel::timer_id_t id;

    long count = 0;

    static void show(TimeWheel::timer_id_t id, void *arg, uint64_t expires)
    {
        TestTimeWheel *pthis = (TestTimeWheel*)arg;

        cout << "-- " << pthis->count << " +" << expires << "ms" << endl << flush;

        if (pthis->count >= 20)
            pthis->tw.cancel_timer(id);
        ++pthis->count;
    }

    void test_smoke()
    {
        cout << endl;

        count = 0;
        id = tw.add_timer(2550, 23, show, this); // 最小轮周期为 2560ms

        while (tw.size() > 0)
        {
            tw.tick();
            // cout << "." << flush;
#if NUT_PLATFORM_OS_WINDOWS
            ::Sleep(TimeWheel::TICK_GRANULARITY_MS);
#else
            ::usleep(TimeWheel::TICK_GRANULARITY_MS * 1000);
#endif
        }
    }

    void test_time_func()
    {
        const size_t MAX_COUNT = 100000;
        cout << endl;

#if NUT_PLATFORM_OS_WINDOWS
        LARGE_INTEGER freq, start, finish;
        ::QueryPerformanceFrequency(&freq);

#   define BEGIN \
        ::QueryPerformanceCounter(&start); \
        for (size_t i = 0; i < MAX_COUNT; ++i)

#   define END(name) \
        ::QueryPerformanceCounter(&finish); \
        cout << name << " " << (finish.QuadPart - start.QuadPart) * 1000000 / freq.QuadPart << "us" << endl;

        BEGIN
            ::clock();
        END("::clock()")

        BEGIN
            ::time(NULL);
        END("::time()")

        BEGIN
            ::GetTickCount();
        END("::GetTickCount()")

        BEGIN
            ::QueryPerformanceCounter(&finish);
        END("::QueryPerformanceCounter()")
#else
        struct timespec start, finish;

#   define BEGIN \
        ::clock_gettime(CLOCK_MONOTONIC_RAW, &start); \
        for (size_t i = 0; i < MAX_COUNT; ++i)

#   define END(name) \
        ::clock_gettime(CLOCK_MONOTONIC_RAW, &finish); \
        cout << name << " " << (finish.tv_sec - start.tv_sec) * 1000000 + (finish.tv_nsec - start.tv_nsec) / 1000 << "us" << endl;

        BEGIN
            ::clock();
        END("::clock()")

        BEGIN
            ::clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
        END("::clock_gettime(CLOCK_MONOTONIC_RAW)")

        BEGIN
            ::clock_gettime(CLOCK_MONOTONIC_RAW_APPROX, &finish);
        END("::clock_gettime(CLOCK_MONOTONIC_RAW_APPROX)")

        struct timeval tv;
        struct timvezone tz;
        BEGIN
            ::gettimeofday(&tv, &tz);
        END("::gettimeofday()")
#endif
    }
};

NUT_REGISTER_FIXTURE(TestTimeWheel, "util")
