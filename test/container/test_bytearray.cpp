
#include <nut/unittest/unittest.hpp>

#include <nut/container/byte_array.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestByteArray)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_to_string)
    NUT_CASE(test_value_of)
    NUT_CASE(test_copy_on_write)
    NUT_CASE(test_bug1)
    NUT_CASES_END()

    void set_up() {}

    void tear_down() {}

    void test_to_string()
    {
        ByteArray ba;
        ba.append(1, 0x13);
        ba.append(1, 0x28);
        ba.append(1, 0xe4);
        NUT_TA(ba.to_string() == "1328E4");
    }

    void test_value_of()
    {
        ByteArray ba = ByteArray::value_of("1328Ef");
        NUT_TA(ba.to_string() == "1328EF");
    }

    void test_copy_on_write()
    {
        ByteArray ba = ByteArray::value_of("1328Ef");
        ByteArray b1(ba), b2;
        b2 = ba;

        // 还未复制时
        NUT_TA(static_cast<const ByteArray&>(b1).data() == static_cast<const ByteArray&>(b2).data());
        NUT_TA(b1.to_string() == "1328EF" && b2.to_string() == "1328EF");

        // 复制后
        b1.append(1, 0x5E);
        NUT_TA(static_cast<const ByteArray&>(b1).data() != static_cast<const ByteArray&>(b2).data());
        NUT_TA(static_cast<const ByteArray&>(ba).data() == static_cast<const ByteArray&>(b2).data());
        NUT_TA(b1.to_string() == "1328EF5E" && b2.to_string() == "1328EF");
    }

    /**
     * []操作符的实现没有调用 copy-on-write，导致错误
     */
    void test_bug1()
    {
        ByteArray b1 = ByteArray::value_of("125e");
        ByteArray b2(b1);

        NUT_TA(static_cast<const ByteArray&>(b1).data() == static_cast<const ByteArray&>(b2).data());
        b1[0] = 2;
        NUT_TA(static_cast<const ByteArray&>(b1).data() != static_cast<const ByteArray&>(b2).data());
        NUT_TA(b1.to_string() == "025E" && b2.to_string() == "125E");
    }
};

NUT_REGISTER_FIXTURE(TestByteArray, "container, quiet")
