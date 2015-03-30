
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
        NUT_TA(c_to_str('m') == "109");

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
        // cout << s << endl;
        NUT_TA(s == "0x0000122E" || s == "0x122e" || s == "0x0000122e");
    }

    void test_split()
    {
        vector<string> rs;
        chr_split("a,b,e", ',', &rs);
        NUT_TA(rs.size() == 3 && rs[0] == "a" && rs[1] == "b" && rs[2] == "e");

        rs.clear();
        chr_split("a,b&e", ",&", &rs);
        NUT_TA(rs.size() == 3 && rs[0] == "a" && rs[1] == "b" && rs[2] == "e");

        rs.clear();
        str_split("a,&b,&", ",&", &rs);
        NUT_TA(rs.size() == 3 && rs[0] == "a" && rs[1] == "b" && rs[2] == "");

        rs.clear();
        str_split("a,&b,&", ",&", &rs, true);
        NUT_TA(rs.size() == 2 && rs[0] == "a" && rs[1] == "b");
    }

    void test_format()
    {
        NUT_TA(format("%d,%s,%c,%f", 1, "am", 's', 1.23) == "1,am,s,1.230000");

        // wcout << format(L"%d,%S,%c,%f", 1, L"am", L's', 1.23) << endl;
#if defined(NUT_PLATFORM_CC_MINGW) || defined(NUT_PLATFORM_OS_LINUX) || defined(NUT_PLATFORM_OS_MAC)
        NUT_TA(format(L"%d,%S,%C,%f", 1, L"am", L's', 1.23) == L"1,am,s,1.230000");
#else
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

#if !defined(NUT_PLATFORM_OS_MAC) // mac 下目前对 wchar_t 常量字符串转换有问题
        wstr_to_ascii(L"c5&汉", &a);
        ascii_to_wstr(a.c_str(), &b);
        //wcout << endl << b << endl;
        NUT_TA(b == L"c5&汉");

		a.clear();
		b.clear();
        wstr_to_utf8(L"c5&汉", &a);
        utf8_to_wstr(a.c_str(), &b);
        //wcout << endl << b << endl;
        NUT_TA(b == L"c5&汉");
#endif

#if defined(NUT_PLATFORM_CC_GCC) || defined(NUT_PLATFORM_CC_MINGW)
        // gcc 或直接取源码的编码并遗留到运行时编码中，目前源码的编码为 utf8
		a.clear();
		b.clear();
        utf8_to_wstr("c5&汉", &b);
        wstr_to_utf8(b.c_str(), &a);
        NUT_TA(a == "c5&汉");
#elif defined(NUT_PLATFORM_CC_VC)
        // vc 会将c字符串转编码为 ascii，所以运行时全部为 ascii
		a.clear();
		b.clear();
        ascii_to_wstr("c5&汉", &b);
        wstr_to_ascii(b.c_str(), &a);
        NUT_TA(a == "c5&汉");
#endif
    }
};

NUT_REGISTER_FIXTURE(TestStringUtil, "util, string, quiet")
