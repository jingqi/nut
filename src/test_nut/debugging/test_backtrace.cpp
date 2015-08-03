
#include <nut/unittest/unit_test.h>

#include <nut/debugging/backtrace.h>
#include <iostream>

using namespace nut;
using namespace std;

#if defined(NUT_PLATFORM_OS_LINUX)

NUT_FIXTURE(TestBacktrace)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void set_up() {}
    void tear_down() {}

    void test_smoking()
    {
        string s;
        Backtrace::backtrace(&s);
        cout << endl << s << endl;
    }
};

NUT_REGISTER_FIXTURE(TestBacktrace, "debugging")

#endif
