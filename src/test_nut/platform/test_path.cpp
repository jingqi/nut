
#include <iostream>
#include <nut/unittest/unittest.h>
#include <nut/platform/path.h>
#include <nut/util/string/string_utils.h>

using namespace std;
using namespace nut;

class TestPath : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_is_root);
        NUT_REGISTER_CASE(test_split);
        NUT_REGISTER_CASE(test_splitw);
        NUT_REGISTER_CASE(test_split_drive);
        NUT_REGISTER_CASE(test_split_drivew);
        NUT_REGISTER_CASE(test_split_ext);
        NUT_REGISTER_CASE(test_split_extw);
        NUT_REGISTER_CASE(test_join);
        NUT_REGISTER_CASE(test_joinw);
        NUT_REGISTER_CASE(test_abspath);
        NUT_REGISTER_CASE(test_abspathw);
        NUT_REGISTER_CASE(test_relpath);
        NUT_REGISTER_CASE(test_relpathw);
        NUT_REGISTER_CASE(test_bug1);
        NUT_REGISTER_CASE(test_bug2);
        NUT_REGISTER_CASE(test_bug3);
    }

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
        std::string tmp = Path::join("a", "b");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "a\\b");
#else
        NUT_TA(tmp == "a/b");
#endif

        tmp = Path::join("/", "sd");
        NUT_TA(tmp == "/sd");

        tmp = Path::join("c:", "\\tmp");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "c:\\tmp");
#else
        NUT_TA(tmp == "\\tmp");
#endif

        NUT_TA(Path::join("a", "/b") == "/b");
    }

    void test_joinw()
    {
        std::wstring tmp = Path::join(L"a", L"b");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"a\\b");
#else
        NUT_TA(tmp == L"a/b");
#endif

        tmp = Path::join(L"/", L"sd");
        NUT_TA(tmp == L"/sd");

        tmp = Path::join(L"c:", L"\\tmp");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"c:\\tmp");
#else
        NUT_TA(tmp == L"\\tmp");
#endif

        NUT_TA(Path::join(L"a", L"/b") == L"/b");
    }

    void test_abspath()
    {
        // cout << Path::abspath("/a//c");

        std::string tmp = Path::abspath("e:\\");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "e:\\");
#else
        NUT_TA(tmp == Path::get_cwd() + "/e:");
#endif

        tmp = Path::abspath("e:");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "e:\\");
#else
        NUT_TA(tmp == Path::get_cwd() + "/e:");
#endif

        tmp = Path::abspath("e:\\..\\..");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "e:\\");
#else
        NUT_TA(tmp == Path::abspath(".."));
#endif

        tmp = Path::abspath("e:\\..\\..\\a");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "e:\\a");
#else
        NUT_TA(tmp == Path::abspath("../a"));
#endif

        tmp = Path::abspath("e:\\b\\..\\.\\a");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "e:\\a");
#else
        NUT_TA(tmp == Path::get_cwd() + "/e:/a");
#endif

        tmp = Path::abspath("e:\\b\\\\a");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "e:\\b\\a");
#else
        NUT_TA(tmp == Path::get_cwd() + "/e:/b/a");
#endif

        tmp = Path::abspath("/");
        NUT_TA(tmp == "/");

        tmp = Path::abspath("/.");
        NUT_TA(tmp == "/");

        tmp = Path::abspath("/../");
        NUT_TA(tmp == "/");

        tmp = Path::abspath("/../../a");
        NUT_TA(tmp == "/a");

        tmp = Path::abspath("/a/.././c");
        NUT_TA(tmp == "/c");

        tmp = Path::abspath("/a//c");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "/a\\c");
#else
        NUT_TA(tmp == "/a/c");
#endif
    }

    void test_abspathw()
    {
        // cout << Path::abspath("/a//c");

        std::wstring tmp = Path::abspath(L"e:\\");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"e:\\");
#else
        NUT_TA(tmp == Path::get_wcwd() + L"/e:");
#endif

        tmp = Path::abspath(L"e:");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"e:\\");
#else
        NUT_TA(tmp == Path::get_wcwd() + L"/e:");
#endif

        tmp = Path::abspath(L"e:\\..\\..");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"e:\\");
#else
        NUT_TA(tmp == Path::abspath(L".."));
#endif

        tmp = Path::abspath(L"e:\\..\\..\\a");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"e:\\a");
#else
        NUT_TA(tmp == Path::abspath(L"../a"));
#endif

        tmp = Path::abspath(L"e:\\b\\..\\.\\a");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"e:\\a");
#else
        NUT_TA(tmp == Path::get_wcwd() + L"/e:/a");
#endif

        tmp = Path::abspath(L"e:\\b\\\\a");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"e:\\b\\a");
#else
        NUT_TA(tmp == Path::get_wcwd() + L"/e:/b/a");
#endif

        tmp = Path::abspath(L"/");
        NUT_TA(tmp == L"/");

        tmp = Path::abspath(L"/.");
        NUT_TA(tmp == L"/");

        tmp = Path::abspath(L"/../");
        NUT_TA(tmp == L"/");

        tmp = Path::abspath(L"/../../a");
        NUT_TA(tmp == L"/a");

        tmp = Path::abspath(L"/a/.././c");
        NUT_TA(tmp == L"/c");

        tmp = Path::abspath(L"/a//c");
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"/a\\c");
#else
        NUT_TA(tmp == L"/a/c");
#endif
    }

    void test_relpath()
    {
        std::string tmp;

#define _H(a, b, c)                     \
        tmp = Path::relpath((a), (b));  \
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

    void test_relpathw()
    {
        std::wstring tmp;

#define _H(a, b, c)                       \
        tmp = Path::relpath((a), (b));    \
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

        NUT_TA(Path::abspath("c:") == "c:\\");

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

        NUT_TA(Path::abspath("c:") != "c:\\");

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

    void test_bug2()
    {
        // bug 描述：
        //     is_abs(const wchar_t*) 在 windows 下对 find_win_drive() 返回值的处理错误
        NUT_TA(!Path::is_abs(L"."));
    }

    void test_bug3()
    {
        // bug 描述:
        //     Windows 下驱动号只应该允许一个 a - z 的字母
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(!Path::is_root("ab:/"));
        NUT_TA(Path::join("m", "b:/") == "b:/");
        NUT_TA(Path::join("m", "ab:/") == "m\\ab:/");
#endif
    }
};

NUT_REGISTER_FIXTURE(TestPath, "platform,quiet")
