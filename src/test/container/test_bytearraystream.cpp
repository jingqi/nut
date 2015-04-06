
#include <nut/unittest/unittest.h>

#include <nut/container/byte_array_stream.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestByteArrayStream)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_little_endian)
    NUT_CASE(test_big_endian)
    NUT_CASES_END()

    void set_up() {}

    void tear_down() {}

    void test_little_endian()
    {
        ByteArrayStream bas;
        uint8_t arr[3] = {1, 2, 3};
        uint8_t arr_check[3] = {0, 0, 0};

        bas.set_little_endian(true);
        NUT_TA(bas.size() == 0 && bas.tell() == 0);
        bas.write_uint8(0x12);
        bas.write_uint16(0x1234);
        bas.write_uint32(0x12345678);
        bas.write_uint64(0x1234567890123456L);
        bas.write_float((float) 1.234);
        bas.write_double(7.8912);
        bas.write_bytes(arr, 3);
        NUT_TA(bas.size() == sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint64_t) + sizeof(float) + sizeof(double) + 3);
        NUT_TA(bas.size() == bas.tell());

        bas.seek(0);
        NUT_TA(bas.tell() == 0);
        NUT_TA(0x12 == bas.read_uint8());
        NUT_TA(0x1234 == bas.read_uint16());
        NUT_TA(0x12345678 == bas.read_uint32());
        NUT_TA(0x1234567890123456L == bas.read_uint64());
        NUT_TA((float)1.234 == bas.read_float());
        NUT_TA(7.8912 == bas.read_double());
        bas.read_bytes(arr_check, 3);
        NUT_TA(::memcmp(arr, arr_check, 3) == 0);
    }

    void test_big_endian()
    {
        ByteArrayStream bas;
        uint8_t arr[3] = {1, 2, 3};
        uint8_t arr_check[3] = {0, 0, 0};

        bas.set_little_endian(false);
        NUT_TA(bas.size() == 0 && bas.tell() == 0);
        bas.write_uint8(0x12);
        bas.write_uint16(0x1234);
        bas.write_uint32(0x12345678);
        bas.write_uint64(0x1234567890123456L);
        bas.write_float((float) 1.234);
        bas.write_double(7.8912);
        bas.write_bytes(arr, 3);
        NUT_TA(bas.size() == sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint64_t) + sizeof(float) + sizeof(double) + 3);
        NUT_TA(bas.size() == bas.tell());

        bas.seek(0);
        NUT_TA(bas.tell() == 0);
        NUT_TA(0x12 == bas.read_uint8());
        NUT_TA(0x1234 == bas.read_uint16());
        NUT_TA(0x12345678 == bas.read_uint32());
        NUT_TA(0x1234567890123456L == bas.read_uint64());
        NUT_TA((float)1.234 == bas.read_float());
        NUT_TA(7.8912 == bas.read_double());
        bas.read_bytes(arr_check, 3);
        NUT_TA(::memcmp(arr, arr_check, 3) == 0);
    }

};

NUT_REGISTER_FIXTURE(TestByteArrayStream, "container, quiet")
