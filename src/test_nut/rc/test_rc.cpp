
#include <iostream>
#include <nut/unittest/unit_test.h>

#include <nut/rc/rc_new.h>

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

    void test_smoking()
    {
        NUT_TA(0 == obj_count);

        {
            rc_ptr<A> pa = rc_new<A>();
            rc_ptr<A> pb;
            NUT_TA(1 == obj_count);

            {
                rc_ptr<A> pc = rc_new<A>();
                NUT_TA(2 == obj_count);

                rc_ptr<A> pd = rc_new<A>();
                NUT_TA(3 == obj_count);
                pb = pd;
            }

            NUT_TA(2 == obj_count);

            pb.set_null();
            NUT_TA(1 == obj_count);
        }

        NUT_TA(0 == obj_count);
    }

    class G
    {
        NUT_REF_COUNTABLE
    public:
        G() {}
        G(int, bool, double) {}
        G(int*,double*) {}
        G(std::ostream &) {}
        G(int, const std::ostream &) {}
    };

    void test_grammaar()
    {
        rc_new<G>();
        rc_new<G>(1, true, 1.5);
        int a;
        rc_new<G>(&a, nullptr);
        rc_new<G>(std::cout);
        rc_new<G>(1, std::cout);
    }
};

NUT_REGISTER_FIXTURE(TestGC, "gc, quiet")
