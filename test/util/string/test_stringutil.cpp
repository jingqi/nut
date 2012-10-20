
#include <nut/unittest/unittest.hpp>

#include <iostream>
#include <nut/util/string/stringutil.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestStringUtil)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testToString)
    NUT_CASE(testSplit)
    NUT_CASE(testFormat)
    NUT_CASE(testTrim)
    NUT_CASE(testStrieq)
    NUT_CASE(testwstr)
    NUT_CASES_END()


    void setUp() {}
    void tearDown() {}

    void testToString()
    {
        NUT_TA(toString('m') == "m");

        NUT_TA(toString((long)12) == "12");
        NUT_TA(toString((unsigned char)13) == "13");
        NUT_TA(toString((short)14) == "14");
        NUT_TA(toString((unsigned short)15) == "15");
        NUT_TA(toString((int)-16) == "-16");
        NUT_TA(toString((unsigned int)17) == "17");
        NUT_TA(toString((unsigned long)18) == "18");

        NUT_TA(toString(false) == "false");
        NUT_TA(toString(true) == "true");

        NUT_TA(toString((double)12.34) == "12.340000");
        NUT_TA(toString((float)-45.67) == "-45.669998");

        NUT_TA(toString((void*)0x122e) == "0x0000122E");
    }

    void testSplit()
    {
        vector<string> rs = chr_split("a,b,e", ',');
        NUT_TA(rs.size() == 3 && rs[0] == "a" && rs[1] == "b" && rs[2] == "e");

        rs = chr_split("a,b&e", ",&");
        NUT_TA(rs.size() == 3 && rs[0] == "a" && rs[1] == "b" && rs[2] == "e");

        rs = str_split("a,&b,&", ",&");
        NUT_TA(rs.size() == 3 && rs[0] == "a" && rs[1] == "b" && rs[2] == "");

        rs = str_split("a,&b,&", ",&", true);
        NUT_TA(rs.size() == 2 && rs[0] == "a" && rs[1] == "b");
    }

    void testFormat()
    {
        NUT_TA(format("%d,%s,%c,%f", 1, "am", 's', 1.23) == "1,am,s,1.230000");
    }

    void testTrim()
    {
        NUT_TA(trim(" ab\r\t") == "ab");
        NUT_TA(ltrim(" ab\r\t") == "ab\r\t");
        NUT_TA(rtrim(" ab\r\t") == " ab");
    }

    void testStrieq()
    {
        NUT_TA(!strieq("a", "ab"));
        NUT_TA(!strieq("ac", "ab"));
        NUT_TA(strieq("ab", "ab"));
        NUT_TA(strieq("aB", "ab"));
    }

    void testwstr()
    {
        NUT_TA(wstr2str(L"c5&汉") == "c5&汉");
        NUT_TA(str2wstr("c5&汉") == L"c5&汉");
    }
};

NUT_REGISTER_FIXTURE(TestStringUtil, "util, quiet")
