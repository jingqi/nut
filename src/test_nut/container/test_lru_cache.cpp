
#include <nut/unittest/unittest.h>

#include <iostream>
#include <nut/container/lru_cache.h>

using namespace std;
using namespace nut;

class TestLRUCache : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        LRUCache<int,int> c(2);
        NUT_TA(c.get(1) == nullptr);
        c.put(1, 11);
        NUT_TA(*c.get(1) == 11);
        c.put(2, 22);
        NUT_TA(*c.get(2) == 22);
        c.put(3, 33);
        NUT_TA(*c.get(3) == 33);
        NUT_TA(c.get(1) == nullptr);

        c.remove(3);
        NUT_TA(c.get(3) == nullptr);

        c.clear();
        NUT_TA(c.get(2) == nullptr);
    }
};

NUT_REGISTER_FIXTURE(TestLRUCache, "container, quiet")
