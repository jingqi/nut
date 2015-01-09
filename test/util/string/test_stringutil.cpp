
#include <nut/unittest/unittest.hpp>

#include <iostream>
#include <nut/util/string/string_util.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestStringUtil)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_to_string)
    NUT_CASE(test_split)
    NUT_CASE(test_format)
    NUT_CASE(test_trim)
    NUT_CASE(test_strieq)
    NUT_CASE(test_wstr)
    NUT_CASES_END()


    void set_up() {}
    void tear_down() {}

    void test_to_string()
    {
        NUT_TA(c_to_str('m') == "m");

        NUT_TA(l_to_str((long)12) == "12");
        NUT_TA(uc_to_str((unsigned char)13) == "13");
        NUT_TA(s_to_str((short)14) == "14");
        NUT_TA(us_to_str((unsigned short)15) == "15");
        NUT_TA(i_to_str((int)-16) == "-16");
        NUT_TA(ui_to_str((unsigned int)17) == "17");
        NUT_TA(ul_to_str((unsigned long)18) == "18");

        NUT_TA(b_to_str(false) == "false");
        NUT_TA(b_to_str(true) == "true");

        NUT_TA(d_to_str((double)12.34) == "12.340000");
        NUT_TA(f_to_str((float)-45.67) == "-45.669998");

        string s = ptr_to_str((void*)0x122e);
        NUT_TA(s == "0x0000122E" || s == "0x122e");
    }

    void test_split()
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

    void test_format()
    {
        NUT_TA(format("%d,%s,%c,%f", 1, "am", 's', 1.23) == "1,am,s,1.230000");
#if !defined(NUT_PLATFORM_OS_MAC)
		NUT_TA(format(L"%d,%s,%c,%f", 1, L"am", L's', 1.23) == L"1,am,s,1.230000");
#endif
    }

    void test_trim()
    {
        NUT_TA(trim(" ab\r\t") == "ab");
        NUT_TA(ltrim(" ab\r\t") == "ab\r\t");
        NUT_TA(rtrim(" ab\r\t") == " ab");
    }

    void test_strieq()
    {
        NUT_TA(!strieq("a", "ab"));
        NUT_TA(!strieq("ac", "ab"));
        NUT_TA(strieq("ab", "ab"));
        NUT_TA(strieq("aB", "ab"));
    }

    void test_wstr()
    {
		std::string a;
		std::wstring b;

		wstr_to_ascii(L"c5&汉", &a); 
        //printf("\n%d %d %s\n", a.length(), strlen(a.c_str()), a.c_str());
        NUT_TA(a == "c5&汉");

		ascii_to_wstr("c5&汉", &b);
        NUT_TA(b == L"c5&汉");

		wstr_to_utf8(L"c5&汉", &a);
        utf8_to_wstr(a.c_str(), &b);
        NUT_TA(b == L"c5&汉");
    }
};

NUT_REGISTER_FIXTURE(TestStringUtil, "util, string, quiet")
