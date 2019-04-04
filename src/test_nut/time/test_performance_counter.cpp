
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/time/performance_counter.h>
#include <nut/security/encrypt/rsa.h>


using namespace std;
using namespace nut;

class TestPerformanceCounter : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        const PerformanceCounter s = PerformanceCounter::now();
        RSA::gen_key(20);
        const double elapse = PerformanceCounter::now() - s;
        cout << " " << elapse;
        NUT_TA(elapse > 0);
    }
};

NUT_REGISTER_FIXTURE(TestPerformanceCounter, "util, quiet")
