
#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_LINUX) || defined(NUT_PLATFORM_OS_MAC)
#   include <unistd.h>
#endif

#include <stdio.h>

#include <nut/unittest/unittest.hpp>

#include <nut/threading/thread_pool.hpp>

using namespace nut;

NUT_FIXTURE(TestThreading)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_manual)
    NUT_CASES_END()

    void set_up() {}
    void tear_down() {}

    static void custom(void *p)
    {
        int c = reinterpret_cast<char*>(p) - (char*)NULL;
        printf("%c", c);
    }

    void test_manual()
    {
        ref<ThreadPool> tp = gc_new<ThreadPool>(NULL, 2);
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
