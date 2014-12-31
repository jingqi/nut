
#include <nut/unittest/unittest.hpp>

#include <nut/container/bundle.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestBundle)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASES_END()

    void setUp() {}

    void tearDown() {}

	void testSmoking()
    {
		nut::ref<Bundle> b = gc_new<Bundle>();
		b->set_value<int>("int", 23);
		NUT_TA(b->has_key("int"));
		NUT_TA(b->get_value<int>("int") == 23);

		b->set_value<string>("str", "abc");
		NUT_TA(b->has_key("str"));
		NUT_TA(b->get_value<string>("str") == "abc");

		b->clear();
		NUT_TA(!b->has_key("int"));
    }
};

NUT_REGISTER_FIXTURE(TestBundle, "container, quiet")
