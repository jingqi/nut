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
    NUT_CASES_END()

    void test_smoking()
    {
        BitStream bs("1010100");
        std::string s;
        bs.to_string(&s);
        NUT_TA(s == "1010100");
        bs.append(false);
        bs.append(true);
        s.clear();
        bs.to_string(&s);
        NUT_TA(s == "101010001");

        NUT_TA(bs.bit_at(2) && ! bs.bit_at(5));

        bs.append(bs);
        s.clear();
        bs.to_string(&s);
        NUT_TA(s == "101010001101010001");

        NUT_TA(bs.bit1_count() == 8 && bs.bit0_count() == 10);
    }

    void test_bug1()
    {
        BitStream bs;
        bs.resize(2, true);
        std::string s;
        bs.to_string(&s);
        NUT_TA(s == "11");
    }
};

NUT_REGISTER_FIXTURE(TestBitStream, "container, quiet")
