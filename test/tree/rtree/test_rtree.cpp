
#include <iostream>
#include <nut/unittest/unittest.hpp>

#include <nut/tree/rtree/rtree.hpp>

using namespace nut;


NUT_FIXTURE(TestRTree)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASES_END()

    void setUp()
    {}

    void tearDown()
    {}

    void testSmoking()
    {
        RTree<int, int> r;
    }
};

NUT_REGISTER_FIXTURE(TestRTree, "tree")
