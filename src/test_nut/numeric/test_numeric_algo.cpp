
#include <time.h>
#include <stdio.h>
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/numeric/numeric_algo/gcd.h>
#include <nut/numeric/numeric_algo/mod.h>
#include <nut/numeric/numeric_algo/prime.h>
#include <nut/numeric/numeric_algo/karatsuba.h>
#include <nut/time/performance_counter.h>
#include <nut/rc/rc_new.h>

using namespace nut;

#if NUT_PLATFORM_CC_VC
#   pragma warning(disable: 4307)
#endif

class TestNumericAlgo : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_bugs);
        NUT_REGISTER_CASE(test_gcd);
        NUT_REGISTER_CASE(test_extend_euclid);
        NUT_REGISTER_CASE(test_bi_mult_mod);
        NUT_REGISTER_CASE(test_bi_pow_mod);
        NUT_REGISTER_CASE(test_prime);
        NUT_REGISTER_CASE(test_karatsuba_multiply);
    }

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
            BigInteger x1 = mult_mod(b, n, table);
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

            const PerformanceCounter s = PerformanceCounter::now();
            for (int i = 0; i < 10; ++i)
            {
                d = gcd(a, b);
                NUT_TA(a % d == 0 && b % d == 0);
            }
            const double t = PerformanceCounter::now() - s;
            printf(" %.6fs ", t);
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

            const PerformanceCounter s = PerformanceCounter::now();
            for (int i = 0; i < 10; ++i)
            {
                extended_euclid(a, b, &d, &x, &y);
                NUT_TA(d == a * x + b * y);
            }
            const double t = PerformanceCounter::now() - s;
            printf(" %.6fs ", t);
        }
    }

    void test_bi_mult_mod()
    {
        {
            BigInteger bound(1);
            bound <<= 1024;
            BigInteger a = BigInteger::rand_between(bound, bound << 1);
            BigInteger b = BigInteger::rand_between(bound, bound << 1);
            BigInteger n = BigInteger::rand_between(bound, bound << 1);

            ModMultiplyPreBuildTable<4> table(a % n, n);
            BigInteger x = mult_mod(b % n, n, table);
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

            const PerformanceCounter s = PerformanceCounter::now();
            for (int i = 0; i < 1; ++i)
            {
                // x = mod_multiply(a, b, n);
                x = (a * b) % n;
            }
            const double t = PerformanceCounter::now() - s;
            // printf("\n%s\n%s\n%s\n%s\n", a.toString().c_str(), b.toString().c_str(), n.toString().c_str(), x.toString().c_str());
            NUT_TA(x == (a * b) % n);
            printf(" %.6fs ", t);
        }
    }

    // 用作对比
    static BigInteger pow_mod_v2(const BigInteger& a, const BigInteger& b, const BigInteger& n)
    {
        assert(a.is_positive() && b.is_positive() && n.is_positive());

        BigInteger ret(1);
        for (size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
        {
            ret = (ret * ret) % n;
            if (0 != b.bit_at(i - 1))
                ret = (ret * a) % n;
        }
        return ret;
    }

    void test_bi_pow_mod()
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
            const PerformanceCounter s = PerformanceCounter::now();
            for (int i = 0; i < iteration; ++i)
            {
                x1 = pow_mod(a, b, n);
            }
            const PerformanceCounter t1 = PerformanceCounter::now();
            for (int i = 0; i < iteration; ++i)
            {
                x2 = pow_mod_v2(a, b, n);
            }
            const double t2 = PerformanceCounter::now() - t1;
            printf(" %.6fs < %.6fs ", t1 - s, t2);
            NUT_TA(x1 == x2);
        }
    }

    void test_prime()
    {
        // 性能测试
        BigInteger bound(1);
        bound <<= 512;

        const PerformanceCounter s = PerformanceCounter::now();
        BigInteger a = next_prime(bound);
        const double t = PerformanceCounter::now() - s;
        printf(" %.6fs ", t);
    }

    void test_karatsuba_multiply()
    {
        typedef unsigned word_type;
        size_t a_len = 10000, b_len = 10186, x_len = a_len + b_len;
        word_type *a = (word_type*) ::malloc(sizeof(word_type) * a_len),
                *b = (word_type*) ::malloc(sizeof(word_type) * b_len),
                *x = (word_type*) ::malloc(sizeof(word_type) * x_len),
                *y = (word_type*) ::malloc(sizeof(word_type) * x_len);
        for (size_t i = 0; i < sizeof(word_type) * a_len; ++i)
            reinterpret_cast<uint8_t*>(a)[i] = ::rand();
        for (size_t i = 0; i < sizeof(word_type) * b_len; ++i)
            reinterpret_cast<uint8_t*>(b)[i] = ::rand();
        ::memset(x, 0, sizeof(word_type) * x_len);
        ::memset(y, 0, sizeof(word_type) * x_len);

        PerformanceCounter s = PerformanceCounter::now();
        signed_multiply<word_type>(a, a_len, b, b_len, x, x_len);
        PerformanceCounter f1 = PerformanceCounter::now();
        signed_karatsuba_multiply<word_type>(a, a_len, b, b_len, y, x_len);
        PerformanceCounter f2 = PerformanceCounter::now();
        printf(" %.6fs(origin %.6fs)", f2 - f1, f1 - s);
        NUT_TA(0 == ::memcmp(x, y, sizeof(word_type) * x_len));

        x_len = 256; // x 变小，应该对此做优化
        s = PerformanceCounter::now();
        signed_multiply<word_type>(a, a_len, b, b_len, x, x_len);
        f1 = PerformanceCounter::now();
        signed_karatsuba_multiply<word_type>(a, a_len, b, b_len, y, x_len);
        f2 = PerformanceCounter::now();
        printf(" %.6fs(orgin %.6fs)", f2 - f1, f1 - s);
        NUT_TA(0 == ::memcmp(x, y, sizeof(word_type) * x_len));

        ::free(a);
        ::free(b);
        ::free(x);
        ::free(y);
    }
};

NUT_REGISTER_FIXTURE(TestNumericAlgo, "numeric,quiet")
