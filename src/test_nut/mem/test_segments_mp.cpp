
#include <nut/unittest/unit_test.h>

#include <nut/mem/segments_mp.h>
#include <nut/mem/sys_ma.h>
#include <nut/rc/rc_new.h>

using namespace nut;

NUT_FIXTURE(TestSegmentsMP)
{
	NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASE(test_profile)
	NUT_CASES_END()

    void test_smoking()
	{
		rc_ptr<sys_ma> sma = rc_new<sys_ma>();
        rc_ptr<segments_mp> mp = rc_new<segments_mp>(sma);
        void *p1 = mp->alloc(1);
		NUT_TA(NULL != p1);

        void *p2 = mp->alloc(17);
		NUT_TA(NULL != p2 && p1 != p2);

        mp->free(p1, 1);
        void *p3 = mp->alloc(8);
		NUT_TA(p3 == p1);

        mp->free(p2, 17);
        void *p4 = mp->alloc(24);
		NUT_TA(p4 == p2);

        mp->free(p3, 8);
        mp->free(p4, 24);
	}

    void test_profile()
    {
        clock_t start = clock();
        const int ROUND = 20000, MAX_SZ = 8 * 128;
        {
            rc_ptr<segments_mp> mp = rc_new<segments_mp>();
            // rc_ptr<sys_ma> mp = rc_new<sys_ma>();
            for (size_t i = 0; i < ROUND; ++i)
            {
                for (size_t j = 1; j < MAX_SZ; ++j)
                {
                    void *p = mp->alloc(j);
                    mp->free(p, j);
                }
            }
        }
        clock_t middle = clock();
        {
            for (size_t i = 0; i < ROUND; ++i)
            {
                for (size_t j = 1; j < MAX_SZ; ++j)
                {
                    void *p = ::malloc(j);
                    ::free(p);
                }
            }
        }
        clock_t finish = clock();
        printf(" %ld ms(origin %ld ms)", (middle - start) * 1000 / CLOCKS_PER_SEC,
               (finish - middle) * 1000 / CLOCKS_PER_SEC);
    }
};

NUT_REGISTER_FIXTURE(TestSegmentsMP, "mem, quiet")
