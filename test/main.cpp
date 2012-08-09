
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

int main()
{
    printf("\n%s\n", NUT_PLATFORM_STR);

    ConsoleTestLogger l;
    TestRunner trunner(l);
    trunner.runAGroup("util");
    // trunner.runAFixture("TestGC");
    // trunner.runACase("TestGC", "testSmoking");

    printf("press any key to continue...");
#if defined(NUT_PLATFORM_OS_WINDOWS)
    getch();
#else
    getchar();
#endif

    return 0;
}

NUT_LOGGING_IMPL
NUT_UNITTEST_IMPL


