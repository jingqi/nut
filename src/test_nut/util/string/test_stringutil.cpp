
#include <nut/unittest/unit_test.h>

#include <iostream>
#include <nut/util/string/string_util.h>
#include <nut/util/string/to_string.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestStringUtil)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_split)
    NUT_CASE(test_format)
    NUT_CASE(test_trim)
    NUT_CASE(test_stricmp)
    NUT_CASE(test_wstr)
    NUT_CASE(test_utf8_ascii_convert)
    NUT_CASE(test_xml_encoding)
    NUT_CASE(test_url_encoding)
    NUT_CASE(test_hex_encoding)
    NUT_CASE(test_cstyle_encoding)
    NUT_CASE(test_base64_encoding)
    NUT_CASES_END()

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

        NUT_TA(format("%d,%s,%c,%f", 1, "am", 's', 1.23) == "1,am,s,1.230000");

        std::wstring wtmp;
        // wcout << format(L"%d,%S,%c,%f", 1, L"am", L's', 1.23) << endl;
#if NUT_PLATFORM_OS_LINUX || NUT_PLATFORM_OS_MAC
        format(&wtmp, L"%d,%S,%C,%f", 1, L"am", L's', 1.23);
        NUT_TA(wtmp == L"1,am,s,1.230000");

        NUT_TA(format(L"%d,%S,%C,%f", 1, L"am", L's', 1.23) == L"1,am,s,1.230000");
#else
        format(&wtmp, L"%d,%s,%c,%f", 1, L"am", L's', 1.23);
        NUT_TA(wtmp == L"1,am,s,1.230000");

        NUT_TA(format(L"%d,%s,%c,%f", 1, L"am", L's', 1.23) == L"1,am,s,1.230000");
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
        NUT_TA(0 != nut::stricmp((const char*)"a", (const char*)"ab"));
        NUT_TA(0 != nut::stricmp("ac", "ab"));
        NUT_TA(0 == nut::stricmp("ab", "ab"));
        NUT_TA(0 == nut::stricmp("aB", "ab"));

        // bug
        NUT_TA(0 != nut::stricmp("`", "@")); // 0x60, 0x40
        NUT_TA(0 != nut::stricmp("a \0", "a\0\0")); // 0x20, 0x00
    }

    void test_wstr()
    {
		std::string a;
		std::wstring b;

#if !NUT_PLATFORM_OS_MAC // mac 下目前对 wchar_t 常量字符串转换有问题
        NUT_TA(wstr_to_ascii(L"c5&汉", &a));
        NUT_TA(ascii_to_wstr(a.c_str(), &b));
        //wcout << endl << b << endl;
        NUT_TA(b == L"c5&汉");

		a.clear();
		b.clear();
        NUT_TA(wstr_to_utf8(L"c5&汉", &a));
        NUT_TA(utf8_to_wstr(a.c_str(), &b));
        //wcout << endl << b << endl;
        NUT_TA(b == L"c5&汉");
#endif

#if NUT_PLATFORM_CC_GCC || NUT_PLATFORM_CC_MINGW
        // gcc 或直接取源码的编码并遗留到运行时编码中，目前源码的编码为 utf8
		a.clear();
		b.clear();
        NUT_TA(utf8_to_wstr("c5&汉", &b));
        NUT_TA(wstr_to_utf8(b.c_str(), &a));
        NUT_TA(a == "c5&汉");
#elif NUT_PLATFORM_CC_VC
        // vc 会将c字符串转编码为 ascii，所以运行时全部为 ascii
		a.clear();
		b.clear();
        NUT_TA(ascii_to_wstr("c5&汉", &b));
        NUT_TA(wstr_to_ascii(b.c_str(), &a));
        NUT_TA(a == "c5&汉");
#endif
    }

    void test_utf8_ascii_convert()
    {
        std::string a = "abcd中文", b, c;
        NUT_TA(nut::ascii_to_utf8(a.c_str(), &b));
        NUT_TA(nut::utf8_to_ascii(b.c_str(), &c));
        NUT_TA(c == a);
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
