
#include <iostream>

#include <nut/unittest/unit_test.h>

#include <nut/util/time/date_time.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestDateTime)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASE(test_bug1)
    NUT_CASES_END()

    void test_smoking()
    {
        DateTime t(2015, 9, 20, 11, 12, 13, 123456);
        // cout << t.to_string() << endl;
        NUT_TA(t.to_string() == "2015-09-20 11:12:13.123456");
    }

    void test_bug1()
    {
        DateTime t(2018, 10, 16, 16, 01, 55, 654321); // dirty = false
        t -= TimeDiff(8 * 60 * 60); // dirty = true
        NUT_TA(t.to_string() == "2018-10-16 08:01:55.654321"); // update struct tm
    }
};

NUT_REGISTER_FIXTURE(TestDateTime, "util, quiet")
