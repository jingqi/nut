
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
        RSA::gen_key(100, &pubk, &prik);
        const PerformanceCounter e = PerformanceCounter::now();
        NUT_TA(s - e > 0);
    }
};

NUT_REGISTER_FIXTURE(TestPerformanceCounter, "util, quiet")
