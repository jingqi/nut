
#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_LINUX || NUT_PLATFORM_OS_MAC
#   include <unistd.h>
#endif

#include <stdio.h>

#include <nut/unittest/unit_test.h>

#include <nut/threading/thread_pool.h>

using namespace nut;

NUT_FIXTURE(TestThreadpool)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_manual)
    NUT_CASE(test_auto_release)
    NUT_CASES_END()

    static void custom(char c)
    {
        printf("%c", c);
    }

    void test_manual()
    {
        rc_ptr<ThreadPool> tp = rc_new<ThreadPool>();

        printf("a");
        tp->add_task([] { custom('A'); });
        printf("b");
        tp->add_task([] { custom('B'); });
        printf("c");
        tp->add_task([] { custom('C'); });

        Thread::sleep(1000);

        printf("x");
        tp->add_task([] { custom('X'); });
        printf("y");
        tp->add_task([] { custom('Y'); });
        printf("z");
        tp->add_task([] { custom('Z'); });

        printf("i");
        tp->add_task([] { custom('I'); });
    }

    void test_auto_release()
    {
        rc_ptr<ThreadPool> tp = rc_new<ThreadPool>(10, 1);

        printf("a");
        tp->add_task([] { custom('A'); });
        printf("b");
        tp->add_task([] { custom('B'); });
        printf("c");
        tp->add_task([] { custom('C'); });

        Thread::sleep(5000); // 空闲线程会超时自动回收

        printf("x");
        tp->add_task([] { custom('X'); });
        printf("y");
        tp->add_task([] { custom('Y'); });
        printf("z");
        tp->add_task([] { custom('Z'); });

        printf("i");
        tp->add_task([] { custom('I'); });
    }
};

NUT_REGISTER_FIXTURE(TestThreadpool, "threading")
