
#include <nut/unittest/unit_test.h>

#include <nut/mem/scoped_gc.h>
#include <nut/rc/rc_new.h>

using namespace nut;

static int obj_count = 0;

NUT_FIXTURE(TestScopedGC)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    class A
    {
    public:
        A() { ++obj_count; }
        ~A() { --obj_count; }
    };

    void test_smoking()
    {
        NUT_TA(0 == obj_count);

        {
            rc_ptr<scoped_gc> gc = rc_new<scoped_gc>();

            gc->gc_new<A>();
            assert(1 == obj_count);

            gc->gc_new<A>();
            assert(2 == obj_count);

            gc->gc_new_array<A>(3);
            assert(5 == obj_count);
        }

        NUT_TA(0 == obj_count);
    }

    class C
    {
        NUT_REF_COUNTABLE
    public:
        C() {}
        C(int) {}
        C(int, int) {}
    };

    class D
    {
    public:
        D() {}
        D(int) {}
        D(int, int) {}
    };
};

NUT_REGISTER_FIXTURE(TestScopedGC, "mem, quiet")
