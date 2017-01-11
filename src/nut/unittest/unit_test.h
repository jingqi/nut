
#ifndef ___HEADFILE___5E5FFC51_E0AE_4F6B_8661_502D66D82839_
#define ___HEADFILE___5E5FFC51_E0AE_4F6B_8661_502D66D82839_

/*

使用示例：

#include <nut/unittest/unittest.hpp>

NUT_FIXTURE(TestTestUnit)
{
    NUT_CASES_BEGIN()
    NUT_CASE(smoking_test)
    NUT_CASES_END()

    void set_up() {}

    void tear_down() {}

    void smoking_test () {
        NUT_TA(5 > 4);
    }
};

NUT_REGISTER_FIXTURE(TestTestUnit, "test,quiet")


#include <nut/unittest/unittest.hpp>
int main()
{
    nut::TestRunner runner(std::cout);
    runner.run_group("quiet");
    runner.run_fixture("TestTestUnit");
    runner.run_case("TestTestUnit","smokingTest");

    return 0;
}

*/

/* *************************************************************************/

#include <assert.h>
#include <string.h> // for strcmp()
#include <stdlib.h> // for malloc()

#include "testcase_failure_exception.h"
#include "test_logger.h"
#include "test_fixture.h"
#include "test_register.h"
#include "test_runner.h"

/** fixture */
#define NUT_FIXTURE(fixture_name)               \
class fixture_name : public ::nut::TestFixture

/** cases begin */
#define NUT_CASES_BEGIN()                                               \
    virtual int ___run_case(::nut::ITestLogger *logger, const int op, const char *case_name) override \
    {                                                                   \
        assert(nullptr != logger);                                      \
        int index = -1;

/** case */
#define NUT_CASE(case_func)                                             \
        ++index;                                                        \
        if (op == index || ((__NUT_UNITTEST_RUN_NAMED_CASE_OP == op) && (0 == ::strcmp(#case_func, case_name)))) \
        {                                                               \
            logger->enter_case(#case_func);                             \
            try                                                         \
            {                                                           \
                set_up();                                               \
                case_func();                                            \
                tear_down();                                            \
            }                                                           \
            catch (::nut::TestCaseFailureException& e)                  \
            {                                                           \
                logger->failed_case(e);                                 \
            }                                                           \
            catch (...)                                                 \
            {                                                           \
                ::nut::TestCaseFailureException e("Unhandled exception", __FILE__, __LINE__); \
                logger->failed_case(e);                                 \
            }                                                           \
            logger->leave_case();                                       \
        }

/** cases end */
#define NUT_CASES_END()     \
        return index + 1;   \
    }

/** test assert */
#define NUT_TA(exp) do { if (!(exp)) throw ::nut::TestCaseFailureException(#exp, __FILE__, __LINE__); } while (false)

/** fixture register */
#define NUT_REGISTER_FIXTURE(fixture, groups)                           \
    static ::nut::TestFixture* ___new##fixture()                        \
    {                                                                   \
        fixture *p = (fixture*) ::malloc(sizeof(fixture));              \
        assert(nullptr != p);                                           \
        new (p) fixture();                                              \
        return p;                                                       \
    }                                                                   \
    static void ___delete##fixture(::nut::TestFixture *p)               \
    {                                                                   \
        assert(nullptr != p);                                           \
        p->~TestFixture();                                              \
        ::free(p);                                                      \
    }                                                                   \
    static ::nut::TestRegister ___register##fixture(#fixture, groups, ___new##fixture, ___delete##fixture);


#endif /* head file guarder */
