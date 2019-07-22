
#include <nut/unittest/unittest.h>

#include <nut/debugging/backtrace.h>
#include <iostream>


using namespace nut;
using namespace std;

class TestBacktrace : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        Backtrace::print_stack();
    }
};

NUT_REGISTER_FIXTURE(TestBacktrace, "debugging")
