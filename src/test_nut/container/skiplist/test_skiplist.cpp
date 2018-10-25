
#include <nut/unittest/unittest.h>

#include <iostream>
#include <nut/container/skiplist/skiplist_set.h>
#include <nut/container/skiplist/skiplist_map.h>

using namespace std;
using namespace nut;

class TestSkipList : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_set);
        NUT_REGISTER_CASE(test_map);
    }

    void test_set()
    {
        SkipListSet<int> sl;
        NUT_TA(sl.size() == 0);

        NUT_TA(sl.add(1));
        NUT_TA(sl.add(4));
        NUT_TA(sl.add(3));
        NUT_TA(!sl.add(4));
        NUT_TA(sl.size() == 3);
        NUT_TA(sl.contains(1));
        NUT_TA(sl.contains(3));
        NUT_TA(sl.contains(4));
        NUT_TA(sl.contains(1) && sl.contains(3) && sl.contains(4));
        NUT_TA(!sl.remove(2));
        NUT_TA(sl.size() == 3);
        NUT_TA(sl.remove(1));
        NUT_TA(sl.size() == 2);
        NUT_TA(!sl.contains(1));
    }

    void test_map()
    {
        SkipListMap<int, int> sl;
        NUT_TA(sl.size() == 0);

        NUT_TA(sl.put(1, 1));
        NUT_TA(sl.put(4 ,4));
        NUT_TA(sl.put(3, 3));
        NUT_TA(!sl.put(4, 4));
        NUT_TA(sl.size() == 3);
        NUT_TA(sl.contains_key(1) && sl.contains_key(3) && sl.contains_key(4));
        NUT_TA(!sl.remove(2));
        NUT_TA(sl.size() == 3);
        NUT_TA(sl.remove(1));
        NUT_TA(sl.size() == 2);
        NUT_TA(!sl.contains_key(1));
    }
};

NUT_REGISTER_FIXTURE(TestSkipList, "container, skiplist, quiet")
