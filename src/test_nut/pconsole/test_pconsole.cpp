
#include <nut/unittest/unittest.h>

#include <iostream>
#include <nut/pconsole/pconsole.h>

using namespace std;
using namespace nut;

class TestPConsole : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        rc_ptr<PConsole> con = rc_new<PConsole>();
        printf("\n");
        con->execute();
    }

};

NUT_REGISTER_FIXTURE(TestPConsole, "pconsole")
