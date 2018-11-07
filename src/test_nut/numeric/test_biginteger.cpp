
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <nut/unittest/unittest.h>
#include <nut/numeric/big_integer.h>

using namespace std;
using namespace nut;

#if NUT_PLATFORM_CC_VC
#   pragma warning(disable: 4307)
#endif

class TestBigInteger : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
        NUT_REGISTER_CASE(test_bugs);
        NUT_REGISTER_CASE(test_comparator);
        NUT_REGISTER_CASE(test_math_operator);
        NUT_REGISTER_CASE(test_square);
        NUT_REGISTER_CASE(test_divide);
        NUT_REGISTER_CASE(test_mod);
        NUT_REGISTER_CASE(test_bit_operator);
        NUT_REGISTER_CASE(test_to_string);
        NUT_REGISTER_CASE(test_value_of);
    }

    void test_smoking()
    {
        // +
        NUT_TA((BigInteger(12) + BigInteger(13)).to_integer() == 12 + 13);
        NUT_TA((BigInteger(12) + BigInteger(-11)).to_integer() == 12 - 11);
        NUT_TA((BigInteger(12) + BigInteger(-13)).to_integer() == 12 - 13);

        // -
        NUT_TA((BigInteger(12) - BigInteger(1)).to_integer() == 12 - 1);
        NUT_TA((BigInteger(12) - BigInteger(-1)).to_integer() == 12 - -1);
        NUT_TA((BigInteger(12) - BigInteger(13)).to_integer() == 12 - 13);

        // *
        NUT_TA((BigInteger(12) * BigInteger(1)).to_integer() == 12 * 1);
        NUT_TA((BigInteger(12) * BigInteger(24)).to_integer() == 12 * 24);
        NUT_TA(((BigInteger(0x12345LL) * BigInteger(0x12345LL)).to_integer() & 0xFFFFFFLL) ==
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
        NUT_TA(BigInteger(0).to_string() == "0");
        NUT_TA(BigInteger(0).to_wstring() == L"0");

        NUT_TA(BigInteger(10).to_string() == "10");
        NUT_TA(BigInteger(10).to_wstring() == L"10");

        NUT_TA(BigInteger(-15).to_string() == "-15");
        NUT_TA(BigInteger(-15).to_wstring() == L"-15");

        NUT_TA(BigInteger(0x4A).to_string(0x10) == "4A");
        NUT_TA(BigInteger(0x4A).to_wstring(0x10) == L"4A");

        NUT_TA(BigInteger(13).to_string(8) == "15");
        NUT_TA(BigInteger(13).to_wstring(8) == L"15");

        NUT_TA(BigInteger(5).to_string(2) == "101");
        NUT_TA(BigInteger(5).to_wstring(2) == L"101");
    }

    void test_value_of()
    {
        NUT_TA(BigInteger::value_of("0").to_integer() == 0);
        NUT_TA(BigInteger::value_of(L"0").to_integer() == 0);

        NUT_TA(BigInteger::value_of("10").to_integer() == 10);
        NUT_TA(BigInteger::value_of(L"10").to_integer() == 10);

        NUT_TA(BigInteger::value_of("-14").to_integer() == -14);
        NUT_TA(BigInteger::value_of(L"-14").to_integer() == -14);

        NUT_TA(BigInteger::value_of("4a", 0x10).to_integer() == 0x4a);
        NUT_TA(BigInteger::value_of(L"4a", 0x10).to_integer() == 0x4a);

        NUT_TA(BigInteger::value_of("15", 8).to_integer() == 13);
        NUT_TA(BigInteger::value_of(L"15", 8).to_integer() == 13);

        NUT_TA(BigInteger::value_of("101",2).to_integer() == 5);
        NUT_TA(BigInteger::value_of(L"101",2).to_integer() == 5);
    }
};

NUT_REGISTER_FIXTURE(TestBigInteger, "numeric,quiet")
