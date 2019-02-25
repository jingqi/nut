
#include <math.h>
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/time/date_time.h>


using namespace std;
using namespace nut;

class TestDateTime : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
        NUT_REGISTER_CASE(test_bug1);
    }

    void test_smoking()
    {
        DateTime t(2015, 9, 21, 11, 12, 13, 123456789);
        NUT_TA(t.get_year() == 2015);
        NUT_TA(t.get_month() == 9);
        NUT_TA(t.get_yday() == 263);
        NUT_TA(t.get_mday() == 21);
        NUT_TA(t.get_wday() == 1);
        NUT_TA(t.get_hour() == 11);
        NUT_TA(t.get_minute() == 12);
        NUT_TA(t.get_second() == 13);
        NUT_TA(t.get_nanosecond() == 123456789);

        NUT_TA(t.to_integer() == 1442805133);
        NUT_TA(fabs(t.to_double() - 1442805133.123456789) <= 1e-7);

        // cout << t.to_string() << endl;
        NUT_TA(t.to_string() == "2015-09-21 11:12:13.123456789");

        // cout << t.format_time("%S.%f") << endl;
        NUT_TA(t.format_time("%S.%f") == "13.123457");
        NUT_TA(t.format_time("%S.%1f") == "13.1");
        NUT_TA(t.format_time("%S.%8f") == "13.12345679");
        NUT_TA(t.format_time("%S.%12f") == "13.123456789000");
    }

    void test_bug1()
    {
        DateTime t(2018, 10, 16, 16, 01, 55, 987654321); // dirty = false
        t -= TimeDiff(8 * 60 * 60); // dirty = true
        NUT_TA(t.to_string() == "2018-10-16 08:01:55.987654321"); // update struct tm
    }
};

NUT_REGISTER_FIXTURE(TestDateTime, "util, quiet")
