
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
		b->set<int>("int", 23);
		NUT_TA(b->hasKey("int") && b->hasKeyT<int>("int"));
		NUT_TA(b->get<int>("int") == 23);

		b->set<string>("str", "abc");
		NUT_TA(b->hasKey("str") && b->hasKeyT<string>("str"));
		NUT_TA(b->get<string>("str") == "abc");

		b->clear();
		NUT_TA(!b->hasKey("int") && !b->hasKeyT<string>("str"));
    }
};

NUT_REGISTER_FIXTURE(TestBundle, "container, quiet")
