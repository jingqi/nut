
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
        NUT_REGISTER_CASE(test_split_entries);
        NUT_REGISTER_CASE(test_split_entriesw);
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
        string dir, base;
        Path::split("ab/c/d", &dir, &base);
        NUT_TA(dir == "ab/c");
        NUT_TA(base == "d");

        dir.clear();
        base.clear();
        Path::split("/ab.txt", &dir, &base);
        NUT_TA(dir == "/");
        NUT_TA(base == "ab.txt");

        dir.clear();
        base.clear();
        Path::split("c:\\tmp", &dir, &base);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(dir == "c:\\");
        NUT_TA(base == "tmp");
#else
        NUT_TA(dir == "c:"); // 并不是当成根目录，而是当成普通目录名
        NUT_TA(base == "tmp");
#endif
    }

    void test_splitw()
    {
        wstring dir, base;
        Path::split(L"ab/c/d", &dir, &base);
        NUT_TA(dir == L"ab/c");
        NUT_TA(base == L"d");

        dir.clear();
        base.clear();
        Path::split(L"/ab.txt", &dir, &base);
        NUT_TA(dir == L"/");
        NUT_TA(base == L"ab.txt");

        dir.clear();
        base.clear();
        Path::split(L"c:\\tmp", &dir, &base);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(dir == L"c:\\");
        NUT_TA(base == L"tmp");
#else
        NUT_TA(dir == L"c:");
        NUT_TA(base == L"tmp");
#endif
    }

    void test_split_entries()
    {
        NUT_TA(Path::split_entries("/a/b") == vector<string>({"/", "a", "b"}));
        NUT_TA(Path::split_entries("a/b") == vector<string>({"a", "b"}));
        NUT_TA(Path::split_entries("\\a/.././b") == vector<string>({"/", "a", "..", ".", "b"}));

#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(Path::split_entries("c:/a/b") == vector<string>({"c:\\", "a", "b"}));
#endif
    }

    void test_split_entriesw()
    {
        NUT_TA(Path::split_entries(L"/a/b") == vector<wstring>({L"/", L"a", L"b"}));
        NUT_TA(Path::split_entries(L"a/b") == vector<wstring>({L"a", L"b"}));
        NUT_TA(Path::split_entries(L"\\a/.././b") == vector<wstring>({L"/", L"a", L"..", L".", L"b"}));

#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(Path::split_entries(L"c:/a/b") == vector<wstring>({L"c:\\", L"a", L"b"}));
#endif
    }

    void test_split_drive()
    {
        string drive, rpath;
        Path::split_drive("c:\\mn\\p", &drive, &rpath);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(drive == "c:");
        NUT_TA(rpath == "\\mn\\p");
#else
        NUT_TA(drive == "");
        NUT_TA(rpath == "c:\\mn\\p"); // 冒号当作是普通文件名中的符号
#endif

        drive.clear();
        rpath.clear();
        Path::split_drive("/mnt/sdcard", &drive, &rpath);
        NUT_TA(drive == "");
        NUT_TA(rpath == "/mnt/sdcard");
    }

    void test_split_drivew()
    {
        wstring drive, rpath;
        Path::split_drive(L"c:\\mn\\p", &drive, &rpath);
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(drive == L"c:");
        NUT_TA(rpath == L"\\mn\\p");
#else
        NUT_TA(drive == L"");
        NUT_TA(rpath == L"c:\\mn\\p");
#endif

        drive.clear();
        rpath.clear();
        Path::split_drive(L"/mnt/sdcard", &drive, &rpath);
        NUT_TA(drive == L"");
        NUT_TA(rpath == L"/mnt/sdcard");
    }

    void test_split_ext()
    {
        string prefix, ext;
        Path::split_ext("a.txt", &prefix, &ext);
        NUT_TA(prefix == "a");
        NUT_TA(ext == ".txt");

        prefix.clear();
        ext.clear();
        Path::split_ext("a/b.txt", &prefix, &ext);
        NUT_TA(prefix == "a/b");
        NUT_TA(ext == ".txt");

        prefix.clear();
        ext.clear();
        Path::split_ext("a/.bash", &prefix, &ext);
        NUT_TA(prefix == "a/.bash");
        NUT_TA(ext == "");
    }

    void test_split_extw()
    {
        wstring prefix, ext;
        Path::split_ext(L"a.txt", &prefix, &ext);
        NUT_TA(prefix == L"a");
        NUT_TA(ext == L".txt");

        prefix.clear();
        ext.clear();
        Path::split_ext(L"a/b.txt", &prefix, &ext);
        NUT_TA(prefix == L"a/b");
        NUT_TA(ext == L".txt");

        prefix.clear();
        ext.clear();
        Path::split_ext(L"a/.bash", &prefix, &ext);
        NUT_TA(prefix == L"a/.bash");
        NUT_TA(ext == L"");
    }

    void test_join()
    {
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(Path::join("a", "b") == "a\\b");
#else
        NUT_TA(Path::join("a", "b") == "a/b");
#endif

        NUT_TA(Path::join("/", "sd") == "/sd");

#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(Path::join("c:", "\\tmp") == "c:\\tmp");
#else
        NUT_TA(Path::join("c:", "\\tmp") == "\\tmp");
#endif

        NUT_TA(Path::join("a", "/b") == "/b");
    }

    void test_joinw()
    {
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(Path::join(L"a", L"b") == L"a\\b");
#else
        NUT_TA(Path::join(L"a", L"b") == L"a/b");
#endif

        NUT_TA(Path::join(L"/", L"sd") == L"/sd");

#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(Path::join(L"c:", L"\\tmp") == L"c:\\tmp");
#else
        NUT_TA(Path::join(L"c:", L"\\tmp") == L"\\tmp");
#endif

        NUT_TA(Path::join(L"a", L"/b") == L"/b");
    }

    void test_abspath()
    {
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(Path::abspath("e:\\") == "e:\\");
        NUT_TA(Path::abspath("e:") == "e:\\");
        NUT_TA(Path::abspath("e:\\..\\..") == "e:\\");
        NUT_TA(Path::abspath("e:\\..\\..\\a") == "e:\\a");
        NUT_TA(Path::abspath("e:\\b\\..\\.\\a") == "e:\\a");
        NUT_TA(Path::abspath("e:\\b\\\\a") == "e:\\b\\a");

        NUT_TA(Path::abspath("/") == "\\");
        NUT_TA(Path::abspath("/.") == "\\");
        NUT_TA(Path::abspath("/../") == "\\");
        NUT_TA(Path::abspath("/../../a") == "\\a");
        NUT_TA(Path::abspath("/a/.././c") == "\\c");
        NUT_TA(Path::abspath("/a//c") == "\\a\\c");
#else
        NUT_TA(Path::abspath("e:\\") == Path::get_cwd() + "/e:");
        NUT_TA(Path::abspath("e:") == Path::get_cwd() + "/e:");
        NUT_TA(Path::abspath("e:\\..\\..") == Path::abspath(".."));
        NUT_TA(Path::abspath("e:\\..\\..\\a") == Path::abspath("../a"));
        NUT_TA(Path::abspath("e:\\b\\..\\.\\a") == Path::get_cwd() + "/e:/a");
        NUT_TA(Path::abspath("e:\\b\\\\a") == Path::get_cwd() + "/e:/b/a");

        NUT_TA(Path::abspath("/") == "/");
        NUT_TA(Path::abspath("/.") == "/");
        NUT_TA(Path::abspath("/../") == "/");
        NUT_TA(Path::abspath("/../../a") == "/a");
        NUT_TA(Path::abspath("/a/.././c") == "/c");
        NUT_TA(Path::abspath("/a//c") == "/a/c");
#endif
    }

    void test_abspathw()
    {
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(Path::abspath(L"e:\\") == L"e:\\");
        NUT_TA(Path::abspath(L"e:") == L"e:\\");
        NUT_TA(Path::abspath(L"e:\\..\\..") == L"e:\\");
        NUT_TA(Path::abspath(L"e:\\..\\..\\a") == L"e:\\a");
        NUT_TA(Path::abspath(L"e:\\b\\..\\.\\a") == L"e:\\a");
        NUT_TA(Path::abspath(L"e:\\b\\\\a") == L"e:\\b\\a");

        NUT_TA(Path::abspath(L"/") == L"\\");
        NUT_TA(Path::abspath(L"/.") == L"\\");
        NUT_TA(Path::abspath(L"/../") == L"\\");
        NUT_TA(Path::abspath(L"/../../a") == L"\\a");
        NUT_TA(Path::abspath(L"/a/.././c") == L"\\c");
        NUT_TA(Path::abspath(L"/a//c") == L"\\a\\c");
#else
        NUT_TA(Path::abspath(L"e:\\") == Path::get_wcwd() + L"/e:");
        NUT_TA(Path::abspath(L"e:") == Path::get_wcwd() + L"/e:");
        NUT_TA(Path::abspath(L"e:\\..\\..") == Path::abspath(L".."));
        NUT_TA(Path::abspath(L"e:\\..\\..\\a") == Path::abspath(L"../a"));
        NUT_TA(Path::abspath(L"e:\\b\\..\\.\\a") == Path::get_wcwd() + L"/e:/a");
        NUT_TA(Path::abspath(L"e:\\b\\\\a") == Path::get_wcwd() + L"/e:/b/a");

        NUT_TA(Path::abspath(L"/") == L"/");
        NUT_TA(Path::abspath(L"/.") == L"/");
        NUT_TA(Path::abspath(L"/../") == L"/");
        NUT_TA(Path::abspath(L"/../../a") == L"/a");
        NUT_TA(Path::abspath(L"/a/.././c") == L"/c");
        NUT_TA(Path::abspath(L"/a//c") == L"/a/c");
#endif
    }

    void test_relpath()
    {
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(Path::relpath("/a/m/n", "/a/b/c") == "..\\..\\m\\n");
        NUT_TA(Path::relpath("/a/m/n/", "/a/b/c") == "..\\..\\m\\n");
        NUT_TA(Path::relpath("/a/m/n", "/a/b/c/") == "..\\..\\m\\n");
        NUT_TA(Path::relpath("/a/m/n/", "/a/b/c/") == "..\\..\\m\\n");
        NUT_TA(Path::relpath("//a//m//n//", "/a/b/c/") == "..\\..\\m\\n");

        NUT_TA(Path::relpath("/A/m/n", "/a/b/c") == "..\\..\\m\\n"); // Windows 下忽略大小写

        NUT_TA(Path::relpath("/a", "/a/b/c") == "..\\..");
        NUT_TA(Path::relpath("/a/", "/a/b/c") == "..\\..");
        NUT_TA(Path::relpath("/a", "/a/b/c/") == "..\\..");
        NUT_TA(Path::relpath("/a/", "/a/b/c/") == "..\\..");

        NUT_TA(Path::relpath("/a/m/n", "/a") == "m\\n");
        NUT_TA(Path::relpath("/a/m/n/", "/a") == "m\\n");
        NUT_TA(Path::relpath("/a/m/n", "/a/") == "m\\n");
        NUT_TA(Path::relpath("/a/m/n/", "/a/") == "m\\n");
        NUT_TA(Path::relpath("/a//m/n", "/a") == "m\\n");
#else
        NUT_TA(Path::relpath("/a/m/n", "/a/b/c") == "../../m/n");
        NUT_TA(Path::relpath("/a/m/n/", "/a/b/c") == "../../m/n");
        NUT_TA(Path::relpath("/a/m/n", "/a/b/c/") == "../../m/n");
        NUT_TA(Path::relpath("/a/m/n/", "/a/b/c/") == "../../m/n");
        NUT_TA(Path::relpath("//a//m//n//", "/a/b/c/") == "../../m/n");

        NUT_TA(Path::relpath("/A/m/n", "/a/b/c") == "../../../A/m/n"); // Unix 不忽略大小写

        NUT_TA(Path::relpath("/a", "/a/b/c") == "../..");
        NUT_TA(Path::relpath("/a/", "/a/b/c") == "../..");
        NUT_TA(Path::relpath("/a", "/a/b/c/") == "../..");
        NUT_TA(Path::relpath("/a/", "/a/b/c/") == "../..");

        NUT_TA(Path::relpath("/a/m/n", "/a") == "m/n");
        NUT_TA(Path::relpath("/a/m/n/", "/a") == "m/n");
        NUT_TA(Path::relpath("/a/m/n", "/a/") == "m/n");
        NUT_TA(Path::relpath("/a/m/n/", "/a/") == "m/n");
        NUT_TA(Path::relpath("/a//m/n", "/a") == "m/n");
#endif

        NUT_TA(Path::relpath("/a/m/n", "/a/m/n/") == ".");
    }

    void test_relpathw()
    {
#if NUT_PLATFORM_OS_WINDOWS
        NUT_TA(Path::relpath(L"/a/m/n", L"/a/b/c") == L"..\\..\\m\\n");
        NUT_TA(Path::relpath(L"/a/m/n/", L"/a/b/c") == L"..\\..\\m\\n");
        NUT_TA(Path::relpath(L"/a/m/n", L"/a/b/c/") == L"..\\..\\m\\n");
        NUT_TA(Path::relpath(L"/a/m/n/", L"/a/b/c/") == L"..\\..\\m\\n");
        NUT_TA(Path::relpath(L"//a//m//n//", L"/a/b/c/") == L"..\\..\\m\\n");

        NUT_TA(Path::relpath(L"/a", L"/a/b/c") == L"..\\..");
        NUT_TA(Path::relpath(L"/a/", L"/a/b/c") == L"..\\..");
        NUT_TA(Path::relpath(L"/a", L"/a/b/c/") == L"..\\..");
        NUT_TA(Path::relpath(L"/a/", L"/a/b/c/") == L"..\\..");

        NUT_TA(Path::relpath(L"/a/m/n", L"/a") == L"m\\n");
        NUT_TA(Path::relpath(L"/a/m/n/", L"/a") == L"m\\n");
        NUT_TA(Path::relpath(L"/a/m/n", L"/a/") == L"m\\n");
        NUT_TA(Path::relpath(L"/a/m/n/", L"/a/") == L"m\\n");
        NUT_TA(Path::relpath(L"/a//m/n", L"/a") == L"m\\n");
#else
        NUT_TA(Path::relpath(L"/a/m/n", L"/a/b/c") == L"../../m/n");
        NUT_TA(Path::relpath(L"/a/m/n/", L"/a/b/c") == L"../../m/n");
        NUT_TA(Path::relpath(L"/a/m/n", L"/a/b/c/") == L"../../m/n");
        NUT_TA(Path::relpath(L"/a/m/n/", L"/a/b/c/") == L"../../m/n");
        NUT_TA(Path::relpath(L"//a//m//n//", L"/a/b/c/") == L"../../m/n");

        NUT_TA(Path::relpath(L"/a", L"/a/b/c") == L"../..");
        NUT_TA(Path::relpath(L"/a/", L"/a/b/c") == L"../..");
        NUT_TA(Path::relpath(L"/a", L"/a/b/c/") == L"../..");
        NUT_TA(Path::relpath(L"/a/", L"/a/b/c/") == L"../..");

        NUT_TA(Path::relpath(L"/a/m/n", L"/a") == L"m/n");
        NUT_TA(Path::relpath(L"/a/m/n/", L"/a") == L"m/n");
        NUT_TA(Path::relpath(L"/a/m/n", L"/a/") == L"m/n");
        NUT_TA(Path::relpath(L"/a/m/n/", L"/a/") == L"m/n");
        NUT_TA(Path::relpath(L"/a//m/n", L"/a") == L"m/n");
#endif

        NUT_TA(Path::relpath(L"/a/m/n", L"/a/m/n/") == L".");
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
