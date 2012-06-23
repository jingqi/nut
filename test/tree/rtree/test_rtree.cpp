
#include <iostream>
#include <nut/unittest/unittest.hpp>

#include <nut/tree/rtree/rtree.hpp>

using namespace nut;

typedef RTree<int, int>::Area Area;

NUT_FIXTURE(TestRTree)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASES_END()

    void setUp()
    {}

    void tearDown()
    {}

    static Area mkrect(int x1, int x2, int y1, int y2)
    {
        Area ret;
        ret.left[0] = x1;
        ret.right[0] = x2;
        ret.left[1] = y1;
        ret.right[1] = y2;
        return ret;
    }

    void testSmoking()
    {
        RTree<int, int> r;
        r.insert(mkrect(1,2, 100,130), 2);
    }
};

NUT_REGISTER_FIXTURE(TestRTree, "tree")
