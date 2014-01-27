
#include <nut/unittest/unittest.hpp>

#include <iostream>
#include <nut/pconsole/pconsole.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestPConsole)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASES_END()


    void setUp() {}
    void tearDown() {}

    void testSmoking()
    {
        nut::ref<PConsole> con = gc_new<PConsole>();
        printf("\n");
        con->execute();
    }

};

NUT_REGISTER_FIXTURE(TestPConsole, "pconsole")
