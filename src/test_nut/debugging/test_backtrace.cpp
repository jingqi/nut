
#include <nut/unittest/unittest.h>

#include <nut/debugging/backtrace.h>
#include <iostream>

using namespace nut;
using namespace std;

#if NUT_PLATFORM_OS_LINUX

class TestBacktrace : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        string s;
        Backtrace::backtrace(&s);
        cout << endl << s << endl;
    }
};

NUT_REGISTER_FIXTURE(TestBacktrace, "debugging")

#endif
