
#include <iostream>
#include <nut/unittest/unittest.hpp>

#include <nut/tree/rtree/rtree.hpp>

using namespace nut;

typedef RTree<int, int>::Area Area;

NUT_FIXTURE(TestRTree)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASE(testRandom)
    NUT_CASES_END()

    void setUp()
    {}

    void tearDown()
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

    void testSmoking()
    {
        RTree<int, int> t;
        t.insert(mkrect(1,2,3,4), 1);
        NUT_TA(t.size() == 1);
        NUT_TA(t.isValid());

        t.insert(mkrect(3,7,2,7), 2);
        NUT_TA(t.size() == 2);
        NUT_TA(t.isValid());

        NUT_TA(t.removeFirst(mkrect(1,2,3,4)));
        NUT_TA(t.size() == 1);
        NUT_TA(t.isValid());
    }

    void testRandom()
    {
        RTree<int, int> t;
        for (int i = 0; i < 2000; ++i)
        {
            int x1 = rand() % 0xffff, x2 = rand() % 0xffff;
            int y1 = rand() % 0xffff, y2 = rand() % 0xffff;
            Area a = mkrect(std::min(x1, x2), std::max(x1,x2), std::min(y1,y2), std::max(y1,y2));
            t.insert(a, i);

            NUT_TA(t.isValid());
        }
    }
};

NUT_REGISTER_FIXTURE(TestRTree, "tree")
