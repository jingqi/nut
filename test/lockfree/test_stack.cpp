
#include <nut/unittest/unittest.hpp>

#include <nut/lockfree/concurrentstack.hpp>

using namespace nut;

NUT_FIXTURE(TestConcurrentStack)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASES_END()

    void setUp() {}
    void tearDown() {}

    void testSmoking()
    {
        ConcurrentStack<int> s;
        s.push(1);
        s.eliminate_push(2);

        int v = 0;
        bool rs = s.eliminate_pop(&v);
        NUT_TA(rs && v == 2);

        rs = s.pop(&v);
        NUT_TA(rs && v == 1);

        NUT_TA(s.isEmpty());
    }
};

NUT_REGISTER_FIXTURE(TestConcurrentStack, "lockfree, quiet")
