﻿
#include <nut/unittest/unittest.h>

#include <iostream>
#include <nut/container/lru_cache.h>

using namespace std;
using namespace nut;

class TestLRUCache : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        LRUCache<int,int> c(2);
        int v = -1;
        NUT_TA(!c.get(1,&v) && -1 == v);
        c.put(1,11);
        NUT_TA(c.get(1,&v) && 11 == v);
        c.put(2,22);
        NUT_TA(c.get(2,&v) && 22 == v);
        c.put(3,33);
        NUT_TA(c.get(3,&v) && 33 == v);
        NUT_TA(!c.get(1,&v) && 11 != v);

        c.remove(3);
        v = -1;
        NUT_TA(!c.get(3,&v) && -1 == v);

        c.clear();
        NUT_TA(!c.get(2,&v) && -1 == v);
    }

};

NUT_REGISTER_FIXTURE(TestLRUCache, "container, quiet")
