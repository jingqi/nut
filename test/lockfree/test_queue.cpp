
#include <nut/unittest/unittest.hpp>

#include <nut/lockfree/concurrentqueue.hpp>

using namespace nut;

NUT_FIXTURE(TestConcurrentQueue)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASES_END()

    void setUp() {}
    void tearDown() {}

    void testSmoking()
    {
        ConcurrentQueue<int> q;
        q.optimistic_enqueue(1);
        q.eliminate_enqueue(2);

        int v = 0;
        bool rs = q.eliminate_dequeue(&v);
        NUT_TA(rs && v == 1);

        rs = q.optimistic_dequeue(&v);
        NUT_TA(rs && v == 2);

        NUT_TA(q.isEmpty());
    }
};

NUT_REGISTER_FIXTURE(TestConcurrentQueue, "lockfree, quiet")
