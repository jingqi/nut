
#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_LINUX)
#   include <unistd.h>
#endif

#include <stdio.h>

#include <nut/unittest/unittest.hpp>

#include <nut/threading/threadpool.hpp>

using namespace nut;

NUT_FIXTURE(TestThreading)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testManual)
    NUT_CASES_END()

    void setUp() {}
    void tearDown() {}

    static void custom(void *p)
    {
        int c = reinterpret_cast<char*>(p) - (char*)NULL;
        printf("%c", c);
    }

    void testManual()
    {
        ref<ThreadPool> tp = gc_new<ThreadPool>(2);
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
