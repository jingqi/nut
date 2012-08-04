
#include <iostream>
#include <nut/unittest/unittest.hpp>
#include <nut/numeric/signedinteger.hpp>

using namespace nut;

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(disable: 4307)
#endif

NUT_FIXTURE(TestSignedInteger)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASE(testComparator)
	NUT_CASE(testMathOperator)
	NUT_CASE(testDivide)
	NUT_CASE(testMod)
	NUT_CASE(testBitOperator)
    NUT_CASES_END()

    void setUp() {}
    void tearDown() {}

    void testSmoking()
    {
        // +
        NUT_TA((SignedInteger<3>(12) + SignedInteger<3>(13)).long_value() == 12 + 13);
        NUT_TA((SignedInteger<3>(12) + SignedInteger<3>(-11)).long_value() == 12 - 11);
        NUT_TA((SignedInteger<3>(12) + SignedInteger<3>(-13)).long_value() == 12 - 13);

        // -
        NUT_TA((SignedInteger<3>(12) - SignedInteger<3>(1)).long_value() == 12 - 1);
        NUT_TA((SignedInteger<3>(12) - SignedInteger<3>(-1)).long_value() == 12 - -1);
        NUT_TA((SignedInteger<3>(12) - SignedInteger<3>(13)).long_value() == 12 - 13);

        // *
        NUT_TA((SignedInteger<3>(12) * SignedInteger<3>(1)).long_value() == 12 * 1);
        NUT_TA((SignedInteger<13>(12) * SignedInteger<13>(24)).long_value() == 12 * 24);
        NUT_TA(((SignedInteger<13>(0x12345L) * SignedInteger<13>(0x12345L)).long_value() & 0xFFFFFFL) == 
            ((0x12345L * 0x12345L) & 0xFFFFFFL));

		// /
		NUT_TA(SignedInteger<3>(3) / SignedInteger<3>(4) == SignedInteger<3>(3/4));
		NUT_TA(SignedInteger<3>(4) / SignedInteger<3>(3) == SignedInteger<3>(4/3));

		// %
		NUT_TA(SignedInteger<3>(3) % SignedInteger<3>(4) == SignedInteger<3>(3%4));
		NUT_TA(SignedInteger<3>(4) % SignedInteger<3>(3) == SignedInteger<3>(4%3));
    }

    void testComparator()
    {
        NUT_TA(SignedInteger<3>(2) < SignedInteger<3>(5));
        NUT_TA(SignedInteger<3>(9) > SignedInteger<3>(-5));
        NUT_TA(SignedInteger<3>(9) > SignedInteger<3>(5));
        NUT_TA(!(SignedInteger<3>(2) < SignedInteger<3>(2)));
    }

	void testMathOperator()
	{
		// +
		NUT_TA(SignedInteger<3>(2) + SignedInteger<3>(-3) == SignedInteger<3>(-1));

		// -
		NUT_TA(SignedInteger<3>(2) - SignedInteger<3>(-3) == SignedInteger<3>(5));

		// *
		NUT_TA(SignedInteger<3>(2) * SignedInteger<3>(-3) == SignedInteger<3>(-6));
	}

	// ≤‚ ‘≥˝∑®
	void testDivide()
	{
		NUT_TA(SignedInteger<3>(4) / SignedInteger<3>(3) == SignedInteger<3>(4/3));
		NUT_TA(SignedInteger<3>(3) / SignedInteger<3>(4) == SignedInteger<3>(3/4));
		NUT_TA(SignedInteger<3>(4) / SignedInteger<3>(4) == SignedInteger<3>(4/4));

		NUT_TA(SignedInteger<3>(4) / SignedInteger<3>(-3) == SignedInteger<3>(4/-3));
		NUT_TA(SignedInteger<3>(3) / SignedInteger<3>(-4) == SignedInteger<3>(3/-4));
		NUT_TA(SignedInteger<3>(4) / SignedInteger<3>(-4) == SignedInteger<3>(4/-4));

		NUT_TA(SignedInteger<3>(-4) / SignedInteger<3>(3) == SignedInteger<3>((-4)/3));
		NUT_TA(SignedInteger<3>(-3) / SignedInteger<3>(4) == SignedInteger<3>((-3)/4));
		NUT_TA(SignedInteger<3>(-4) / SignedInteger<3>(4) == SignedInteger<3>((-4)/4));

		NUT_TA(SignedInteger<3>(-4) / SignedInteger<3>(-3) == SignedInteger<3>((-4)/-3));
		NUT_TA(SignedInteger<3>(-3) / SignedInteger<3>(-4) == SignedInteger<3>((-3)/-4));
		NUT_TA(SignedInteger<3>(-4) / SignedInteger<3>(-4) == SignedInteger<3>((-4)/-4));
	}

	// ≤‚ ‘»°”‡ ˝
	void testMod()
	{
		NUT_TA(SignedInteger<3>(4) % SignedInteger<3>(3) == SignedInteger<3>(4%3));
		NUT_TA(SignedInteger<3>(3) % SignedInteger<3>(4) == SignedInteger<3>(3%4));
		NUT_TA(SignedInteger<3>(4) % SignedInteger<3>(4) == SignedInteger<3>(4%4));

		NUT_TA(SignedInteger<3>(4) % SignedInteger<3>(-3) == SignedInteger<3>(4%-3));
		NUT_TA(SignedInteger<3>(3) % SignedInteger<3>(-4) == SignedInteger<3>(3%-4));
		NUT_TA(SignedInteger<3>(4) % SignedInteger<3>(-4) == SignedInteger<3>(4%-4));

		NUT_TA(SignedInteger<3>(-4) % SignedInteger<3>(3) == SignedInteger<3>((-4)%3));
		NUT_TA(SignedInteger<3>(-3) % SignedInteger<3>(4) == SignedInteger<3>((-3)%4));
		NUT_TA(SignedInteger<3>(-4) % SignedInteger<3>(4) == SignedInteger<3>((-4)%4));

		NUT_TA(SignedInteger<3>(-4) % SignedInteger<3>(-3) == SignedInteger<3>((-4)%-3));
		NUT_TA(SignedInteger<3>(-3) % SignedInteger<3>(-4) == SignedInteger<3>((-3)%-4));
		NUT_TA(SignedInteger<3>(-4) % SignedInteger<3>(-4) == SignedInteger<3>((-4)%-4));
	}

	void testBitOperator()
	{
		NUT_TA((SignedInteger<3>(5) | SignedInteger<3>(3)) == SignedInteger<3>(5|3));
		NUT_TA((SignedInteger<3>(5) & SignedInteger<3>(3)) == SignedInteger<3>(5&3));
		NUT_TA((SignedInteger<3>(5) ^ SignedInteger<3>(3)) == SignedInteger<3>(5^3));

		NUT_TA((SignedInteger<3>(5) << 2) == SignedInteger<3>(5 << 2));
		NUT_TA((SignedInteger<3>(5) >> 1) == SignedInteger<3>(5 >> 1));
	}
};

NUT_REGISTER_FIXTURE(TestSignedInteger, "numeric,quiet")
