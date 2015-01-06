
#include <iostream>
#include <nut/unittest/unittest.hpp>
#include <nut/platform/os.hpp>

using namespace nut;

NUT_FIXTURE(TestOS)
{
	NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void set_up() {}
    void tear_down() {}

    void test_smoking()
	{
	}

};

NUT_REGISTER_FIXTURE(TestOS, "platform,quiet")
