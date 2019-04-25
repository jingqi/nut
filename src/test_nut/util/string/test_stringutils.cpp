﻿
#include <nut/unittest/unittest.h>

#include <iostream>
#include <nut/util/string/string_utils.h>
#include <nut/util/string/to_string.h>

using namespace std;
using namespace nut;

class TestStringUtil : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_split);
        NUT_REGISTER_CASE(test_safe_snprintf);
        NUT_REGISTER_CASE(test_format);
        NUT_REGISTER_CASE(test_trim);
        NUT_REGISTER_CASE(test_stricmp);
        NUT_REGISTER_CASE(test_wstr);
        NUT_REGISTER_CASE(test_utf8_ascii_convert);
        NUT_REGISTER_CASE(test_xml_encoding);
        NUT_REGISTER_CASE(test_url_encoding);
        NUT_REGISTER_CASE(test_hex_encoding);
        NUT_REGISTER_CASE(test_cstyle_encoding);
        NUT_REGISTER_CASE(test_base64_encoding);
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
        // std::string tmp = format("%d,%s,%c,%f", 1, "am", 's', 1.23);
        // NUT_TA(tmp == "1,am,s,1.230000");

        NUT_TA(format("%d,%s,%c,%f", 1, "am", 's', 1.23) == "1,am,s,1.230000");

        // std::wstring wtmp;
        // wcout << format(L"%d,%S,%c,%f", 1, L"am", L's', 1.23) << endl;
#if NUT_PLATFORM_OS_LINUX || NUT_PLATFORM_OS_MACOS
        //wtmp = format(L"%d,%S,%C,%f", 1, L"am", L's', 1.23);
        //NUT_TA(wtmp == L"1,am,s,1.230000");

        NUT_TA(format(L"%d,%S,%C,%f", 1, L"am", L's', 1.23) == L"1,am,s,1.230000");
#else
        // wtmp = format(L"%d,%ls,%c,%f", 1, L"am", L's', 1.23);
        // NUT_TA(wtmp == L"1,am,s,1.230000");

        // wchar_t* 格式化要用 %S 或者 %ls
        NUT_TA(format(L"%d,%S,%c,%f", 1, L"am", L's', 1.23) == L"1,am,s,1.230000");
#endif
    }

    void test_safe_snprintf()
    {
        char buf[10];
        wchar_t wbuf[10];
        NUT_TA(8 == safe_snprintf(buf, 10, "abcde%d", 123));
        NUT_TA(0 == ::strcmp(buf, "abcde123"));

        NUT_TA(8 == safe_snprintf(wbuf, 10, L"abcde%d", 123));
        NUT_TA(0 == ::wcscmp(wbuf, L"abcde123"));

        NUT_TA(9 == safe_snprintf(buf, 10, "abcde%d", 1234));
        NUT_TA(0 == ::strcmp(buf, "abcde1234"));

        NUT_TA(9 == safe_snprintf(wbuf, 10, L"abcde%d", 1234));
        NUT_TA(0 == ::wcscmp(wbuf, L"abcde1234"));

        NUT_TA(9 == safe_snprintf(buf, 10, "abcde%d", 12345));
        NUT_TA(0 == ::strcmp(buf, "abcde1234"));

        NUT_TA(10 > safe_snprintf(wbuf, 10, L"abcde%d", 12345));
        NUT_TA(0 != ::wcscmp(wbuf, L"abcde12345"));
    }

    void test_trim()
    {
        std::string tmp = trim(" ab\r\t");
        NUT_TA(tmp == "ab");

        tmp = ltrim(" ab\r\t");
        NUT_TA(tmp == "ab\r\t");

        tmp = rtrim(" ab\r\t");
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

#if !NUT_PLATFORM_OS_MACOS // macOS 下目前对 wchar_t 常量字符串转换有问题
        a = wstr_to_ascii(L"c5&汉");
        b = ascii_to_wstr(a.c_str());
        //wcout << endl << b << endl;
        NUT_TA(b == L"c5&汉");

        a = wstr_to_utf8(L"c5&汉");
        b = utf8_to_wstr(a.c_str());
        //wcout << endl << b << endl;
        NUT_TA(b == L"c5&汉");
#endif

        // NOTE Windows 下
        // - VC 会在 exe 文件中存储的是 ascii(cp936) 字符串
        // - minGW 默认存储的是 utf8, 除非使用 "-fexec-charset=CP936" 编译参数
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_MINGW
        NUT_TA(wstr_to_utf8(utf8_to_wstr("c5&汉")) == "c5&汉" || // 如果没有指定 "-fexec-charset=CP936"
               wstr_to_ascii(ascii_to_wstr("c5&汉")) == "c5&汉"); // 如果指定了 "-fexec-charset=CP936"
#else
        // vc 会将c字符串转编码为 ascii，所以运行时全部为 ascii
        b = ascii_to_wstr("c5&汉");
        a = wstr_to_ascii(b.c_str());
        NUT_TA(a == "c5&汉");
#endif
    }

    void test_utf8_ascii_convert()
    {
        std::string a = "abcd中文", b, c;
        b = nut::ascii_to_utf8(a.c_str());
        c = nut::utf8_to_ascii(b.c_str());
        NUT_TA(c == a);
    }

    void test_xml_encoding()
    {
        std::string s = xml_encode("a\"b<c>d&ef", -1);
        NUT_TA(s == "a&quot;b&lt;c&gt;d&amp;ef");

        s = xml_decode("a&quot;b&lt;c&gt;d&amp;ef", -1);
        NUT_TA(s == "a\"b<c>d&ef");
    }

    void test_url_encoding()
    {
        std::string s = url_encode("abc#$^* m123", -1);
        NUT_TA(s == "abc%23%24%5E%2A%20m123");

        s = url_decode("abc%23%24%5E%2A%20m123", -1);
        NUT_TA(s == "abc#$^* m123");
    }

    void test_hex_encoding()
    {
        std::string s = hex_encode("\x03\xfA", 2);
        NUT_TA(s == "03FA");

        std::vector<uint8_t> v = hex_decode("03 FA\t", -1);
        NUT_TA(v.size() == 2);
        NUT_TA(v[0] == 0x03);
        NUT_TA(v[1] == 0xFA);
    }

    void test_cstyle_encoding()
    {
        std::string s = cstyle_encode("abc\adef\b\f\n\r\t\v\\\"\'\xf9", -1);
        // std::cout << std::endl << s << std::endl;
        NUT_TA(s == "abc\\adef\\b\\f\\n\\r\\t\\v\\\\\\\"\\\'\\xF9");

        s = cstyle_decode("abc\\adef\\b\\f\\n\\r\\t\\v\\\\\\\"\\\'\\xF9", -1);
        NUT_TA(s == "abc\adef\b\f\n\r\t\v\\\"\'\xf9");
    }

    void test_base64_encoding()
    {
        // encode
        std::string s = base64_encode("abcdef", 6);
        NUT_TA(s == "YWJjZGVm");

        s = base64_encode("abcdefg", 7);
        NUT_TA(s == "YWJjZGVmZw==");

        s = base64_encode("abcdefgh", 8);
        NUT_TA(s == "YWJjZGVmZ2g=");

        // decode
        std::vector<uint8_t> v = base64_decode("YW \n Jj \t ZGVm", -1);
        NUT_TA(v.size() == 6);
        NUT_TA(0 == ::strncmp((const char*)&v[0], "abcdef", 6));

        v = base64_decode("YWJjZGVmZw==", -1);
        NUT_TA(v.size() == 7);
        NUT_TA(0 == ::strncmp((const char*)&v[0], "abcdefg", 7));

        v = base64_decode("YWJjZGVmZ2g=", -1);
        NUT_TA(v.size() == 8);
        NUT_TA(0 == ::strncmp((const char*)&v[0], "abcdefgh", 8));
    }
};

NUT_REGISTER_FIXTURE(TestStringUtil, "util, string, quiet")
