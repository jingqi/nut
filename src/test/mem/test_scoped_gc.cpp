
#include <nut/unittest/unittest.h>

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

    void set_up() {}
    void tear_down() {}

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

    // 测试可变参数宏
    void test_var_macro()
    {
        rc_ptr<C> p = rc_new<C>();
        p = rc_new<C>(1);
        p = rc_new<C>(1, 2);

        D *pp = ma_new<D>(NULL);
        ma_delete(NULL, pp);
        pp = ma_new<D>(NULL, 1);
        ma_delete(NULL, pp);
        pp = ma_new<D>(NULL, 1, 2);
        ma_delete(NULL, pp);
    }
};

NUT_REGISTER_FIXTURE(TestScopedGC, "mem, quiet")
