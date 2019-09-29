
#include <stdio.h>
#include <time.h> // for time()
#include <iostream>

#include <nut/nut.h> // check if "nut.h" collect the right header files

#if NUT_PLATFORM_OS_WINDOWS
#   include <conio.h>
#endif


using namespace std;
using namespace nut;

#if NUT_PLATFORM_CC_VC
#   pragma warning(disable: 4996 4190)
#endif

static void print_platform()
{
    printf("\nOS: %s, Bits: %s, Compiler: %s\n",
        // OS
#if NUT_PLATFORM_OS_WINDOWS
        "Windows"
#elif NUT_PLATFORM_OS_MACOS
        "Mac"
#elif NUT_PLATFORM_OS_LINUX
        "Linux"
#else
        "Unknown"
#endif
        ,

        // Bits
#if NUT_PLATFORM_BITS_16
        "16"
#elif NUT_PLATFORM_BITS_32
        "32"
#elif NUT_PLATFORM_BITS_64
        "64"
#else
        "Unknown"
#endif
        ,

        // Compiler
#if NUT_PLATFORM_CC_VC
        "VC"
#elif NUT_PLATFORM_CC_MINGW
        "MINGW"
#elif NUT_PLATFORM_CC_GCC
        "GCC"
#else
        "Unknown"
#endif
    );
}

static void print_help()
{
    std::cout << "test_nut [-h] [-g GROUP] [-f FIXTURE] [-c CASE_FIXTURE CASE_NAME]" <<
        std::endl;
}

int main(int argc, char *argv[])
{
#if !NUT_PLATFORM_OS_WINDOWS
    // NOTE 在 Linux/Macos 下, 使用操作系统的 locale 设置来设置程序的 locale, 解
    //      决 std::wcout 无法显示中文以及 char/wchar_t 相互转换失败的问题
    // - 不设置时，默认 locale 是 "C", 无地域信息
    // - 第一个参数可以是 LC_ALL / LC_CTYPE, LC_CTYPE 表示 "字符分类和转换"
    // - 第二个参数为 NULL 时, 仅做查询操作, 此时 locale 保持不变
    // - 第二个参数为 "" 时, 表示使用操作系统的 locale 设置给当前程序
    // - 有可能设置会失败(如操作系统并未安装相应语言包), 此时 locale 保持不变
    ::setlocale(LC_CTYPE, "");
#endif

    ::srand((unsigned) ::time(nullptr));
    print_platform();

    ConsoleTestLogger l;
    TestRunner runner(&l);
    bool has_run = false, quiet = false;
    for (int i = 1; i < argc; ++i)
    {
        const char *arg = argv[i];
        if (0 == ::strcmp(arg, "-h"))
        {
            print_help();
        }
        else if (0 == ::strcmp(arg, "-g"))
        {
            if (i + 1 < argc)
            {
                runner.run_group(argv[i + 1]);
                has_run = true;
            }
            else
            {
                std::cerr << "need group name!" << std::endl;
                print_help();
            }
            i += 1;
        }
        else if (0 == ::strcmp(arg, "-f"))
        {
            if (i + 1 < argc)
            {
                runner.run_fixture(argv[i + 1]);
                has_run = true;
            }
            else
            {
                std::cerr << "need fixture name!" << std::endl;
                print_help();
            }
            i += 1;
        }
        else if (0 == ::strcmp(arg, "-c"))
        {
            if (i + 2 < argc)
            {
                runner.run_case(argv[i + 1], argv[i + 2]);
                has_run = true;
            }
            else
            {
                std::cerr << "need fixture name of case, and case name!" <<
                    std::endl;
                print_help();
            }
            i += 2;
        }
        else if (0 == ::strcmp(arg, "-q"))
        {
            quiet = true;
        }
        else
        {
            std::cerr << "unknown option: " << arg << std::endl;
            print_help();
            return -1;
        }
    }

    if (!has_run)
    {
        // default action
        runner.run_group("quiet");
        // runner.run_fixture("TestTimeWheel");
        // runner.run_case("TestTimeWheel", "test_bug1");
    }

    HPRecord::clear();

#if NUT_PLATFORM_OS_WINDOWS
    if (!quiet && ConsoleUtil::isatty())
    {
        printf("press any key to continue...");
        getch();
    }
#endif
    UNUSED(quiet);

    return l.get_count_of_failed_cases();
}
