
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
    NUT_CASE(test_abspathw)
    NUT_CASES_END()

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
        NUT_TA(tmp == "c:\\tmp");
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
        NUT_TA(tmp == L"c:\\tmp");
    }

    void test_abspath()
    {
        // cout << Path::abspath("/a//c");

        std::string tmp;
        Path::abs_path("e:\\", &tmp);
        NUT_TA(tmp == "e:\\");

        tmp.clear();
        Path::abs_path("e:", &tmp);
        NUT_TA(tmp == "e:\\");

        tmp.clear();
        Path::abs_path("e:\\..\\..", &tmp);
        NUT_TA(tmp == "e:\\");

        tmp.clear();
        Path::abs_path("e:\\..\\..\\a", &tmp);
        NUT_TA(tmp == "e:\\a");

        tmp.clear();
        Path::abs_path("e:\\b\\..\\.\\a", &tmp);
        NUT_TA(tmp == "e:\\a");

        tmp.clear();
        Path::abs_path("e:\\b\\\\a", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == "e:\\b\\a");
#else
        NUT_TA(tmp == "e:\\b/a");
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


    void test_abspathw()
    {
        // cout << Path::abspath("/a//c");

        std::wstring tmp;
        Path::abs_path(L"e:\\", &tmp);
        NUT_TA(tmp == L"e:\\");

        tmp.clear();
        Path::abs_path(L"e:", &tmp);
        NUT_TA(tmp == L"e:\\");

        tmp.clear();
        Path::abs_path(L"e:\\..\\..", &tmp);
        NUT_TA(tmp == L"e:\\");

        tmp.clear();
        Path::abs_path(L"e:\\..\\..\\a", &tmp);
        NUT_TA(tmp == L"e:\\a");

        tmp.clear();
        Path::abs_path(L"e:\\b\\..\\.\\a", &tmp);
        NUT_TA(tmp == L"e:\\a");

        tmp.clear();
        Path::abs_path(L"e:\\b\\\\a", &tmp);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(tmp == L"e:\\b\\a");
#else
        NUT_TA(tmp == L"e:\\b/a");
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
};

NUT_REGISTER_FIXTURE(TestPath, "platform,quiet")
