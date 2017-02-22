
#include <nut/unittest/unit_test.h>

#include <nut/container/bytestream/byte_array_stream.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestByteArrayStream)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_little_endian)
    NUT_CASE(test_big_endian)
    NUT_CASE(test_operators)
    NUT_CASE(test_bug1)
    NUT_CASES_END()

    void test_little_endian()
    {
        rc_ptr<ByteArrayStream> bas = rc_new<ByteArrayStream>();
        uint8_t arr[3] = {1, 2, 3};
        uint8_t arr_check[3] = {0, 0, 0};

        bas->set_little_endian(true);
        NUT_TA(bas->size() == 0 && bas->tell() == 0);
        bas->write_uint8(0x12);
        bas->write_uint16(0x1234);
        bas->write_uint32(0x12345678);
        bas->write_uint64(0x1234567890123456L);
        bas->write_float((float) 1.234);
        bas->write_double(7.8912);
        bas->write(arr, 3);
        NUT_TA(bas->size() == sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t) +
               sizeof(uint64_t) + sizeof(float) + sizeof(double) + 3);
        NUT_TA(bas->size() == bas->tell());

        bas->seek(0);
        NUT_TA(bas->tell() == 0);
        NUT_TA(0x12 == bas->read_uint8());
        NUT_TA(0x1234 == bas->read_uint16());
        NUT_TA(0x12345678 == bas->read_uint32());
        NUT_TA(0x1234567890123456L == bas->read_uint64());
        NUT_TA((float)1.234 == bas->read_float());
        NUT_TA(7.8912 == bas->read_double());
        bas->read(arr_check, 3);
        NUT_TA(::memcmp(arr, arr_check, 3) == 0);
    }

    void test_big_endian()
    {
        rc_ptr<ByteArrayStream> bas = rc_new<ByteArrayStream>();
        uint8_t arr[3] = {1, 2, 3};
        uint8_t arr_check[3] = {0, 0, 0};

        bas->set_little_endian(false);
        NUT_TA(bas->size() == 0 && bas->tell() == 0);
        bas->write_uint8(0x12);
        bas->write_uint16(0x1234);
        bas->write_uint32(0x12345678);
        bas->write_uint64(0x1234567890123456L);
        bas->write_float((float) 1.234);
        bas->write_double(7.8912);
        bas->write(arr, 3);
        NUT_TA(bas->size() == sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t) +
               sizeof(uint64_t) + sizeof(float) + sizeof(double) + 3);
        NUT_TA(bas->size() == bas->tell());

        bas->seek(0);
        NUT_TA(bas->tell() == 0);
        NUT_TA(0x12 == bas->read_uint8());
        NUT_TA(0x1234 == bas->read_uint16());
        NUT_TA(0x12345678 == bas->read_uint32());
        NUT_TA(0x1234567890123456L == bas->read_uint64());
        NUT_TA((float)1.234 == bas->read_float());
        NUT_TA(7.8912 == bas->read_double());
        bas->read(arr_check, 3);
        NUT_TA(::memcmp(arr, arr_check, 3) == 0);
    }

    void test_operators()
    {
        rc_ptr<ByteArrayStream> bas = rc_new<ByteArrayStream>();

        bas->set_little_endian(true);
        NUT_TA(bas->size() == 0 && bas->tell() == 0);
        *bas << (uint8_t) 0x12
            << (uint16_t) 0x1234
            << (uint32_t) 0x12345678
            << (uint64_t) 0x1234567890123456L
            << (float) 1.234
            << (double) 7.8912
            << "abc"
            << L"opq";
        NUT_TA(bas->size() == sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t) +
               sizeof(uint64_t) + sizeof(float) + sizeof(double) +
               sizeof(uint32_t) + sizeof(char) * 3 + sizeof(uint32_t) + sizeof(wchar_t) * 3);
        NUT_TA(bas->size() == bas->tell());

        bas->seek(0);
        NUT_TA(bas->tell() == 0);
        uint8_t v8;
        *bas >> v8;
        NUT_TA(0x12 == v8);

        uint16_t v16;
        *bas >> v16;
        NUT_TA(0x1234 == v16);

        uint32_t v32;
        *bas >> v32;
        NUT_TA(0x12345678 == v32);

        uint64_t v64;
        *bas >> v64;
        NUT_TA(0x1234567890123456L == v64);

        float fv;
        *bas >> fv;
        NUT_TA((float)1.234 == fv);

        double dv;
        *bas >> dv;
        NUT_TA(7.8912 == dv);

        string s;
        *bas >> s;
        NUT_TA(s == "abc");

        wstring ws;
        *bas >> ws;
        NUT_TA(ws == L"opq");

        NUT_TA(bas->readable_size() == 0);
    }

    void test_bug1()
    {
        // 函数 size_t ByteArrayStream::write(const void *buf, size_t cb) 实现的
        // 有问题，可能导致的问题：
        //   1. 内存读取越界
        //   1. 范围迭代器参数 begin,end 不满足 begin <= end 关系, 从而要么内存
        //      分配失败(内存大小从负数回卷成超大正数)或者标准库崩溃
        rc_ptr<ByteArrayStream> bas = rc_new<ByteArrayStream>();
        bas->write_uint64(0);
        bas->write_uint64(0);
        bas->seek(0);
        uint16_t v = 0;
        bas->write(&v, sizeof(v)); // Will crash here if bug exists
    }
};

NUT_REGISTER_FIXTURE(TestByteArrayStream, "container, quiet")
