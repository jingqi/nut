
#include <iostream>
#include <nut/unittest/unittest.hpp>
#include <nut/numeric/numeric_algo/gcd.hpp>
#include <nut/numeric/numeric_algo/mod.hpp>
#include <nut/numeric/numeric_algo/prime.hpp>

#include <time.h>
#include <stdio.h>

using namespace nut;

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(disable: 4307)
#endif

NUT_FIXTURE(TestNumericAlgo)
{
	NUT_CASES_BEGIN()
    NUT_CASE(test_bugs)
    NUT_CASE(test_gcd)
    NUT_CASE(test_extend_euclid)
    NUT_CASE(test_mod_multiply)
    NUT_CASE(test_mod_pow)
    NUT_CASE(test_prime)
	NUT_CASES_END()

    void set_up() {}
    void tear_down() {}

    void test_bugs()
	{
        {
            // bug 根据二进制特性对扩展欧几里得算法的优化实现有问题
            // 实际上是 BigInteger 的 -= 操作由于计算结果和操作数共享内存导致的问题
            BigInteger a(65537), b = ::BigInteger::value_of("50956903296"), d, x, y;
            extended_euclid(a, b, &d, &x, &y);
            // printf("\n%s\n%s\n%s\n%s\n%s\n", a.toString().c_str(),b.toString().c_str(), d.toString().c_str(), x.toString().c_str(), y.toString().c_str());
            NUT_TA(d == a * x + b * y);
        }

        {
            // BUG 预算表优化的 _mod_multiply() 实现有问题
            BigInteger a(1), b = BigInteger::value_of("400000000", 16), n = BigInteger::value_of("c00000000", 16);
            ModMultiplyPreBuildTable<2> table(a, n);
            BigInteger x1;
            mod_multiply(b, n, table, &x1);
            BigInteger x2 = (a * b) % n;
//            printf("\n%s\n%s\n", x1.toString().c_str(), x2.toString().c_str());
            NUT_TA(x1 == x2);
        }
	}

    void test_gcd()
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
            printf(" %ld ms ", t * 1000 / CLOCKS_PER_SEC);
        }
    }

    void test_extend_euclid()
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
            printf(" %ld ms ", t * 1000 / CLOCKS_PER_SEC);
        }
    }

    void test_mod_multiply()
    {
        {
            BigInteger bound(1);
            bound <<= 1024;
            BigInteger a = BigInteger::rand_between(bound, bound << 1);
            BigInteger b = BigInteger::rand_between(bound, bound << 1);
            BigInteger n = BigInteger::rand_between(bound, bound << 1);
            BigInteger x;

            ModMultiplyPreBuildTable<4> table(a % n, n);
            mod_multiply(b % n, n, table, &x);
            NUT_TA(x == (a * b) % n);
        }

        // 测性能
        {
            BigInteger bound(1);
            bound <<= 1024;
            BigInteger a = BigInteger::rand_between(bound, bound << 1);
            BigInteger b = BigInteger::rand_between(bound, bound << 1);
            BigInteger n = BigInteger::rand_between(bound, bound << 1);
            BigInteger x;

            clock_t s = clock();
            for (int i = 0; i < 1; ++i)
            {
                // x = mod_multiply(a, b, n);
                x = (a * b) % n;
            }
            clock_t t = clock() - s;
            // printf("\n%s\n%s\n%s\n%s\n", a.toString().c_str(), b.toString().c_str(), n.toString().c_str(), x.toString().c_str());
            NUT_TA(x == (a * b) % n);
            printf(" %ld ms ", t * 1000 / CLOCKS_PER_SEC);
        }
    }

    // 用作对比
    static BigInteger mod_pow2(const BigInteger& a, const BigInteger& b, const BigInteger& n)
    {
        assert(a.is_positive() && b.is_positive() && n.is_positive());

        BigInteger ret(1);
        for (register size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
        {
            ret = (ret * ret) % n;
            if (0 != b.bit_at(i - 1))
                ret = (ret * a) % n;
        }
        return ret;
    }

    void test_mod_pow()
    {
        // 测性能
        {
            BigInteger bound(1);
            bound <<= 900;
            BigInteger a = BigInteger::rand_between(bound, bound << 1);
            BigInteger b = BigInteger::rand_between(bound, bound << 1);
            BigInteger n = BigInteger::rand_between(bound, bound << 1);
            BigInteger x1, x2;

            const int iteration = 1;
            clock_t s = clock();
            for (int i = 0; i < iteration; ++i)
            {
                mod_pow(a, b, n, &x1);
            }
            clock_t t1 = clock() - s;
            for (int i = 0; i < iteration; ++i)
            {
                x2 = mod_pow2(a, b, n);
            }
            clock_t t2 = clock() - t1;
            printf(" %ld ms < %ld ms ", t1 * 1000 / CLOCKS_PER_SEC, t2 * 1000 / CLOCKS_PER_SEC);
            NUT_TA(x1 == x2);
        }
    }

    void test_prime()
    {
        // 性能测试
        BigInteger bound(1);
        bound <<= 512;

        clock_t s = clock();
        BigInteger a = next_prime(bound);
        clock_t t = clock() - s;
        printf(" %ld ms ", t * 1000 / CLOCKS_PER_SEC);
    }
};

NUT_REGISTER_FIXTURE(TestNumericAlgo, "numeric,quiet")
