
#include <iostream>
#include <nut/unittest/unittest.hpp>
#include <nut/numeric/biginteger.hpp>

using namespace nut;

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(disable: 4307)
#endif

NUT_FIXTURE(TestBigInteger)
{
	NUT_CASES_BEGIN()
	NUT_CASE(testSmoking)
	NUT_CASE(testComparator)
	NUT_CASE(testMathOperator)
	NUT_CASE(testDivide)
	NUT_CASE(testMod)
    NUT_CASE(testBitOperator)
    NUT_CASE(testToString)
    NUT_CASE(testValueOf)
	NUT_CASES_END()

	void setUp() {}
	void tearDown() {}

	void testSmoking()
	{
		// +
		NUT_TA((BigInteger(12) + BigInteger(13)).long_value() == 12 + 13);
		NUT_TA((BigInteger(12) + BigInteger(-11)).long_value() == 12 - 11);
		NUT_TA((BigInteger(12) + BigInteger(-13)).long_value() == 12 - 13);

		// -
		NUT_TA((BigInteger(12) - BigInteger(1)).long_value() == 12 - 1);
		NUT_TA((BigInteger(12) - BigInteger(-1)).long_value() == 12 - -1);
		NUT_TA((BigInteger(12) - BigInteger(13)).long_value() == 12 - 13);

		// *
		NUT_TA((BigInteger(12) * BigInteger(1)).long_value() == 12 * 1);
		NUT_TA((BigInteger(12) * BigInteger(24)).long_value() == 12 * 24);
		NUT_TA(((BigInteger(0x12345L) * BigInteger(0x12345L)).long_value() & 0xFFFFFFL) == 
			((0x12345L * 0x12345L) & 0xFFFFFFL));

		// /
		NUT_TA(BigInteger(3) / BigInteger(4) == BigInteger(3/4));
		NUT_TA(BigInteger(4) / BigInteger(3) == BigInteger(4/3));

		// %
		NUT_TA(BigInteger(3) % BigInteger(4) == BigInteger(3%4));
		NUT_TA(BigInteger(4) % BigInteger(3) == BigInteger(4%3));
	}

	void testComparator()
	{
		NUT_TA(BigInteger(2) < BigInteger(5));
		NUT_TA(BigInteger(9) > BigInteger(-5));
		NUT_TA(BigInteger(9) > BigInteger(5));
		NUT_TA(!(BigInteger(2) < BigInteger(2)));
	}

	void testMathOperator()
	{
		// +
		NUT_TA(BigInteger(2) + BigInteger(-3) == BigInteger(-1));

		// -
		NUT_TA(BigInteger(2) - BigInteger(-3) == BigInteger(5));

		// *
		NUT_TA(BigInteger(2) * BigInteger(-3) == BigInteger(-6));
	}

	// ²âÊÔ³ý·¨
	void testDivide()
	{
		NUT_TA(BigInteger(4) / BigInteger(3) == BigInteger(4/3));
		NUT_TA(BigInteger(3) / BigInteger(4) == BigInteger(3/4));
		NUT_TA(BigInteger(4) / BigInteger(4) == BigInteger(4/4));

		NUT_TA(BigInteger(4) / BigInteger(-3) == BigInteger(4/-3));
		NUT_TA(BigInteger(3) / BigInteger(-4) == BigInteger(3/-4));
		NUT_TA(BigInteger(4) / BigInteger(-4) == BigInteger(4/-4));

		NUT_TA(BigInteger(-4) / BigInteger(3) == BigInteger((-4)/3));
		NUT_TA(BigInteger(-3) / BigInteger(4) == BigInteger((-3)/4));
		NUT_TA(BigInteger(-4) / BigInteger(4) == BigInteger((-4)/4));

		NUT_TA(BigInteger(-4) / BigInteger(-3) == BigInteger((-4)/-3));
		NUT_TA(BigInteger(-3) / BigInteger(-4) == BigInteger((-3)/-4));
		NUT_TA(BigInteger(-4) / BigInteger(-4) == BigInteger((-4)/-4));
	}

	// ²âÊÔÈ¡ÓàÊý
	void testMod()
	{
		NUT_TA(BigInteger(4) % BigInteger(3) == BigInteger(4%3));
		NUT_TA(BigInteger(3) % BigInteger(4) == BigInteger(3%4));
		NUT_TA(BigInteger(4) % BigInteger(4) == BigInteger(4%4));

		NUT_TA(BigInteger(4) % BigInteger(-3) == BigInteger(4%-3));
		NUT_TA(BigInteger(3) % BigInteger(-4) == BigInteger(3%-4));
		NUT_TA(BigInteger(4) % BigInteger(-4) == BigInteger(4%-4));

		NUT_TA(BigInteger(-4) % BigInteger(3) == BigInteger((-4)%3));
		NUT_TA(BigInteger(-3) % BigInteger(4) == BigInteger((-3)%4));
		NUT_TA(BigInteger(-4) % BigInteger(4) == BigInteger((-4)%4));

		NUT_TA(BigInteger(-4) % BigInteger(-3) == BigInteger((-4)%-3));
		NUT_TA(BigInteger(-3) % BigInteger(-4) == BigInteger((-3)%-4));
		NUT_TA(BigInteger(-4) % BigInteger(-4) == BigInteger((-4)%-4));
	}

	void testBitOperator()
	{
		NUT_TA((BigInteger(5) << 2) == BigInteger(5 << 2));
		NUT_TA((BigInteger(5) >> 1) == BigInteger(5 >> 1));
	}

    void testToString()
    {
        NUT_TA(BigInteger(0).toString() == "0");
        NUT_TA(BigInteger(10).toString() == "10");
        NUT_TA(BigInteger(-15).toString() == "-15");

        NUT_TA(BigInteger(0x4A).toString(0x10) == "4A");

        NUT_TA(BigInteger(13).toString(8) == "15");

        NUT_TA(BigInteger(5).toString(2) == "101");
    }

    void testValueOf()
    {
        NUT_TA(BigInteger::valueOf("0").long_value() == 0);
        NUT_TA(BigInteger::valueOf("10").long_value() == 10);
        NUT_TA(BigInteger::valueOf("-14").long_value() == -14);

        NUT_TA(BigInteger::valueOf("4a", 0x10).long_value() == 0x4a);

        NUT_TA(BigInteger::valueOf("15", 8).long_value() == 13);

        NUT_TA(BigInteger::valueOf("101",2).long_value() == 5);
    }
};

NUT_REGISTER_FIXTURE(TestBigInteger, "numeric,quiet")
