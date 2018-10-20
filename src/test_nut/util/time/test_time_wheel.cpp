
#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#else
#   include <unistd.h>
#   include <sys/time.h> // for ::gettimeofday()
#endif

#include <stdio.h>
#include <iostream>
#include <thread>

#include <nut/unittest/unittest.h>
#include <nut/util/time/time_wheel.h>

using namespace std;
using namespace nut;

class TestTimeWheel : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoke);
        NUT_REGISTER_CASE(test_time_func);
    }

    TimeWheel tw;
    TimeWheel::timer_id_type id;

    long count = 0;

    void show(TimeWheel::timer_id_type id, uint64_t expires)
    {
        cout << "-- " << this->count << " +" << expires << "ms" << endl << flush;

        if (this->count >= 20)
            this->tw.cancel_timer(id);
        ++this->count;
    }

    void test_smoke()
    {
        cout << endl;

        count = 0;
        id = tw.add_timer(2550, 23, [=](TimeWheel::timer_id_type id, uint64_t expires) { show(id, expires); }); // 最小轮周期为 2560ms

        while (tw.size() > 0)
        {
            tw.tick();
            // cout << "." << flush;
            std::this_thread::sleep_for(
                std::chrono::milliseconds(TimeWheel::TICK_GRANULARITY_MS));
        }
    }

    void test_time_func()
    {
        const size_t MAX_COUNT = 100000;
        cout << endl;

#if NUT_PLATFORM_OS_WINDOWS
        LARGE_INTEGER freq, start, finish;
        ::QueryPerformanceFrequency(&freq);

#   define BEGIN                                \
        ::QueryPerformanceCounter(&start);      \
        for (size_t i = 0; i < MAX_COUNT; ++i)

#   define END(name)                                                    \
        ::QueryPerformanceCounter(&finish);                             \
        cout << name << " " << (finish.QuadPart - start.QuadPart) * 1000000 / freq.QuadPart << "us" << endl;

        BEGIN
            ::clock();
        END("::clock()")

        BEGIN
            ::time(nullptr);
        END("::time()")

        BEGIN
            ::GetTickCount();
        END("::GetTickCount()")

        BEGIN
            ::QueryPerformanceCounter(&finish);
        END("::QueryPerformanceCounter()")
#else
        struct timespec start, finish;

#   define BEGIN                                        \
        ::clock_gettime(CLOCK_MONOTONIC_RAW, &start);   \
        for (size_t i = 0; i < MAX_COUNT; ++i)

#   define END(name)                                                    \
        ::clock_gettime(CLOCK_MONOTONIC_RAW, &finish);                  \
        cout << name << " " << (finish.tv_sec - start.tv_sec) * 1000000 + (finish.tv_nsec - start.tv_nsec) / 1000 << "us" << endl;

        BEGIN
            ::clock();
        END("::clock()")

        BEGIN
            ::clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
        END("::clock_gettime(CLOCK_MONOTONIC_RAW)")

#if NUT_PLATFORM_OS_MAC
        BEGIN
            ::clock_gettime(CLOCK_MONOTONIC_RAW_APPROX, &finish);
        END("::clock_gettime(CLOCK_MONOTONIC_RAW_APPROX)")
#endif

        struct timeval tv;
        BEGIN
            ::gettimeofday(&tv, nullptr);
        END("::gettimeofday()")
#endif
    }
};

NUT_REGISTER_FIXTURE(TestTimeWheel, "util")
