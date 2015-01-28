

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_LINUX)
#   include <unistd.h>
#endif

#include <nut/unittest/unittest.hpp>

#include <nut/threading/lockfree/concurrent_queue.hpp>

using namespace nut;

NUT_FIXTURE(TestConcurrentQueue)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void set_up() {}
    void tear_down() {}

    void test_smoking()
    {
        ConcurrentQueue<int> q;
        q.optimistic_enqueue(1);
        q.eliminate_enqueue(2);

        int v = 0;
        bool rs = q.eliminate_dequeue(&v);
        NUT_TA(rs && v == 1);

        rs = q.optimistic_dequeue(&v);
        NUT_TA(rs && v == 2);

        NUT_TA(q.is_empty());
    }
};

NUT_REGISTER_FIXTURE(TestConcurrentQueue, "threading, lockfree, quiet")
