
#include <nut/unittest/unittest.h>

#include <iostream>
#include <nut/container/lru_pool.h>

using namespace std;
using namespace nut;

class TestLRUPool : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        LRUPool<int,int> c(2);
        int v = 0;
        NUT_TA(!c.obtain_object(1, &v));
        c.release_object(1, 11);
        NUT_TA(c.obtain_object(1, &v) && v == 11);

        c.release_object(2, 22);
        NUT_TA(c.obtain_object(2, &v) && v == 22);

        v = 0;
        c.release_object(2, 22);
        c.release_object(2, 33);
        NUT_TA(c.size() == 2);
        NUT_TA(c.obtain_object(2, &v) && v == 33);
        NUT_TA(c.obtain_object(2, &v) && v == 22);
        NUT_TA(c.size() == 0);

        c.release_object(3, 333);
        c.clear();
        NUT_TA(c.size() == 0 && !c.obtain_object(3, &v));
    }
};

NUT_REGISTER_FIXTURE(TestLRUPool, "container, quiet")
