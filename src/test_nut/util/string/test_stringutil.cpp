
#include <nut/unittest/unit_test.h>

#include <iostream>
#include <nut/util/string/string_util.h>
#include <nut/util/string/to_string.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestStringUtil)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_to_string)
    NUT_CASE(test_split)
    NUT_CASE(test_format)
    NUT_CASE(test_trim)
    NUT_CASE(test_stricmp)
    NUT_CASE(test_wstr)
    NUT_CASE(test_xml_encoding)
    NUT_CASE(test_url_encoding)
    NUT_CASE(test_hex_encoding)
    NUT_CASE(test_cstyle_encoding)
    NUT_CASE(test_base64_encoding)
    NUT_CASES_END()


    void set_up() {}
    void tear_down() {}

    void test_to_string()
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
        std::string tmp;
        format(&tmp, "%d,%s,%c,%f", 1, "am", 's', 1.23);
        NUT_TA(tmp == "1,am,s,1.230000");

        std::wstring wtmp;
        // wcout << format(L"%d,%S,%c,%f", 1, L"am", L's', 1.23) << endl;
#if defined(NUT_PLATFORM_OS_LINUX) || defined(NUT_PLATFORM_OS_MAC)
        format(&wtmp, L"%d,%S,%C,%f", 1, L"am", L's', 1.23);
        NUT_TA(wtmp == L"1,am,s,1.230000");
#else
        format(&wtmp, L"%d,%s,%c,%f", 1, L"am", L's', 1.23);
        NUT_TA(wtmp == L"1,am,s,1.230000");
#endif
    }

    void test_trim()
    {
        std::string tmp;
        trim(" ab\r\t", &tmp);
        NUT_TA(tmp == "ab");

        tmp.clear();
        ltrim(" ab\r\t", &tmp);
        NUT_TA(tmp == "ab\r\t");

        tmp.clear();
        rtrim(" ab\r\t", &tmp);
        NUT_TA(tmp == " ab");
    }

    void test_stricmp()
    {
        NUT_TA(0 != stricmp("a", "ab"));
        NUT_TA(0 != stricmp("ac", "ab"));
        NUT_TA(0 == stricmp("ab", "ab"));
        NUT_TA(0 == stricmp("aB", "ab"));
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

    void test_xml_encoding()
    {
        std::string s;
        xml_encode("a\"b<c>d&ef", -1, &s);
        NUT_TA(s == "a&quot;b&lt;c&gt;d&amp;ef");

        s.clear();
        xml_decode("a&quot;b&lt;c&gt;d&amp;ef", -1, &s);
        NUT_TA(s == "a\"b<c>d&ef");
    }

    void test_url_encoding()
    {
        std::string s;
        url_encode("abc#$^* m123", -1, &s);
        NUT_TA(s == "abc%23%24%5E%2A%20m123");

        s.clear();
        url_decode("abc%23%24%5E%2A%20m123", -1, &s);
        NUT_TA(s == "abc#$^* m123");
    }

    void test_hex_encoding()
    {
        std::string s;
        hex_encode("\x03\xfA", 2, &s);
        NUT_TA(s == "03FA");

        Array<uint8_t> v;
        hex_decode("03 FA\t", -1, &v);
        NUT_TA(v.size() == 2);
        NUT_TA(v[0] == 0x03);
        NUT_TA(v[1] == 0xFA);
    }

    void test_cstyle_encoding()
    {
        std::string s;
        cstyle_encode("abc\adef\b\f\n\r\t\v\\\"\'\xf9", -1, &s);
        // std::cout << std::endl << s << std::endl;
        NUT_TA(s == "abc\\adef\\b\\f\\n\\r\\t\\v\\\\\\\"\\\'\\xF9");

        s.clear();
        cstyle_decode("abc\\adef\\b\\f\\n\\r\\t\\v\\\\\\\"\\\'\\xF9", -1, &s);
        NUT_TA(s == "abc\adef\b\f\n\r\t\v\\\"\'\xf9");
    }

    void test_base64_encoding()
    {
        // encode
        std::string s;
        base64_encode("abcdef", 6, &s);
        NUT_TA(s == "YWJjZGVm");

        s.clear();
        base64_encode("abcdefg", 7, &s);
        NUT_TA(s == "YWJjZGVmZw==");

        s.clear();
        base64_encode("abcdefgh", 8, &s);
        NUT_TA(s == "YWJjZGVmZ2g=");

        // decode
        Array<uint8_t> v;
        base64_decode("YW \n Jj \t ZGVm", -1, &v);
        NUT_TA(v.size() == 6);
        NUT_TA(0 == ::strncmp((const char*)&v[0], "abcdef", 6));

        v.clear();
        base64_decode("YWJjZGVmZw==", -1, &v);
        NUT_TA(v.size() == 7);
        NUT_TA(0 == ::strncmp((const char*)&v[0], "abcdefg", 7));

        v.clear();
        base64_decode("YWJjZGVmZ2g=", -1, &v);
        NUT_TA(v.size() == 8);
        NUT_TA(0 == ::strncmp((const char*)&v[0], "abcdefgh", 8));
    }
};

NUT_REGISTER_FIXTURE(TestStringUtil, "util, string, quiet")
