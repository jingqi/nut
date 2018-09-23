
#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_LINUX || NUT_PLATFORM_OS_MAC
#   include <unistd.h>
#endif

#include <stdio.h>

#include <nut/unittest/unit_test.h>

#include <nut/threading/thread_pool.h>

using namespace nut;

NUT_FIXTURE(TestThreadPool)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoke)
    NUT_CASE(test_auto_release)
    NUT_CASE(test_bug1)
    NUT_CASES_END()

    void test_smoke()
    {
        rc_ptr<ThreadPool> tp = rc_new<ThreadPool>();

        printf("a");
        tp->add_task([] { printf("A"); });
        printf("b");
        tp->add_task([] { printf("B"); });
        printf("c");
        tp->add_task([] { printf("C"); });

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        printf("|");

        printf("x");
        tp->add_task([] { printf("X"); });
        printf("y");
        tp->add_task([] { printf("Y"); });
        printf("z");
        tp->add_task([] { printf("Z"); });

        printf("i");
        tp->add_task([] { printf("I"); });
    }

    void test_auto_release()
    {
        rc_ptr<ThreadPool> tp = rc_new<ThreadPool>(10, 1);

        printf("a");
        tp->add_task([] { printf("A"); });
        printf("b");
        tp->add_task([] { printf("B"); });
        printf("c");
        tp->add_task([] { printf("C"); });

        std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // 空闲线程会超时自动回收

        printf("x");
        tp->add_task([] { printf("X"); });
        printf("y");
        tp->add_task([] { printf("Y"); });
        printf("z");
        tp->add_task([] { printf("Z"); });

        printf("i");
        tp->add_task([] { printf("I"); });
    }

    void test_bug1()
    {
        // bug 描述:
        //     一旦线程池的 join() 方法被调用，线程池就不会再创建新的工作线程了
        //
        rc_ptr<ThreadPool> tp = rc_new<ThreadPool>(10, 1);
        bool should_be_set_by_new_thread = false, has_bug = false;
        tp->add_task([&] {
            // 确保主线程已经进入 join()
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // 创建新任务，看能否让线程池创建新的工作线程
            tp->add_task([&] {
                should_be_set_by_new_thread = true;
            });
            // 等待新线程完成任务
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            has_bug = !should_be_set_by_new_thread;
            // 终止线程池
            tp->interrupt();
        });
        tp->join();
        NUT_TA(!has_bug);
    }
};

NUT_REGISTER_FIXTURE(TestThreadPool, "threading")
