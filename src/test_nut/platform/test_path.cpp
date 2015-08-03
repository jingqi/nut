
#include <iostream>
#include <nut/unittest/unit_test.h>
#include <nut/platform/path.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestPath)
{
	NUT_CASES_BEGIN()
    NUT_CASE(test_split)
    NUT_CASE(test_splitw)
    NUT_CASE(test_split_drive)
    NUT_CASE(test_split_drivew)
    NUT_CASE(test_split_ext)
    NUT_CASE(test_split_extw)
    NUT_CASE(test_join)
    NUT_CASE(test_joinw)
    NUT_CASE(test_abspath)
    NUT_CASES_END()

    void set_up() {}
    void tear_down() {}

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
        NUT_TA(p == "c:\\");
        NUT_TA(c == "tmp");
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
        NUT_TA(p == L"c:\\");
        NUT_TA(c == L"tmp");
	}

    void test_split_drive()
    {
        string d, r;
        Path::split_drive("c:\\mn\\p", &d, &r);
        NUT_TA(d == "c:");
        NUT_TA(r == "\\mn\\p");

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
        NUT_TA(d == L"c:");
        NUT_TA(r == L"\\mn\\p");

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
#if defined(NUT_PLATFORM_OS_WINDOWS)
        NUT_TA(Path::join("a", "b") == "a\\b");
#else
        NUT_TA(Path::join("a", "b") == "a/b");
#endif

        NUT_TA(Path::join("/", "sd") == "/sd");
        NUT_TA(Path::join("c:", "\\tmp") == "c:\\tmp");
    }

    void test_joinw()
    {
#if defined(NUT_PLATFORM_OS_WINDOWS)
        NUT_TA(Path::join(L"a", L"b") == L"a\\b");
#else
        NUT_TA(Path::join(L"a", L"b") == L"a/b");
#endif

        NUT_TA(Path::join(L"/", L"sd") == L"/sd");
        NUT_TA(Path::join(L"c:", L"\\tmp") == L"c:\\tmp");
    }

    void test_abspath()
    {
        // cout << Path::abspath("/a//c");

        NUT_TA(Path::abs_path("e:\\") == "e:\\");
        NUT_TA(Path::abs_path("e:") == "e:\\");
        NUT_TA(Path::abs_path("e:\\..\\..") == "e:\\");
        NUT_TA(Path::abs_path("e:\\..\\..\\a") == "e:\\a");
        NUT_TA(Path::abs_path("e:\\b\\..\\.\\a") == "e:\\a");
#if defined(NUT_PLATFORM_OS_WINDOWS)
        NUT_TA(Path::abs_path("e:\\b\\\\a") == "e:\\b\\a");
#else
        NUT_TA(Path::abs_path("e:\\b\\\\a") == "e:\\b/a");
#endif

        NUT_TA(Path::abs_path("/") == "/");
        NUT_TA(Path::abs_path("/.") == "/");
        NUT_TA(Path::abs_path("/../") == "/");
        NUT_TA(Path::abs_path("/../../a") == "/a");
        NUT_TA(Path::abs_path("/a/.././c") == "/c");
#if defined(NUT_PLATFORM_OS_WINDOWS)
        NUT_TA(Path::abs_path("/a//c") == "/a\\c");
#else
        NUT_TA(Path::abs_path("/a//c") == "/a/c");
#endif

    }
};

NUT_REGISTER_FIXTURE(TestPath, "platform,quiet")
