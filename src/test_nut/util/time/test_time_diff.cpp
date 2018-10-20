
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/util/time/time_diff.h>

using namespace std;
using namespace nut;

class TestTimeDiff : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_math);
    }

    void test_math()
    {
        TimeDiff t(1, 987654321);
        NUT_TA(t.to_double() == 1.987654321);
        NUT_TA((t * 10000).to_double() == 19876.54321);
        NUT_TA((t / 0.001).to_double() == 1987.654321);
    }
};

NUT_REGISTER_FIXTURE(TestTimeDiff, "util, quiet")
