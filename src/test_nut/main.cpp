
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

int main()
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

    TestRunner trunner(&l);

    trunner.run_group("quiet");

    // trunner.run_fixture("TestIntegerSet");

    // trunner.run_case("TestNumericAlgo", "test_karatsuba_multiply");

    printf("press any key to continue...");
#if NUT_PLATFORM_OS_WINDOWS
    getch();
#endif

    return l.get_count_of_failed_cases();
}
