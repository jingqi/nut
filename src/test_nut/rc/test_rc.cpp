﻿
#include <iostream>
#include <nut/unittest/unittest.h>

#include <nut/rc/rc_new.h>

using namespace nut;

static int obj_count = 0;

class TestRC : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

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

NUT_REGISTER_FIXTURE(TestRC, "rc, quiet")
