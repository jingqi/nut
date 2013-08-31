
#include <nut/unittest/unittest.hpp>

#include <iostream>
#include <nut/container/skiplist/skiplistset.hpp>
#include <nut/container/skiplist/skiplistmap.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestSkipList)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSet)
    NUT_CASE(testMap)
    NUT_CASES_END()

    void setUp() {}
    void tearDown() {}

    void testSet()
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

    void testMap()
    {
        SkipListMap<int, int> sl;
        NUT_TA(sl.size() == 0);

        NUT_TA(sl.add(1, 1));
        NUT_TA(sl.add(4 ,4));
        NUT_TA(sl.add(3, 3));
        NUT_TA(!sl.add(4, 4));
        NUT_TA(sl.size() == 3);
        NUT_TA(sl.containsKey(1) && sl.containsKey(3) && sl.containsKey(4));
        NUT_TA(!sl.remove(2));
        NUT_TA(sl.size() == 3);
        NUT_TA(sl.remove(1));
        NUT_TA(sl.size() == 2);
        NUT_TA(!sl.containsKey(1));
    }
};

NUT_REGISTER_FIXTURE(TestSkipList, "container, skiplist, quiet")
