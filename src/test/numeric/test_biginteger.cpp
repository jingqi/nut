
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <nut/unittest/unittest.h>
#include <nut/numeric/big_integer.h>

using namespace std;
using namespace nut;

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(disable: 4307)
#endif

NUT_FIXTURE(TestBigInteger)
{
	NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASE(test_bugs)
    NUT_CASE(test_comparator)
    NUT_CASE(test_math_operator)
    NUT_CASE(test_square)
    NUT_CASE(test_divide)
    NUT_CASE(test_mod)
    NUT_CASE(test_bit_operator)
    NUT_CASE(test_to_string)
    NUT_CASE(test_value_of)
	NUT_CASES_END()

    void set_up() {}
    void tear_down() {}

    void test_smoking()
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
		NUT_TA(((BigInteger(0x12345LL) * BigInteger(0x12345LL)).llong_value() & 0xFFFFFFLL) == 
			((0x12345LL * 0x12345LL) & 0xFFFFFFLL));

		// /
		NUT_TA(BigInteger(3) / BigInteger(4) == BigInteger(3/4));
		NUT_TA(BigInteger(4) / BigInteger(3) == BigInteger(4/3));

		// %
		NUT_TA(BigInteger(3) % BigInteger(4) == BigInteger(3%4));
        NUT_TA(BigInteger(4) % BigInteger(3) == BigInteger(4%3));
	}

    void test_bugs()
    {
        // bug 无符号除数可能被当成有符号负数
        NUT_TA(BigInteger(999) / BigInteger(131) == BigInteger(999/131));
        NUT_TA(BigInteger(7) / BigInteger(128) == BigInteger(7/128));
        NUT_TA(BigInteger(128) / BigInteger(7) == BigInteger(128/7));
        NUT_TA(BigInteger(7) % BigInteger(128) == BigInteger(7%128));
        NUT_TA(BigInteger(128) % BigInteger(7) == BigInteger(128%7));

        // bug 由于操作数和计算结果共享内存，导致计算bug
        BigInteger a = BigInteger::value_of("-985809295"), b = BigInteger::value_of("6369612912");
        BigInteger c(a);
        c -= b;
        NUT_TA(c == a - b);
    }

    void test_comparator()
	{
		NUT_TA(BigInteger(2) < BigInteger(5));
		NUT_TA(BigInteger(9) > BigInteger(-5));
		NUT_TA(BigInteger(9) > BigInteger(5));
		NUT_TA(!(BigInteger(2) < BigInteger(2)));
	}

    void test_math_operator()
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
        a.data()[0] = 45;
        a.data()[1] = 0x10000;
        a.data()[2] = 5;
        a.data()[3] = 68;
        NUT_TA(a * a == a * BigInteger(a));
	}

	// 测试除法
    void test_divide()
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

    // 测试平方
    void test_square()
    {
        // 平法优化算法处理负数时可能出错
        int a[2] = {-2, -1};
        int x[6];
        signed_multiply<int>(a, 2, a, 2, x, 6);
        //cout << endl << x[0] << ' ' << x[1] << ' ' << x[2] << ' ' << x[3] <<
        //        ' ' << x[4] << ' ' << x[5] << endl;
        NUT_TA(x[0] == 4 && x[1] == 0 && x[2] == 0 && x[3] == 0 && x[4] == 0 && x[5] == 0);
    }

	// 测试取余数
    void test_mod()
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

    void test_bit_operator()
	{
		NUT_TA((BigInteger(5) << 2) == BigInteger(5 << 2));
        NUT_TA((BigInteger(5) >> 1) == BigInteger(5 >> 1));

        NUT_TA((BigInteger(-5) << 1) == -10);
        NUT_TA((BigInteger(-6) >> 1) == -3);
        NUT_TA((BigInteger(-5) >> 1) == (-5 >> 1) && (-5 >> 1) == -3);
    }

    void test_to_string()
    {
        string s;
        wstring ws;
        BigInteger(0).to_string(&s);
        NUT_TA(s == "0");
        BigInteger(0).to_string(&ws);
        NUT_TA(ws == L"0");

        s.clear();
        BigInteger(10).to_string(&s);
        NUT_TA(s == "10");
        ws.clear();
        BigInteger(10).to_string(&ws);
        NUT_TA(ws == L"10");

        s.clear();
        BigInteger(-15).to_string(&s);
        NUT_TA(s == "-15");
        ws.clear();
        BigInteger(-15).to_string(&ws);
        NUT_TA(ws == L"-15");

        s.clear();
        BigInteger(0x4A).to_string(&s, 0x10);
        NUT_TA(s == "4A");
        ws.clear();
        BigInteger(0x4A).to_string(&ws, 0x10);
        NUT_TA(ws == L"4A");

        s.clear();
        BigInteger(13).to_string(&s, 8);
        NUT_TA(s == "15");
        ws.clear();
        BigInteger(13).to_string(&ws, 8);
        NUT_TA(ws == L"15");

        s.clear();
        BigInteger(5).to_string(&s, 2);
        NUT_TA(s == "101");
        ws.clear();
        BigInteger(5).to_string(&ws, 2);
        NUT_TA(ws == L"101");
    }

    void test_value_of()
    {
        NUT_TA(BigInteger::value_of("0").llong_value() == 0);
        NUT_TA(BigInteger::value_of(L"0").llong_value() == 0);

        NUT_TA(BigInteger::value_of("10").llong_value() == 10);
        NUT_TA(BigInteger::value_of(L"10").llong_value() == 10);

        NUT_TA(BigInteger::value_of("-14").llong_value() == -14);
        NUT_TA(BigInteger::value_of(L"-14").llong_value() == -14);

        NUT_TA(BigInteger::value_of("4a", 0x10).llong_value() == 0x4a);
        NUT_TA(BigInteger::value_of(L"4a", 0x10).llong_value() == 0x4a);

        NUT_TA(BigInteger::value_of("15", 8).llong_value() == 13);
        NUT_TA(BigInteger::value_of(L"15", 8).llong_value() == 13);

        NUT_TA(BigInteger::value_of("101",2).llong_value() == 5);
        NUT_TA(BigInteger::value_of(L"101",2).llong_value() == 5);
    }
};

NUT_REGISTER_FIXTURE(TestBigInteger, "numeric,quiet")
