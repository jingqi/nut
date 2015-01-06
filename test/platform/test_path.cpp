
#include <iostream>
#include <nut/unittest/unittest.hpp>
#include <nut/platform/path.hpp>

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
        NUT_TA(Path::split("ab/c/d").first == "ab/c");
        NUT_TA(Path::split("ab/c/d").second == "d");

        NUT_TA(Path::split("/ab.txt").first == "/");
        NUT_TA(Path::split("/ab.txt").second == "ab.txt");

        NUT_TA(Path::split("c:\\tmp").first == "c:\\");
        NUT_TA(Path::split("c:\\tmp").second == "tmp");
	}

    void test_splitw()
	{
        NUT_TA(Path::split(L"ab/c/d").first == L"ab/c");
        NUT_TA(Path::split(L"ab/c/d").second == L"d");

        NUT_TA(Path::split(L"/ab.txt").first == L"/");
        NUT_TA(Path::split(L"/ab.txt").second == L"ab.txt");

        NUT_TA(Path::split(L"c:\\tmp").first == L"c:\\");
        NUT_TA(Path::split(L"c:\\tmp").second == L"tmp");
	}

    void test_split_drive()
    {
        NUT_TA(Path::splitdrive("c:\\mn\\p").first == "c:");
        NUT_TA(Path::splitdrive("c:\\mn\\p").second == "\\mn\\p");

        NUT_TA(Path::splitdrive("/mnt/sdcard").first == "");
        NUT_TA(Path::splitdrive("/mnt/sdcard").second == "/mnt/sdcard");
    }

    void test_split_drivew()
    {
        NUT_TA(Path::splitdrive(L"c:\\mn\\p").first == L"c:");
        NUT_TA(Path::splitdrive(L"c:\\mn\\p").second == L"\\mn\\p");

        NUT_TA(Path::splitdrive(L"/mnt/sdcard").first == L"");
        NUT_TA(Path::splitdrive(L"/mnt/sdcard").second == L"/mnt/sdcard");
    }

    void test_split_ext()
    {
        NUT_TA(Path::splitext("a.txt").first == "a");
        NUT_TA(Path::splitext("a.txt").second == ".txt");
    }

    void test_split_extw()
    {
        NUT_TA(Path::splitext(L"a.txt").first == L"a");
        NUT_TA(Path::splitext(L"a.txt").second == L".txt");
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

        NUT_TA(Path::abspath("e:\\") == "e:\\");
        NUT_TA(Path::abspath("e:") == "e:\\");
        NUT_TA(Path::abspath("e:\\..\\..") == "e:\\");
        NUT_TA(Path::abspath("e:\\..\\..\\a") == "e:\\a");
        NUT_TA(Path::abspath("e:\\b\\..\\.\\a") == "e:\\a");
#if defined(NUT_PLATFORM_OS_WINDOWS)
        NUT_TA(Path::abspath("e:\\b\\\\a") == "e:\\b\\a");
#else
        NUT_TA(Path::abspath("e:\\b\\\\a") == "e:\\b/a");
#endif

        NUT_TA(Path::abspath("/") == "/");
        NUT_TA(Path::abspath("/.") == "/");
        NUT_TA(Path::abspath("/../") == "/");
        NUT_TA(Path::abspath("/../../a") == "/a");
        NUT_TA(Path::abspath("/a/.././c") == "/c");
#if defined(NUT_PLATFORM_OS_WINDOWS)
        NUT_TA(Path::abspath("/a//c") == "/a\\c");
#else
        NUT_TA(Path::abspath("/a//c") == "/a/c");
#endif

    }
};

NUT_REGISTER_FIXTURE(TestPath, "platform,quiet")
