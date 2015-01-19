
#include <nut/unittest/unittest.hpp>

#include <nut/mem/scoped_gc.hpp>

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
            scoped_gc<> *gc = scoped_gc<>::create();

            gc->gc_new<A>();
			assert(1 == obj_count);

            gc->gc_new<A>();
			assert(2 == obj_count);

            gc->gc_new_array<A>(3);
			assert(5 == obj_count);

            gc->rls_ref();
		}

		NUT_TA(0 == obj_count);
	}
};

NUT_REGISTER_FIXTURE(TestScopedGC, "mem, quiet")
