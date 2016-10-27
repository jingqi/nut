
#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_WINDOWS
#   include <conio.h>
#endif

#include <stdio.h>
#include <iostream>
#include <nut/unittest/unit_test.h>
#include <nut/unittest/console_test_logger.h>
#include <nut/unittest/stream_test_logger.h>
#include <nut/unittest/test_runner.h>
#include <nut/logging/logger.h>

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
#elif NUT_PLATFORM_OS_MAC
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
#if NUT_PLATFORM_OS_LINUX
    // 解决 std::wcout 无法显示中文以及 char/wchar_t 相互转换问题
    setlocale(LC_ALL, "zh_CN.UTF8");
#endif

    ::srand((unsigned) ::time(NULL));
	print_platform();

#if NUT_PLATFORM_CC_VC
    ConsoleTestLogger l;
#else
    StreamTestLogger l(&std::cout);
#endif

    TestRunner runner(&l);
    if (argc <= 1)
    {
        // default action
        runner.run_group("quiet");
        // runner.run_fixture("TestTimeWheel");
    }
    else
    {
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
                }
                else
                {
                    std::cerr << "need fixture name of case, and case name!" <<
                        std::endl;
                    print_help();
                }
                i += 2;
            }
            else
            {
                std::cerr << "unknown option: " << arg << std::endl;
                print_help();
                return -1;
            }
        }
    }

#if NUT_PLATFORM_OS_WINDOWS
    printf("press any key to continue...");
    getch();
#endif

    return l.get_count_of_failed_cases();
}
