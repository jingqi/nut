
#include <nut/unittest/unittest.hpp>

#include <nut/mem/scoped_gc.hpp>
#include <nut/gc/gc.hpp>

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
            nut::ref<scoped_gc> gc = GC_NEW(NULL, scoped_gc);

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
        NUT_GC_REFERABLE
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
        nut::ref<C> p = GC_NEW(NULL, C);
        p = GC_NEW(NULL, C, 1);
        p = GC_NEW(NULL, C, 1, 2);

        D *pp = MA_NEW(NULL, D);
        MA_DELETE(NULL, pp, D);
        pp = MA_NEW(NULL, D, 1);
        MA_DELETE(NULL, pp, D);
        pp = MA_NEW(NULL, D, 1, 2);
        MA_DELETE(NULL, pp, D);
    }
};

NUT_REGISTER_FIXTURE(TestScopedGC, "mem, quiet")
