
#include <stdio.h>


#include <nut/unittest/unittest.hpp>

#include <nut/security/encrypt/rsa.hpp>

#include <time.h>

using namespace nut;

NUT_FIXTURE(TestRSA)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASE(testBugs)
    NUT_CASES_END()

    void setUp() {}
    void tearDown() {}

    void testSmoking()
    {
        // 性能测试
        {
            clock_t s = clock();
            RSA::PublicKey pk;
            RSA::PrivateKey vk;
            RSA::genKey(1024, &pk, &vk);
            NUT_TA(RSA::decode(RSA::encode(BigInteger(1986), pk), vk) == 1986);
            NUT_TA(RSA::encode(RSA::decode(BigInteger(0x457a), vk), pk) == 0x457a);
            clock_t t = clock() - s;
            printf(" %ld ms ", t * 1000 / CLOCKS_PER_SEC);
        }
    }

    void testBugs()
    {
        // bug 由于扩展欧几里得实现的有问题，这里可能测试失败
        RSA::PublicKey pk;
        RSA::PrivateKey vk;
        RSA::genKey(31, &pk, &vk);
        // printf("public key :\n\te: %d\n\tn: %s\n", pk.e, pk.n.toString().c_str());
        // printf("private key :\n\td: %s\n\tn: %s\n", vk.d.toString().c_str(), vk.n.toString().c_str());
        NUT_TA(RSA::decode(RSA::encode(BigInteger(1986), pk), vk) == 1986);
        NUT_TA(RSA::encode(RSA::decode(BigInteger(0x457a), vk), pk) == 0x457a);
    }
};

NUT_REGISTER_FIXTURE(TestRSA, "security, encrypt, quiet")
