
#include <nut/unittest/unittest.hpp>

#include <nut/container/bytearraystream.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestByteArrayStream)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testLittleEndian)
    NUT_CASE(testBigEndian)
    NUT_CASES_END()

    void setUp() {}

    void tearDown() {}

    void testLittleEndian()
    {
        ByteArrayStream bas;
        uint8_t arr[3] = {1, 2, 3};
        uint8_t arr_check[3] = {0, 0, 0};

        bas.setLittleEndian(true);
        NUT_TA(bas.size() == 0 && bas.tell() == 0);
        bas.writeByte(0x12);
        bas.writeWord(0x1234);
        bas.writeDWord(0x12345678);
        bas.writeQWord(0x1234567890123456L);
        bas.writeFloat((float) 1.234);
        bas.writeDouble(7.8912);
        bas.writeBytes(arr, 3);
        NUT_TA(bas.size() == sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint64_t) + sizeof(float) + sizeof(double) + 3);
        NUT_TA(bas.size() == bas.tell());

        bas.seek(0);
        NUT_TA(bas.tell() == 0);
        NUT_TA(0x12 == bas.readByte());
        NUT_TA(0x1234 == bas.readWord());
        NUT_TA(0x12345678 == bas.readDWord());
        NUT_TA(0x1234567890123456L == bas.readQWord());
        NUT_TA((float)1.234 == bas.readFloat());
        NUT_TA(7.8912 == bas.readDouble());
        bas.readBytes(arr_check, 3);
        NUT_TA(::memcmp(arr, arr_check, 3) == 0);
    }

    void testBigEndian()
    {
        ByteArrayStream bas;
        uint8_t arr[3] = {1, 2, 3};
        uint8_t arr_check[3] = {0, 0, 0};

        bas.setLittleEndian(false);
        NUT_TA(bas.size() == 0 && bas.tell() == 0);
        bas.writeByte(0x12);
        bas.writeWord(0x1234);
        bas.writeDWord(0x12345678);
        bas.writeQWord(0x1234567890123456L);
        bas.writeFloat((float) 1.234);
        bas.writeDouble(7.8912);
        bas.writeBytes(arr, 3);
        NUT_TA(bas.size() == sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint64_t) + sizeof(float) + sizeof(double) + 3);
        NUT_TA(bas.size() == bas.tell());

        bas.seek(0);
        NUT_TA(bas.tell() == 0);
        NUT_TA(0x12 == bas.readByte());
        NUT_TA(0x1234 == bas.readWord());
        NUT_TA(0x12345678 == bas.readDWord());
        NUT_TA(0x1234567890123456L == bas.readQWord());
        NUT_TA((float)1.234 == bas.readFloat());
        NUT_TA(7.8912 == bas.readDouble());
        bas.readBytes(arr_check, 3);
        NUT_TA(::memcmp(arr, arr_check, 3) == 0);
    }

};

NUT_REGISTER_FIXTURE(TestByteArrayStream, "container, quiet")
