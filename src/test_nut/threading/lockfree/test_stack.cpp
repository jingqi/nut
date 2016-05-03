
#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_LINUX
#   include <unistd.h>
#endif

#include <nut/unittest/unit_test.h>

#include <nut/threading/lockfree/concurrent_stack.h>

using namespace nut;

NUT_FIXTURE(TestConcurrentStack)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void test_smoking()
    {
        ConcurrentStack<int> s;
        s.push(1);
        s.eliminate_push(2);

        int v = 0;
        bool rs = s.eliminate_pop(&v);
        NUT_TA(rs && v == 2);

        rs = s.pop(&v);
        NUT_TA(rs && v == 1);

        NUT_TA(s.is_empty());
    }
};

NUT_REGISTER_FIXTURE(TestConcurrentStack, "threading, lockfree, quiet")
