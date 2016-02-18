#include <iostream>

#include <nut/unittest/unit_test.h>

#include <nut/container/bit_stream.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestBitStream)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASE(test_bug1)
    NUT_CASE(test_bug2)
    NUT_CASE(test_substream)
    NUT_CASES_END()

    void test_smoking()
    {
        BitStream bs("1010100");
        NUT_TA(bs.to_string() == "1010100");
        bs.append_bit(0);
        bs.append_bit(1);
        NUT_TA(bs.to_string() == "101010001");

        NUT_TA(1 == bs.bit_at(2) && 0 == bs.bit_at(5));

        bs.append(bs);
        NUT_TA(bs.to_string() == "101010001101010001");

        NUT_TA(bs.bit1_count() == 8 && bs.bit0_count() == 10);
    }

    void test_bug1()
    {
        BitStream bs;
        bs.resize(2, 1);
        NUT_TA(bs.to_string() == "11");
    }

    void test_bug2()
    {
        BitStream bs;
        bs.append_bit(0);
        bs.resize(3, 1);
        NUT_TA(bs.to_string() == "011");
    }

    void test_substream()
    {
        BitStream bs("1100");
        NUT_TA(bs.substream(1,2).to_string() == "10");
    }
};

NUT_REGISTER_FIXTURE(TestBitStream, "container, quiet")
