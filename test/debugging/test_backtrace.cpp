
#include <nut/unittest/unittest.hpp>

#include <nut/debugging/backtrace.hpp>
#include <iostream>

using namespace nut;
using namespace std;

#if defined(NUT_PLATFORM_OS_LINUX)

NUT_FIXTURE(TestBacktrace)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASES_END()

    void setUp() {}
    void tearDown() {}

    void testSmoking()
    {
        string s = Backtrace::backtrace();
        cout << endl << s << endl;
    }
};

NUT_REGISTER_FIXTURE(TestBacktrace, "debugging")

#endif

