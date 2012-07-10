
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
        printf("%c", (char)p);
    }

    void testManual()
    {
        ref<ThreadPool> tp = gc_new<ThreadPool>(2);
        tp->start();

        Sleep(1000);
        printf("a");
        tp->add_task(custom, (void*)'A');
        printf("b");
        tp->add_task(custom, (void*)'B');
        printf("c");
        tp->add_task(custom, (void*)'C');

        Sleep(2000);
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
