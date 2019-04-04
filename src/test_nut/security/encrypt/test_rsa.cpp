
#include <stdio.h>
#include <iostream>


#include <nut/unittest/unittest.h>
#include <nut/time/performance_counter.h>
#include <nut/security/encrypt/rsa.h>

#include <time.h>

using namespace std;
using namespace nut;

class TestRSA : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_profile);
        NUT_REGISTER_CASE(test_bugs);
    }

    void test_profile()
    {
        // 性能测试
        {
            const PerformanceCounter s = PerformanceCounter::now();
            RSA::PrivateKey key = RSA::gen_key(2048);
            // cout << key.n.bit_length() << endl;
            NUT_TA(key.n.bit_length() == 2047 || key.n.bit_length() == 2048);

            for (int i = 0; i < 20; ++i)
            {
                BigInteger a, b;
                a = RSA::public_transfer(BigInteger(1986), key);
                b = RSA::private_transfer(a, key);
                NUT_TA(b == 1986);

                a = RSA::private_transfer(BigInteger(0x457a), key);
                b = RSA::public_transfer(a, key);
                NUT_TA(b == 0x457a);
            }

            const double t = PerformanceCounter::now() - s;
            printf(" %.6fs ", t);
        }
    }

    void test_bugs()
    {
        // bug 由于扩展欧几里得实现的有问题，这里可能测试失败
        RSA::PrivateKey key = RSA::gen_key(31);
        // printf("public key :\n\te: %d\n\tn: %s\n", key.e, key.n.toString().c_str());
        // printf("private key :\n\td: %s\n\tn: %s\n", key.d.toString().c_str(), key.n.toString().c_str());
        // cout << key.n.bit_length() << endl;
        NUT_TA(key.n.bit_length() == 30 || key.n.bit_length() == 31);

        BigInteger a, b;
        a = RSA::public_transfer(BigInteger(1986), key);
        b = RSA::private_transfer(a, key);
        NUT_TA(b == 1986);

        a = RSA::private_transfer(BigInteger(0x457a), key);
        b = RSA::public_transfer(a, key);
        NUT_TA(b == 0x457a);
    }
};

NUT_REGISTER_FIXTURE(TestRSA, "security, encrypt, quiet")
