
#include <nut/unittest/unittest.hpp>

#include <nut/util/bytearray.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestByteArray)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testToString)
    NUT_CASE(testValueOf)
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
};

NUT_REGISTER_FIXTURE(TestByteArray, "util, quiet")
