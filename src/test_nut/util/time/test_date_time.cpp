
#include <iostream>

#include <nut/unittest/unit_test.h>

#include <nut/util/time/date_time.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestDateTime)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void test_smoking()
    {
        DateTime t(2015, 9, 20, 11, 12, 13);
        // cout << t.to_string() << endl;
        NUT_TA(t.to_string() == "2015-09-20 11:12:13");
    }
};

NUT_REGISTER_FIXTURE(TestDateTime, "util, quiet")
