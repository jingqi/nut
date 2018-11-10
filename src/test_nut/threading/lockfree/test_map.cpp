
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/threading/lockfree/concurrent_hash_map.h>

using namespace std;
using namespace nut;

class TestConcurrentHashMap : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        ConcurrentHashMap<int,int> m;
        NUT_TA(m.insert(1, 2));
        NUT_TA(m.size() == 1);
        NUT_TA(m.contains_key(1));
        NUT_TA(!m.insert(1, 3));

        int v = 0;
        NUT_TA(m.get(1, &v));
        NUT_TA(v == 2);

        NUT_TA(m.remove(1));
        NUT_TA(m.size() == 0);
    }
};

NUT_REGISTER_FIXTURE(TestConcurrentHashMap, "threading, lockfree, quiet")
