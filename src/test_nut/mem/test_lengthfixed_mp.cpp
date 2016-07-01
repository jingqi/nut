
#include <nut/unittest/unit_test.h>

#include <nut/mem/lengthfixed_mp.h>
#include <nut/rc/rc_new.h>

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

    void test_smoking()
	{
        rc_ptr<lengthfixed_mp> mp = rc_new<lengthfixed_mp>(sizeof(A));
        A *p1 = (A*) mp->alloc(sizeof(A));
		NUT_TA(NULL != p1);
        p1->a = 0x12345678;

        A *p2 = (A*) mp->alloc(sizeof(A));
		NUT_TA(NULL != p2 && p1 != p2);
        p2->a = ~0;

        mp->free(p1, sizeof(A));
        A *p3 = (A*) mp->alloc(sizeof(A));
		NUT_TA(p3 == p1);
        p3->a = 0xef98612a;

        mp->free(p2, sizeof(A));
        A *p4 = (A*) mp->alloc(sizeof(A));
		NUT_TA(p4 == p2);
        p4->a = ~0;

        mp->free(p3, sizeof(A));
        mp->free(p4, sizeof(A));
	}
};

NUT_REGISTER_FIXTURE(TestLengthFixedMP, "mem, quiet")
