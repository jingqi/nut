
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/platform/sys.h>

using namespace std;
using namespace nut;

class TestSys : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        const int num = Sys::get_processor_num();
        NUT_TA(num > 0);
    }

};

NUT_REGISTER_FIXTURE(TestSys, "platform,quiet")
