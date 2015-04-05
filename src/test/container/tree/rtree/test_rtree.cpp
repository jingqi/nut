
#include <iostream>
#include <stdlib.h>
#include <algorithm>

#include <nut/unittest/unittest.hpp>

#include <nut/container/tree/rtree/rtree.hpp>

using namespace nut;

typedef RTree<int, int>::area_type Area;

NUT_FIXTURE(TestRTree)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASE(test_random)
    NUT_CASE(test_simple)
    NUT_CASES_END()

    void set_up()
    {}

    void tear_down()
    {}

    static Area mkrect(int x1, int x2, int y1, int y2)
    {
        Area ret;
        ret.lower[0] = x1;
        ret.higher[0] = x2;
        ret.lower[1] = y1;
        ret.higher[1] = y2;
        return ret;
    }

    void test_smoking()
    {
        RTree<int, int> t;
        t.insert(mkrect(1,2,3,4), 1);
        NUT_TA(t.size() == 1);
        NUT_TA(t.is_valid());

        t.insert(mkrect(3,7,2,7), 2);
        NUT_TA(t.size() == 2);
        NUT_TA(t.is_valid());

        NUT_TA(t.remove_first(mkrect(1,2,3,4)));
        NUT_TA(t.size() == 1);
        NUT_TA(t.is_valid());
    }

    void test_random()
    {
        RTree<int, int> t;
        for (int i = 0; i < 2000; ++i)
        {
            int x1 = rand() % 0xffff, x2 = rand() % 0xffff;
            int y1 = rand() % 0xffff, y2 = rand() % 0xffff;
            Area a = mkrect(std::min(x1, x2), std::max(x1,x2), std::min(y1,y2), std::max(y1,y2));
            t.insert(a, i);

            NUT_TA(t.is_valid());
        }
    }

    void test_simple()
    {
        RTree<int,int> t;
        for (int i = 0; i < 50; ++i)
        {
            Area a = mkrect(i, i + 2, 0, 3);
            t.insert(a, i);
            NUT_TA(t.is_valid());
        }
        NUT_TA(t.size() == 50);
        for (int i = 49; i >= 0; --i)
        {
            Area a = mkrect(i, i + 2, 0, 3);
            NUT_TA(t.remove(a, i));
            NUT_TA(t.is_valid());
        }
        NUT_TA(t.size() == 0 && t.height() == 1);
    }
};

NUT_REGISTER_FIXTURE(TestRTree, "container, tree, rtree, quiet")
