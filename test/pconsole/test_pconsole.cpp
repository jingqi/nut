
#include <nut/unittest/unittest.hpp>

#include <iostream>
#include <nut/pconsole/pconsole.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestPConsole)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()


    void set_up() {}
    void tear_down() {}

    void test_smoking()
    {
        rc_ptr<PConsole> con = RC_NEW(NULL, PConsole);
        printf("\n");
        con->execute();
    }

};

NUT_REGISTER_FIXTURE(TestPConsole, "pconsole")
