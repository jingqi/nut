
#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <nut/unittest/unittest.hpp>
#include <nut/unittest/streamtestlogger.hpp>
#include <nut/unittest/testrunner.hpp>
#include <nut/logging/logmanager.hpp>

using namespace std;
using namespace nut;

int main()
{
    StreamTestLogger l(cout);
    TestRunner trunner(l);
    trunner.runAGroup("logging");
    // trunner.runAFixture("TestGC");
    // trunner.runACase("TestGC", "testSmoking");

    printf("press any key to continue...");
    getch();
    return 0;
}

NUT_LOGGING_IMPL
NUT_UNITTEST_IMPL
