
#include <nut/unittest/unittest.h>

#include <nut/rc/rc_new.hpp>

using namespace nut;

static int obj_count = 0;

NUT_FIXTURE(TestGC)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    class B
    {
        NUT_REF_COUNTABLE
    };

    class A : public B
    {
    public:
        A() { ++obj_count; }
        ~A() { --obj_count; }
    };

    void set_up() {}
    void tear_down() {}

    void test_smoking()
    {
        NUT_TA(0 == obj_count);

        {
            rc_ptr<A> pa = RC_NEW(NULL, A);
            rc_ptr<A> pb;
            NUT_TA(1 == obj_count);

            {
                rc_ptr<A> pc = RC_NEW(NULL, A);
                NUT_TA(2 == obj_count);

                rc_ptr<A> pd = RC_NEW(NULL, A);
                NUT_TA(3 == obj_count);
                pb = pd;
            }

            NUT_TA(2 == obj_count);

            pb.set_null();
            NUT_TA(1 == obj_count);
        }

        NUT_TA(0 == obj_count);
    }
};

NUT_REGISTER_FIXTURE(TestGC, "gc, quiet")
