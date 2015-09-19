
#include <nut/platform/platform.h>

#if defined(NUT_PLATFORM_OS_LINUX) || defined(NUT_PLATFORM_OS_MAC)
#   include <unistd.h>
#endif

#include <stdio.h>

#include <nut/unittest/unit_test.h>

#include <nut/threading/thread_pool.h>

using namespace nut;

NUT_FIXTURE(TestThreading)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_manual)
    NUT_CASES_END()

    static void custom(void *p)
    {
        int c = (int) (reinterpret_cast<char*>(p) - (char*)NULL);
        printf("%c", c);
    }

    void test_manual()
    {
        rc_ptr<ThreadPool> tp = rc_new<ThreadPool>(2);
        tp->start();

#if defined(NUT_PLATFORM_OS_WINDOWS)
        Sleep(1000);
#else
        sleep(1);
#endif

        printf("a");
        tp->add_task(custom, (void*)'A');
        printf("b");
        tp->add_task(custom, (void*)'B');
        printf("c");
        tp->add_task(custom, (void*)'C');

#if defined(NUT_PLATFORM_OS_WINDOWS)
        Sleep(2000);
#else
        sleep(2);
#endif
        printf("x");
        tp->add_task(custom, (void*)'X');
        printf("y");
        tp->add_task(custom, (void*)'Y');
        printf("z");
        tp->add_task(custom, (void*)'Z');

        tp->interupt();

        printf("i");
        tp->add_task(custom, (void*)'I');
    }
};

NUT_REGISTER_FIXTURE(TestThreading, "threading")
