
#include <nut/unittest/unittest.hpp>

#include <nut/mem/lengthfixed_mp.hpp>
#include <nut/gc/gc.hpp>

using namespace nut;

NUT_FIXTURE(TestLengthFixedMP)
{
	NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
	NUT_CASES_END()

	struct A
    {
        int a;
    };

    void set_up() {}
    void tear_down() {}

    void test_smoking()
	{
        nut::ref<lengthfixed_mp<sizeof(A)> > mp = gc_new<lengthfixed_mp<sizeof(A)> >(NULL);
        void *p1 = mp->alloc();
		NUT_TA(NULL != p1);

        void *p2 = mp->alloc();
		NUT_TA(NULL != p2 && p1 != p2);

        mp->free(p1);
        void *p3 = mp->alloc();
		NUT_TA(p3 == p1);

        mp->free(p2);
        void *p4 = mp->alloc();
		NUT_TA(p4 == p2);

        mp->free(p3);
        mp->free(p4);
	}
};

NUT_REGISTER_FIXTURE(TestLengthFixedMP, "mem, quiet")
