
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
#include <nut/time/time_wheel.h>


using namespace std;
using namespace nut;

class TestTimeWheel : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoke);
        NUT_REGISTER_CASE(test_cross_wheel);
        NUT_REGISTER_CASE(test_time_func);
        NUT_REGISTER_CASE(test_bug1);
    }

    TimeWheel tw;

    long count = 0;

    virtual void set_up() final override
    {
        count = 0;
    }

    virtual void tear_down() final override
    {
        tw.clear();
    }

    void show(TimeWheel::timer_id_type id, int64_t expires)
    {
        cout << "-- " << this->count << (expires < 0 ? " " : " +") << expires << "ms" << endl << flush;

        ++this->count;
    }

    void test_smoke()
    {
        cout << endl;

        count = 0;
        tw.add_timer(
            2550, 23,  // 最小轮周期为 2560ms
            [=] (TimeWheel::timer_id_type id, int64_t expires) {
                show(id, expires);
                if (this->count >= 20)
                    this->tw.cancel_timer(id);
            });

        while (tw.size() > 0)
        {
            uint64_t idle_ms = tw.get_idle();
            // cout << "idle " << idle_ms << "ms" << endl;
            NUT_TA(idle_ms > 0);
            idle_ms = std::max<uint64_t>(1, idle_ms);
            std::this_thread::sleep_for(
                std::chrono::milliseconds(idle_ms));
            tw.tick();
        }
    }

    void test_cross_wheel()
    {
        cout << endl;

#define AT(t)                                                       \
        tw.add_timer(                                               \
            t, 0,                                                   \
            [=] (TimeWheel::timer_id_type id, int64_t expires) {    \
                show(id, expires);                                  \
            });

        uint64_t t = 1;
        AT(t); t += 2570; // 跨越一个 wheel
        AT(t); t += 2570;
        AT(t); t += 2570;
        AT(t); t += 2570;

        while (tw.size() > 0)
        {
            uint64_t idle_ms = tw.get_idle();
            // cout << "idle " << idle_ms << "ms" << endl;
            NUT_TA(idle_ms > 0);
            idle_ms = std::max<uint64_t>(1, idle_ms);
            std::this_thread::sleep_for(
                std::chrono::milliseconds(idle_ms));
            tw.tick();
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

    void test_bug1()
    {
        // NOTE
        // 由于 cancel_timer() 定位 timer 的 bug, 会导致 assert() 失败
        //
        TimeWheel::timer_id_type id1 = tw.add_timer(
            2570, 0,
            [=] (TimeWheel::timer_id_type id, int64_t expires) {
                cout << "should not run!!!!!" << endl;
                NUT_TA(false);
            });
        tw.add_timer(
            2540, 0,
            [=] (TimeWheel::timer_id_type id, int64_t expires) {
                tw.cancel_timer(id1);
            });

        while (tw.size() > 0)
        {
            tw.tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    }
};

NUT_REGISTER_FIXTURE(TestTimeWheel, "util")
