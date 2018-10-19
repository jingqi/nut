
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/platform/portable_endian.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestEndian)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_little_endian)
    NUT_CASE(test_big_endian)
    NUT_CASE(test_bswap)
    NUT_CASES_END()

    void test_little_endian()
    {
        uint32_t ui = 0x12345678;
        NUT_TA(ui == le32toh(htole32(ui)));

        int32_t ii = 0xfedcba87;
        NUT_TA(ii == htole32(le32toh(ii)));
    }

    void test_big_endian()
    {
        uint32_t ui = 0x12345678;
        NUT_TA(ui == be32toh(htobe32(ui)));

        int32_t ii = 0xfedcba87;
        NUT_TA(ii == htobe32(be32toh(ii)));
    }

    void test_bswap()
    {
        NUT_TA(bswap_uint16(0xfedc) == 0xdcfeU);
        NUT_TA(bswap_uint32(0xfedcba87) == 0x87badcfeU);
        NUT_TA(bswap_uint64(0xfedcba9876543210LL) == 0x1032547698badcfeULL);

        NUT_TA(bswap_int16(0xfedc) == (int16_t) 0xdcfe);
        NUT_TA(bswap_int32(0xfedcba87) == (int32_t) 0x87badcfe);
        NUT_TA(bswap_int64(0xfedcba9876543210LL) == (int64_t) 0x1032547698badcfeLL);
    }
};

NUT_REGISTER_FIXTURE(TestEndian, "platform,quiet")
