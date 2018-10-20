
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/util/time/performance_counter.h>
#include <nut/security/encrypt/rsa.h>


using namespace std;
using namespace nut;

NUT_FIXTURE(TestPerformanceCounter)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void test_smoking()
    {
        const PerformanceCounter s = PerformanceCounter::now();
        RSA::PublicKey pubk;
        RSA::PrivateKey prik;
        RSA::gen_key(20, &pubk, &prik);
        const double elapse = PerformanceCounter::now() - s;
        cout << " " << elapse;
        NUT_TA(elapse > 0);
    }
};

NUT_REGISTER_FIXTURE(TestPerformanceCounter, "util, quiet")
