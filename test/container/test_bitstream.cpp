#include <iostream>

#include <nut/unittest/unittest.hpp>

#include <nut/container/bitstream.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestBitStream)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASES_END()

    void setUp() {}

    void tearDown() {}

    void testSmoking()
    {
        BitStream bs("1010100");
        NUT_TA(bs.toString() == "1010100");
        bs.append(false);
        bs.append(true);
        NUT_TA(bs.toString() == "101010001");

        NUT_TA(bs.bit_at(2) && ! bs.bit_at(5));

        bs.append(bs);
        NUT_TA(bs.toString() == "101010001101010001");

        NUT_TA(bs.bit1_count() == 8 && bs.bit0_count() == 10);
    }
};

NUT_REGISTER_FIXTURE(TestBitStream, "container, quiet")
