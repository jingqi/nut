
#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <conio.h>
#endif

#include <stdio.h>
#include <iostream>
#include <nut/unittest/unittest.hpp>
#include <nut/unittest/consoletestlogger.hpp>
#include <nut/unittest/testrunner.hpp>
#include <nut/logging/logmanager.hpp>

using namespace std;
using namespace nut;

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(disable: 4996 4190)
#endif

int main()
{
#if defined(NUT_PLATFORM_OS_LINUX)
    // 解决 wcout 无法显示中文以及 char/wchar_t 相互转换问题
    setlocale(LC_ALL, "zh_CN.UTF8");
#endif

    // srand(time(NULL));

    printf("\n%s\n", NUT_PLATFORM_STR);

    ConsoleTestLogger l;
    TestRunner trunner(l);

    // trunner.runAGroup("pconsole");

    // trunner.runAFixture("TestBigInteger");
    //trunner.runAFixture("TestNumericAlgo");
     trunner.runAFixture("TestRSA");

    // trunner.runACase("TestGC", "testSmoking");

    printf("press any key to continue...");
#if defined(NUT_PLATFORM_OS_WINDOWS)
    getch();
#endif

    return 0;
}

NUT_LOGGING_IMPL
NUT_UNITTEST_IMPL

