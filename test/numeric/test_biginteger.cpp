
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
    NUT_CASE(testBugs)
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
		NUT_TA((BigInteger(12) + BigInteger(13)).llong_value() == 12 + 13);
		NUT_TA((BigInteger(12) + BigInteger(-11)).llong_value() == 12 - 11);
		NUT_TA((BigInteger(12) + BigInteger(-13)).llong_value() == 12 - 13);

		// -
		NUT_TA((BigInteger(12) - BigInteger(1)).llong_value() == 12 - 1);
		NUT_TA((BigInteger(12) - BigInteger(-1)).llong_value() == 12 - -1);
		NUT_TA((BigInteger(12) - BigInteger(13)).llong_value() == 12 - 13);

		// *
		NUT_TA((BigInteger(12) * BigInteger(1)).llong_value() == 12 * 1);
		NUT_TA((BigInteger(12) * BigInteger(24)).llong_value() == 12 * 24);
		NUT_TA(((BigInteger(0x12345L) * BigInteger(0x12345L)).llong_value() & 0xFFFFFFL) == 
			((0x12345L * 0x12345L) & 0xFFFFFFL));

		// /
		NUT_TA(BigInteger(3) / BigInteger(4) == BigInteger(3/4));
		NUT_TA(BigInteger(4) / BigInteger(3) == BigInteger(4/3));

		// %
		NUT_TA(BigInteger(3) % BigInteger(4) == BigInteger(3%4));
        NUT_TA(BigInteger(4) % BigInteger(3) == BigInteger(4%3));
	}

    void testBugs()
    {
        // bug 无符号除数可能被当成有符号负数
        NUT_TA(BigInteger(999) / BigInteger(131) == BigInteger(999/131));
        NUT_TA(BigInteger(7) / BigInteger(128) == BigInteger(7/128));
        NUT_TA(BigInteger(128) / BigInteger(7) == BigInteger(128/7));
        NUT_TA(BigInteger(7) % BigInteger(128) == BigInteger(7%128));
        NUT_TA(BigInteger(128) % BigInteger(7) == BigInteger(128%7));

        // bug 由于操作数和计算结果共享内存，导致计算bug
        BigInteger a = BigInteger::valueOf("-985809295"), b = BigInteger::valueOf("6369612912");
        BigInteger c(a);
        c -= b;
        NUT_TA(c == a - b);
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
        NUT_TA(BigInteger(2) + BigInteger(3) == BigInteger(2 + 3));
		NUT_TA(BigInteger(2) + BigInteger(-3) == BigInteger(2 + -3));

		// -
        NUT_TA(BigInteger(3) - BigInteger(2) == BigInteger(3 - 2));
		NUT_TA(BigInteger(2) - BigInteger(-3) == BigInteger(2 - -3));

		// *
        NUT_TA(BigInteger(2) * BigInteger(3) == BigInteger(2 * 3));
		NUT_TA(BigInteger(2) * BigInteger(-3) == BigInteger(2 * -3));

        // 乘方
        BigInteger a;
        a.resize(4);
        a.buffer()[0] = 45;
        a.buffer()[1] = 0x10000;
        a.buffer()[2] = 5;
        a.buffer()[3] = 68;
        NUT_TA(a * a == a * BigInteger(a));
	}

	// 测试除法
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

	// 测试取余数
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

        NUT_TA((BigInteger(-5) << 1) == -10);
        NUT_TA((BigInteger(-6) >> 1) == -3);
        NUT_TA((BigInteger(-5) >> 1) == (-5 >> 1) && (-5 >> 1) == -3);
    }

    void testToString()
    {
        NUT_TA(BigInteger(0).toString() == "0");
        NUT_TA(BigInteger(0).toWString() == L"0");

        NUT_TA(BigInteger(10).toString() == "10");
        NUT_TA(BigInteger(10).toWString() == L"10");

        NUT_TA(BigInteger(-15).toString() == "-15");
        NUT_TA(BigInteger(-15).toWString() == L"-15");

        NUT_TA(BigInteger(0x4A).toString(0x10) == "4A");
        NUT_TA(BigInteger(0x4A).toWString(0x10) == L"4A");

        NUT_TA(BigInteger(13).toString(8) == "15");
        NUT_TA(BigInteger(13).toWString(8) == L"15");

        NUT_TA(BigInteger(5).toString(2) == "101");
        NUT_TA(BigInteger(5).toWString(2) == L"101");
    }

    void testValueOf()
    {
        NUT_TA(BigInteger::valueOf("0").llong_value() == 0);
        NUT_TA(BigInteger::valueOf(L"0").llong_value() == 0);

        NUT_TA(BigInteger::valueOf("10").llong_value() == 10);
        NUT_TA(BigInteger::valueOf(L"10").llong_value() == 10);

        NUT_TA(BigInteger::valueOf("-14").llong_value() == -14);
        NUT_TA(BigInteger::valueOf(L"-14").llong_value() == -14);

        NUT_TA(BigInteger::valueOf("4a", 0x10).llong_value() == 0x4a);
        NUT_TA(BigInteger::valueOf(L"4a", 0x10).llong_value() == 0x4a);

        NUT_TA(BigInteger::valueOf("15", 8).llong_value() == 13);
        NUT_TA(BigInteger::valueOf(L"15", 8).llong_value() == 13);

        NUT_TA(BigInteger::valueOf("101",2).llong_value() == 5);
        NUT_TA(BigInteger::valueOf(L"101",2).llong_value() == 5);
    }
};

NUT_REGISTER_FIXTURE(TestBigInteger, "numeric,quiet")
