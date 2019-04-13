
#include <iostream>
#include <nut/unittest/unittest.h>
#include <nut/numeric/word_array_integer.h>


using namespace std;
using namespace nut;

class TestWordArrayInteger : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_bit_count);
        NUT_REGISTER_CASE(test_reverse_bits);
        NUT_REGISTER_CASE(test_lowest_bit);
        NUT_REGISTER_CASE(test_highest_bit);
        NUT_REGISTER_CASE(test_bug1);
    }

    void test_bit_count()
    {
        uint8_t a = 0x4B; // 0100 1011
        NUT_TA(bit1_count(a) == 4);

        uint16_t b = 0x9C4B; // 1001 1100 0100 1011
        NUT_TA(bit1_count(b) == 8);

        uint32_t c = 0x12345678; // 0001 0010 0011 0100 0101 0110 0111 1000
        NUT_TA(bit1_count(c) == 13);

        uint64_t d = 0x123456789ABCDEF0LL; // 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 1101 1110 1111 0000
        NUT_TA(bit1_count(d) == 32);
    }

    void test_reverse_bits()
    {
        uint8_t a = 0x4B; // 0100 1011
        NUT_TA(reverse_bits(a) == 0xD2); // 1101 0010

        uint16_t b = 0x9C4B; // 1001 1100 0100 1011
        NUT_TA(reverse_bits(b) == 0xD239); // 1101 0010 0011 1001

        uint32_t c = 0x12345678; // 0001 0010 0011 0100 0101 0110 0111 1000
        NUT_TA(reverse_bits(c) == 0x1E6A2C48); // 0001 1110 0110 1010 0010 1100 0100 1000

        uint64_t d = 0x123456789ABCDEF0LL; // 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 1101 1110 1111 0000
        NUT_TA(reverse_bits(d) == 0x0F7B3D591E6A2C48LL); // 0000 1111 0111 1011 0011 1101 0101 1001 0001 1110 0110 1010 0010 1100 0100 1000
    }

    void test_lowest_bit()
    {
        NUT_TA(lowest_bit1((unsigned) 0) == -1);

        NUT_TA(lowest_bit1((uint8_t)1) == 0);

        uint8_t a = 0x4B; // 0100 1011
        NUT_TA(lowest_bit1(a) == 0);

        uint16_t b = 0x9C4B; // 1001 1100 0100 1011
        NUT_TA(lowest_bit1(b) == 0);

        uint32_t c = 0x12345678; // 0001 0010 0011 0100 0101 0110 0111 1000
        NUT_TA(lowest_bit1(c) == 3);

        uint64_t d = 0x123456789ABCDEF0LL; // 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 1101 1110 1111 0000
        NUT_TA(lowest_bit1(d) == 4);
    }

    void test_highest_bit()
    {
        NUT_TA(highest_bit1((unsigned) 0) == -1);

        NUT_TA(highest_bit1((uint8_t)1) == 0);

        uint8_t a = 0x4B; // 0100 1011
        NUT_TA(highest_bit1(a) == 6);

        uint16_t b = 0x9C4B; // 1001 1100 0100 1011
        NUT_TA(highest_bit1(b) == 15);

        uint32_t c = 0x12345678; // 0001 0010 0011 0100 0101 0110 0111 1000
        NUT_TA(highest_bit1(c) == 28);

        uint64_t d = 0x123456789ABCDEF0LL; // 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 1101 1110 1111 0000
        NUT_TA(highest_bit1(d) == 60);
    }

    void test_bug1()
    {
        /*
         * 代码中下述逻辑判断是错误的，从而导致bug:
         *     uint8_t a = 0x80;
         *     if ((a << 4) == 0)
         *     {...}
         * 因为左移操作会先自动扩展类型为 int, 操作结果也是 int.
         * 故此正确逻辑是需要截断:
         *     if ((uint8_t)(a << 4) == 0)
         *     {...}
         */
        uint8_t a = 0x80; // 1000 0000
        NUT_TA(lowest_bit1(a) == 7);
    }
};

NUT_REGISTER_FIXTURE(TestWordArrayInteger, "numeric,quiet")
