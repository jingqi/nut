
#include <iostream>
#include <nut/unittest/unittest.hpp>
#include <nut/numeric/numeric_algo.hpp>

#include <time.h>

using namespace nut;

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(disable: 4307)
#endif

NUT_FIXTURE(TestNumericAlgo)
{
	NUT_CASES_BEGIN()
	NUT_CASE(testBugs)
    NUT_CASE(testGcd)
    NUT_CASE(testExtendEuclid)
	NUT_CASES_END()

	void setUp() {}
	void tearDown() {}

	void testBugs()
	{
        // bug 根据二进制特性对扩展欧几里得算法的优化实现有问题
        // 实际上是 BigInteger 的 -= 操作由于计算结果和操作数共享内存导致的问题
        BigInteger a(65537), b = ::BigInteger::valueOf("50956903296"), d, x, y;
        extended_euclid(a, b, &d, &x, &y);
        // printf("\n%s\n%s\n%s\n%s\n%s\n", a.toString().c_str(),b.toString().c_str(), d.toString().c_str(), x.toString().c_str(), y.toString().c_str());
        NUT_TA(d == a * x + b * y);
	}

    void testGcd()
    {
         // 测性能
        {
            BigInteger bound(1);
            bound <<= 1024;
            BigInteger a = BigInteger::rand_between(bound, bound << 1);
            BigInteger b = BigInteger::rand_between(bound, bound << 1);
            BigInteger d;

            clock_t s = clock();
            for (int i = 0; i < 10; ++i)
            {
                d = gcd(a, b);
                NUT_TA(a % d == 0 && b % d == 0);
            }
            clock_t t = clock() - s;
            printf(" %ld ms ", t);
        }
    }

    void testExtendEuclid() 
    {
        // 测性能
        {
            BigInteger bound(1);
            bound <<= 1024;
            BigInteger a = BigInteger::rand_between(bound, bound << 1);
            BigInteger b = BigInteger::rand_between(bound, bound << 1);
            BigInteger d, x, y;

            clock_t s = clock();
            for (int i = 0; i < 10; ++i)
            {
                extended_euclid(a, b, &d, &x, &y);
                NUT_TA(d == a * x + b * y);
            }
            clock_t t = clock() - s;
            printf(" %ld ms ", t);
        }
    }
};

NUT_REGISTER_FIXTURE(TestNumericAlgo, "numeric,quiet")
