
#include <iostream>

#include <nut/unittest/unit_test.h>
#include <nut/platform/sys.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestSys)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void test_smoking()
    {
        NUT_TA(Sys::get_processor_num() > 0);
    }

};

NUT_REGISTER_FIXTURE(TestSys, "platform,quiet")
