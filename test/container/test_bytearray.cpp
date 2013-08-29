
#include <nut/unittest/unittest.hpp>

#include <nut/container/bytearray.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestByteArray)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testToString)
    NUT_CASE(testValueOf)
    NUT_CASE(testCopyOnWrite)
    NUT_CASES_END()

    void setUp() {}

    void tearDown() {}

    void testToString()
    {
        ByteArray ba;
        ba.append(1, 0x13);
        ba.append(1, 0x28);
        ba.append(1, 0xe4);
        NUT_TA(ba.toString() == "1328E4");
    }

    void testValueOf()
    {
        ByteArray ba = ByteArray::valueOf("1328Ef");
        NUT_TA(ba.toString() == "1328EF");
    }

    void testCopyOnWrite()
    {
        ByteArray ba = ByteArray::valueOf("1328Ef");
        ByteArray b1(ba), b2;
        b2 = ba;

        // 还未复制时
        NUT_TA(static_cast<const ByteArray&>(b1).buffer() == static_cast<const ByteArray&>(b2).buffer());
        NUT_TA(b1.toString() == "1328EF" && b2.toString() == "1328EF");

        // 复制后
        b1.append(1, 0x5E);
        NUT_TA(static_cast<const ByteArray&>(b1).buffer() != static_cast<const ByteArray&>(b2).buffer());
        NUT_TA(static_cast<const ByteArray&>(ba).buffer() == static_cast<const ByteArray&>(b2).buffer());
        NUT_TA(b1.toString() == "1328EF5E" && b2.toString() == "1328EF");
    }
};

NUT_REGISTER_FIXTURE(TestByteArray, "util, quiet")
