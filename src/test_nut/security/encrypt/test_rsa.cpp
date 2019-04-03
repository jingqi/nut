
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
        NUT_REGISTER_CASE(test_smoking);
        NUT_REGISTER_CASE(test_bugs);
    }

    void test_smoking()
    {
        // 性能测试
        {
            const PerformanceCounter s = PerformanceCounter::now();
            RSA::PublicKey pk;
            RSA::PrivateKey vk;
            RSA::gen_key(1024, &pk, &vk);
            // cout << pk.n.bit_length() << endl;
            NUT_TA(pk.n.bit_length() == 1023 || pk.n.bit_length() == 1024);

            BigInteger a, b;
            a = RSA::transfer(BigInteger(1986), pk);
            b = RSA::transfer(a, vk);
            NUT_TA(b == 1986);

            a = RSA::transfer(BigInteger(0x457a), vk);
            b = RSA::transfer(a, pk);
            NUT_TA(b == 0x457a);

            const double t = PerformanceCounter::now() - s;
            printf(" %.6fs ", t);
        }
    }

    void test_bugs()
    {
        // bug 由于扩展欧几里得实现的有问题，这里可能测试失败
        RSA::PublicKey pk;
        RSA::PrivateKey vk;
        RSA::gen_key(31, &pk, &vk);
        // printf("public key :\n\te: %d\n\tn: %s\n", pk.e, pk.n.toString().c_str());
        // printf("private key :\n\td: %s\n\tn: %s\n", vk.d.toString().c_str(), vk.n.toString().c_str());
        // cout << pk.n.bit_length() << endl;
        NUT_TA(pk.n.bit_length() == 30 || pk.n.bit_length() == 31);

        BigInteger a, b;
        a = RSA::transfer(BigInteger(1986), pk);
        b = RSA::transfer(a, vk);
        NUT_TA(b == 1986);

        a = RSA::transfer(BigInteger(0x457a), vk);
        b = RSA::transfer(a, pk);
        NUT_TA(b == 0x457a);
    }
};

NUT_REGISTER_FIXTURE(TestRSA, "security, encrypt, quiet")
