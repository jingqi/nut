
#include <iostream>
#include <nut/unittest/unit_test.h>
#include <nut/platform/path.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestPath)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_is_root)
    NUT_CASE(test_split)
    NUT_CASE(test_splitw)
    NUT_CASE(test_split_drive)
    NUT_CASE(test_split_drivew)
    NUT_CASE(test_split_ext)
    NUT_CASE(test_split_extw)
    NUT_CASE(test_join)
    NUT_CASE(test_joinw)
    NUT_CASE(test_abs_path)
    NUT_CASE(test_abs_pathw)
    NUT_CASE(test_relative_path)
    NUT_CASE(test_relative_pathw)
    NUT_CASE(test_bug1)
    NUT_CASES_END()

    void test_is_root()
    {
        NUT_TA(Path::is_root("/"));
        NUT_TA(Path::is_root("\\"));
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(Path::is_root("c:"));
        NUT_TA(Path::is_root("c:\\"));
        NUT_TA(Path::is_root("c:/"));
#else
        NUT_TA(!Path::is_root("c:"));
        NUT_TA(!Path::is_root("c:\\"));
        NUT_TA(!Path::is_root("c:/"));
#endif

        NUT_TA(!Path::is_root("~"));
        NUT_TA(!Path::is_root("/a"));
        NUT_TA(!Path::is_root("c:\\a"));
    }

    void test_split()
    {
        string p, c;
        Path::split("ab/c/d", &p, &c);
        NUT_TA(p == "ab/c");
        NUT_TA(c == "d");

        p.clear();
        c.clear();
        Path::split("/ab.txt", &p, &c);
        NUT_TA(p == "/");
        NUT_TA(c == "ab.txt");

        p.clear();
        c.clear();
        Path::split("c:\\tmp", &p, &c);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(p == "c:\\");
        NUT_TA(c == "tmp");
#else
        NUT_TA(p == "c:"); // 并不是当成根目录，而是当成普通目录名
        NUT_TA(c == "tmp");
#endif
    }

    void test_splitw()
    {
        wstring p, c;
        Path::split(L"ab/c/d", &p, &c);
        NUT_TA(p == L"ab/c");
        NUT_TA(c == L"d");

        p.clear();
        c.clear();
        Path::split(L"/ab.txt", &p, &c);
        NUT_TA(p == L"/");
        NUT_TA(c == L"ab.txt");

        p.clear();
        c.clear();
        Path::split(L"c:\\tmp", &p, &c);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(p == L"c:\\");
        NUT_TA(c == L"tmp");
#else
        NUT_TA(p == L"c:");
        NUT_TA(c == L"tmp");
#endif
    }

    void test_split_drive()
    {
        string d, r;
        Path::split_drive("c:\\mn\\p", &d, &r);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(d == "c:");
        NUT_TA(r == "\\mn\\p");
#else
        NUT_TA(d == "");
        NUT_TA(r == "c:\\mn\\p"); // 冒号当作是普通文件名中的符号
#endif

        d.clear();
        r.clear();
        Path::split_drive("/mnt/sdcard", &d, &r);
        NUT_TA(d == "");
        NUT_TA(r == "/mnt/sdcard");
    }

    void test_split_drivew()
    {
        wstring d, r;
        Path::split_drive(L"c:\\mn\\p", &d, &r);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(d == L"c:");
        NUT_TA(r == L"\\mn\\p");
#else
        NUT_TA(d == L"");
        NUT_TA(r == L"c:\\mn\\p");
#endif

        d.clear();
        r.clear();
        Path::split_drive(L"/mnt/sdcard", &d, &r);
        NUT_TA(d == L"");
        NUT_TA(r == L"/mnt/sdcard");
    }

    void test_split_ext()
    {
        string n, e;
        Path::split_ext("a.txt", &n, &e);
        NUT_TA(n == "a");
        NUT_TA(e == ".txt");
    }

    void test_split_extw()
    {
        wstring n, e;
        Path::split_ext(L"a.txt", &n, &e);
        NUT_TA(n == L"a");
        NUT_TA(e == L".txt");
    }

    void test_join()
    {
        std::string tmp;
        Path::join("a", "b", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "a\\b");
#else
        NUT_TA(tmp == "a/b");
#endif

        tmp.clear();
        Path::join("/", "sd", &tmp);
        NUT_TA(tmp == "/sd");

        tmp.clear();
        Path::join("c:", "\\tmp", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "c:\\tmp");
#else
        NUT_TA(tmp == "\\tmp");
#endif
    }

    void test_joinw()
    {
        std::wstring tmp;
        Path::join(L"a", L"b", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"a\\b");
#else
        NUT_TA(tmp == L"a/b");
#endif

        tmp.clear();
        Path::join(L"/", L"sd", &tmp);
        NUT_TA(tmp == L"/sd");

        tmp.clear();
        Path::join(L"c:", L"\\tmp", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"c:\\tmp");
#else
        NUT_TA(tmp == L"\\tmp");
#endif
    }

    void test_abs_path()
    {
        // cout << Path::abspath("/a//c");

        std::string tmp;
        Path::abs_path("e:\\", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "e:\\");
#else
        NUT_TA(tmp == Path::get_cwd() + "/e:");
#endif

        tmp.clear();
        Path::abs_path("e:", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "e:\\");
#else
        NUT_TA(tmp == Path::get_cwd() + "/e:");
#endif

        tmp.clear();
        Path::abs_path("e:\\..\\..", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "e:\\");
#else
        NUT_TA(tmp == Path::abs_path(".."));
#endif

        tmp.clear();
        Path::abs_path("e:\\..\\..\\a", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "e:\\a");
#else
        NUT_TA(tmp == Path::abs_path("../a"));
#endif

        tmp.clear();
        Path::abs_path("e:\\b\\..\\.\\a", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "e:\\a");
#else
        NUT_TA(tmp == Path::get_cwd() + "/e:/a");
#endif

        tmp.clear();
        Path::abs_path("e:\\b\\\\a", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "e:\\b\\a");
#else
        NUT_TA(tmp == Path::get_cwd() + "/e:/b/a");
#endif

        tmp.clear();
        Path::abs_path("/", &tmp);
        NUT_TA(tmp == "/");

        tmp.clear();
        Path::abs_path("/.", &tmp);
        NUT_TA(tmp == "/");

        tmp.clear();
        Path::abs_path("/../", &tmp);
        NUT_TA(tmp == "/");

        tmp.clear();
        Path::abs_path("/../../a", &tmp);
        NUT_TA(tmp == "/a");

        tmp.clear();
        Path::abs_path("/a/.././c", &tmp);
        NUT_TA(tmp == "/c");

        tmp.clear();
        Path::abs_path("/a//c", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "/a\\c");
#else
        NUT_TA(tmp == "/a/c");
#endif
    }

    void test_abs_pathw()
    {
        // cout << Path::abspath("/a//c");

        std::wstring tmp;
        Path::abs_path(L"e:\\", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"e:\\");
#else
        NUT_TA(tmp == Path::get_wcwd() + L"/e:");
#endif

        tmp.clear();
        Path::abs_path(L"e:", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"e:\\");
#else
        NUT_TA(tmp == Path::get_wcwd() + L"/e:");
#endif

        tmp.clear();
        Path::abs_path(L"e:\\..\\..", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"e:\\");
#else
        NUT_TA(tmp == Path::abs_path(L".."));
#endif

        tmp.clear();
        Path::abs_path(L"e:\\..\\..\\a", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"e:\\a");
#else
        NUT_TA(tmp == Path::abs_path(L"../a"));
#endif

        tmp.clear();
        Path::abs_path(L"e:\\b\\..\\.\\a", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"e:\\a");
#else
        NUT_TA(tmp == Path::get_wcwd() + L"/e:/a");
#endif

        tmp.clear();
        Path::abs_path(L"e:\\b\\\\a", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"e:\\b\\a");
#else
        NUT_TA(tmp == Path::get_wcwd() + L"/e:/b/a");
#endif

        tmp.clear();
        Path::abs_path(L"/", &tmp);
        NUT_TA(tmp == L"/");

        tmp.clear();
        Path::abs_path(L"/.", &tmp);
        NUT_TA(tmp == L"/");

        tmp.clear();
        Path::abs_path(L"/../", &tmp);
        NUT_TA(tmp == L"/");

        tmp.clear();
        Path::abs_path(L"/../../a", &tmp);
        NUT_TA(tmp == L"/a");

        tmp.clear();
        Path::abs_path(L"/a/.././c", &tmp);
        NUT_TA(tmp == L"/c");

        tmp.clear();
        Path::abs_path(L"/a//c", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"/a\\c");
#else
        NUT_TA(tmp == L"/a/c");
#endif
    }

    void test_relative_path()
    {
        std::string tmp;

#define _H(a, b, c)                             \
        tmp.clear();                            \
        Path::relative_path((a), (b), &tmp);    \
        NUT_TA(tmp == (c));

#if NUT_PLATFORM_OS_WINDOWS
        _H("/a/m/n", "/a/b/c", "..\\..\\m\\n");
        _H("/a/m/n/", "/a/b/c", "..\\..\\m\\n");
        _H("/a/m/n", "/a/b/c/", "..\\..\\m\\n");
        _H("/a/m/n/", "/a/b/c/", "..\\..\\m\\n");
        _H("//a//m//n//", "/a/b/c/", "..\\..\\m\\n");

        _H("/A/m/n", "/a/b/c", "..\\..\\m\\n");

        _H("/a", "/a/b/c", "..\\..");
        _H("/a/", "/a/b/c", "..\\..");
        _H("/a", "/a/b/c/", "..\\..");
        _H("/a/", "/a/b/c/", "..\\..");

        _H("/a/m/n", "/a", "m\\n");
        _H("/a/m/n/", "/a", "m\\n");
        _H("/a/m/n", "/a/", "m\\n");
        _H("/a/m/n/", "/a/", "m\\n");
        _H("/a//m/n", "/a", "m\\n");
#else
        _H("/a/m/n", "/a/b/c", "../../m/n");
        _H("/a/m/n/", "/a/b/c", "../../m/n");
        _H("/a/m/n", "/a/b/c/", "../../m/n");
        _H("/a/m/n/", "/a/b/c/", "../../m/n");
        _H("//a//m//n//", "/a/b/c/", "../../m/n");

        _H("/a", "/a/b/c", "../..");
        _H("/a/", "/a/b/c", "../..");
        _H("/a", "/a/b/c/", "../..");
        _H("/a/", "/a/b/c/", "../..");

        _H("/a/m/n", "/a", "m/n");
        _H("/a/m/n/", "/a", "m/n");
        _H("/a/m/n", "/a/", "m/n");
        _H("/a/m/n/", "/a/", "m/n");
        _H("/a//m/n", "/a", "m/n");
#endif


        _H("/a/m/n", "/a/m/n/", ".");

#undef _H
    }

    void test_relative_pathw()
    {
        std::wstring tmp;

#define _H(a, b, c)                             \
        tmp.clear();                            \
        Path::relative_path((a), (b), &tmp);    \
        NUT_TA(tmp == (c));

#if NUT_PLATFORM_OS_WINDOWS
        _H(L"/a/m/n", L"/a/b/c", L"..\\..\\m\\n");
        _H(L"/a/m/n/", L"/a/b/c", L"..\\..\\m\\n");
        _H(L"/a/m/n", L"/a/b/c/", L"..\\..\\m\\n");
        _H(L"/a/m/n/", L"/a/b/c/", L"..\\..\\m\\n");
        _H(L"//a//m//n//", L"/a/b/c/", L"..\\..\\m\\n");

        _H(L"/a", L"/a/b/c", L"..\\..");
        _H(L"/a/", L"/a/b/c", L"..\\..");
        _H(L"/a", L"/a/b/c/", L"..\\..");
        _H(L"/a/", L"/a/b/c/", L"..\\..");

        _H(L"/a/m/n", L"/a", L"m\\n");
        _H(L"/a/m/n/", L"/a", L"m\\n");
        _H(L"/a/m/n", L"/a/", L"m\\n");
        _H(L"/a/m/n/", L"/a/", L"m\\n");
        _H(L"/a//m/n", L"/a", L"m\\n");
#else
        _H(L"/a/m/n", L"/a/b/c", L"../../m/n");
        _H(L"/a/m/n/", L"/a/b/c", L"../../m/n");
        _H(L"/a/m/n", L"/a/b/c/", L"../../m/n");
        _H(L"/a/m/n/", L"/a/b/c/", L"../../m/n");
        _H(L"//a//m//n//", L"/a/b/c/", L"../../m/n");

        _H(L"/a", L"/a/b/c", L"../..");
        _H(L"/a/", L"/a/b/c", L"../..");
        _H(L"/a", L"/a/b/c/", L"../..");
        _H(L"/a/", L"/a/b/c/", L"../..");

        _H(L"/a/m/n", L"/a", L"m/n");
        _H(L"/a/m/n/", L"/a", L"m/n");
        _H(L"/a/m/n", L"/a/", L"m/n");
        _H(L"/a/m/n/", L"/a/", L"m/n");
        _H(L"/a//m/n", L"/a", L"m/n");
#endif

        _H(L"/a/m/n", L"/a/m/n/", L".");

#undef _H
    }

    void test_bug1()
    {
        // bug 描述:
        //     在非 windows 平台，是允许文件名中出现 ':' 字符的
        //
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(Path::is_abs("c:/"));

        NUT_TA(Path::abs_path("c:") == "c:\\");

        string a, b;
        Path::split("c:/a", &a, &b);
        NUT_TA(a == "c:/" && b == "a");

        a.clear();
        b.clear();
        Path::split_drive("c:/a/b", &a, &b);
        NUT_TA(a == "c:" && b == "/a/b");

        NUT_TA(Path::join("abc", "c:/") == "c:/");
#else
        NUT_TA(!Path::is_abs("c:/"));

        NUT_TA(Path::abs_path("c:") != "c:\\");

        string a, b;
        Path::split("c:/a", &a, &b);
        NUT_TA(a == "c:" && b == "a");

        a.clear();
        b.clear();
        Path::split_drive("c:/a/b", &a, &b);
        NUT_TA(a == "" && b == "c:/a/b");

        NUT_TA(Path::join("abc", "c:/") == "abc/c:/");
#endif
    }
};

NUT_REGISTER_FIXTURE(TestPath, "platform,quiet")
