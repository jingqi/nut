/**

使用示例：

#include <nut/unittest/unittest.h>

NUT_FIXTURE(TestTestUnit)
{
    NUT_CASES_BEGIN()
    NUT_CASE(smoking_test)
    NUT_CASES_END()

    void set_up()
    {}

    void tear_down()
    {}

    void smoking_test ()
    {
        NUT_TA(5 > 4);
    }
};

NUT_REGISTER_FIXTURE(TestTestUnit, "test,quiet")


#include <nut/unittest/unittest.h>
int main()
{
    nut::TestRunner runner(std::cout);
    runner.run_group("quiet");
    runner.run_fixture("TestTestUnit");
    runner.run_case("TestTestUnit","smokingTest");

    return 0;
}

*/

#ifndef ___HEADFILE___5E5FFC51_E0AE_4F6B_8661_502D66D82839_
#define ___HEADFILE___5E5FFC51_E0AE_4F6B_8661_502D66D82839_

#include <assert.h>
#include <string.h> // for strcmp()
#include <stdlib.h> // for malloc()

#include "testcase_failure_exception.h"
#include "test_logger.h"
#include "test_fixture.h"
#include "test_register.h"
#include "test_runner.h"


/** register case */
#define NUT_REGISTER_CASE(case_func)                                    \
    do { register_case(#case_func, [=] { case_func(); }); } while (false)

/** test assert */
#define NUT_TA(exp)                                                     \
    do                                                                  \
    {                                                                   \
        if (!(exp))                                                     \
            throw nut::TestCaseFailureException(#exp, __FILE__, __LINE__); \
    } while (false)

/** fixture register */
#define NUT_REGISTER_FIXTURE(fixture, groups)                   \
    static nut::TestRegister ___register##fixture(              \
        #fixture, groups,                                       \
        [] {                                                    \
            fixture *p = (fixture*) ::malloc(sizeof(fixture));  \
            assert(nullptr != p);                               \
            new (p) fixture();                                  \
            return p;                                           \
        },                                                      \
        [] (nut::TestFixture*p) {                               \
            assert(nullptr != p);                               \
            p->~TestFixture();                                  \
            ::free(p);                                          \
        });

#endif /* head file guarder */
