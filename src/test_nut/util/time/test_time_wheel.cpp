
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
    NUT_CASE(test_smoking)
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

    void test_smoking()
    {
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
};

NUT_REGISTER_FIXTURE(TestTimeWheel, "util")
