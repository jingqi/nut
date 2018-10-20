
#include <nut/unittest/unittest.h>

#include <nut/container/bundle.h>

using namespace std;
using namespace nut;

class TestBundle : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        rc_ptr<Bundle> b = rc_new<Bundle>();
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
