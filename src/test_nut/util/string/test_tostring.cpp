
#include <nut/unittest/unit_test.h>

#include <iostream>
#include <nut/util/string/string_util.h>
#include <nut/util/string/to_string.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestToString)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoke)
    NUT_CASE(test_read_str)
    NUT_CASES_END()

    void test_smoke()
    {
        NUT_TA(char_to_str('m') == "109");

        NUT_TA(long_to_str((long)12) == "12");
        NUT_TA(uchar_to_str((unsigned char)13) == "13");
        NUT_TA(short_to_str((short)14) == "14");
        NUT_TA(ushort_to_str((unsigned short)15) == "15");
        NUT_TA(int_to_str((int)-16) == "-16");
        NUT_TA(uint_to_str((unsigned int)17) == "17");
        NUT_TA(ulong_to_str((unsigned long)18) == "18");

        NUT_TA(bool_to_str(false) == "false");
        NUT_TA(bool_to_str(true) == "true");

        NUT_TA(double_to_str((double)12.34) == "12.340000");
        NUT_TA(float_to_str((float)-45.67) == "-45.669998");

        string s = ptr_to_str((void*)0x122e);
        // cout << s << endl;
        NUT_TA(s == "0x0000122E" || s == "0x122e" || s == "0x0000122e");
    }

    void test_read_str()
    {
        NUT_TA(str_to_long("-145264") == -145264L);

        NUT_TA(str_to_double("-45.68") == -45.68);
    }
};

NUT_REGISTER_FIXTURE(TestToString, "util, string, quiet")