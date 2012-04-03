
#include <iostream>
#include <nut/unittest/unittest.hpp>
#include <nut/numeric/unsignedinteger.hpp>

using namespace nut;


NUT_FIXTURE(TestUnsignedInteger)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASE(testComparator)
	NUT_CASE(testMathOperator)
	NUT_CASE(testBitOperator)
    NUT_CASES_END()

    void setUp() {}
    void tearDown() {}

    void testSmoking()
    {
        // +
        NUT_TA((UnsignedInteger<3>(12) + UnsignedInteger<3>(13)).ulong_value() == 12 + 13);

        // -
        NUT_TA((UnsignedInteger<3>(12) - UnsignedInteger<3>(1)).ulong_value() == 12 - 1);

        // *
        NUT_TA((UnsignedInteger<3>(12) * UnsignedInteger<3>(1)).ulong_value() == 12 * 1);
        NUT_TA((UnsignedInteger<13>(12) * UnsignedInteger<13>(24)).ulong_value() == 12 * 24);
        NUT_TA(((UnsignedInteger<13>(0x12345L) * UnsignedInteger<13>(0x12345L)).ulong_value() & 0xFFFFFFL) == 
            ((0x12345L * 0x12345L) & 0xFFFFFFL));

		// /
		NUT_TA(UnsignedInteger<3>(3) / UnsignedInteger<3>(4) == UnsignedInteger<3>(3/4));
		NUT_TA(UnsignedInteger<3>(4) / UnsignedInteger<3>(3) == UnsignedInteger<3>(4/3));

		// %
		NUT_TA(UnsignedInteger<3>(3) % UnsignedInteger<3>(4) == UnsignedInteger<3>(3%4));
		NUT_TA(UnsignedInteger<3>(4) % UnsignedInteger<3>(3) == UnsignedInteger<3>(4%3));
    }

    void testComparator()
    {
        NUT_TA(UnsignedInteger<3>(2) < UnsignedInteger<3>(5));
        NUT_TA(UnsignedInteger<3>(9) > UnsignedInteger<3>(5));
        NUT_TA(!(UnsignedInteger<3>(2) < UnsignedInteger<3>(2)));
    }

	void testMathOperator()
	{
		NUT_TA(UnsignedInteger<3>(2) + UnsignedInteger<3>(3) == UnsignedInteger<3>(2 + 3));

		NUT_TA(UnsignedInteger<3>(3) - UnsignedInteger<3>(2) == UnsignedInteger<3>(3 - 2));

		NUT_TA(UnsignedInteger<3>(2) * UnsignedInteger<3>(3) == UnsignedInteger<3>(2 * 3));

		NUT_TA(UnsignedInteger<3>(4) / UnsignedInteger<3>(3) == UnsignedInteger<3>(4/3));
		NUT_TA(UnsignedInteger<3>(3) / UnsignedInteger<3>(4) == UnsignedInteger<3>(3/4));
		NUT_TA(UnsignedInteger<3>(4) / UnsignedInteger<3>(4) == UnsignedInteger<3>(4/4));

		NUT_TA(UnsignedInteger<3>(4) % UnsignedInteger<3>(3) == UnsignedInteger<3>(4%3));
		NUT_TA(UnsignedInteger<3>(3) % UnsignedInteger<3>(4) == UnsignedInteger<3>(3%4));
		NUT_TA(UnsignedInteger<3>(4) % UnsignedInteger<3>(4) == UnsignedInteger<3>(4%4));
	}

	void testBitOperator()
	{
		NUT_TA((UnsignedInteger<3>(5) | UnsignedInteger<3>(3)) == UnsignedInteger<3>(5|3));
		NUT_TA((UnsignedInteger<3>(5) & UnsignedInteger<3>(3)) == UnsignedInteger<3>(5&3));
		NUT_TA((UnsignedInteger<3>(5) ^ UnsignedInteger<3>(3)) == UnsignedInteger<3>(5^3));

		NUT_TA((UnsignedInteger<3>(5) << 2) == UnsignedInteger<3>(5 << 2));
		NUT_TA((UnsignedInteger<3>(5) >> 1) == UnsignedInteger<3>(5 >> 1));
	}
};

NUT_REGISTER_FIXTURE(TestUnsignedInteger, "numeric,quiet")
