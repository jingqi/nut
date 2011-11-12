/**
 * @file -
 * @author jingqi
 * @date 2011-11-12 13:27
 */

#ifndef ___HEADFILE___5E5FFC51_E0AE_4F6B_8661_502D66D82839_
#define ___HEADFILE___5E5FFC51_E0AE_4F6B_8661_502D66D82839_

/*

Ê¹ÓÃÊ¾Àý£º

#include <nut/unittest/unittest.hpp>

NUT_FIXTURE(TestTestUnit)
{
    NUT_CASES_BEGIN()
    NUT_CASE(smokingTest)
    NUT_CASES_END()

    void setUp() {}

    void tearDown() {}

    void smokingTest () {
        NUT_TA(5 > 4);
    }
};

NUT_REGISTER_FIXTURE(TestTestUnit, "test,quiet")


#include <nut/unittest/unittest.hpp>
int main()
{
    nut::TestRunner runner(std::cout);
    runner.runAGroup("quiet");
    runner.runAFixture("TestTestUnit");
    runner.runACase("TestTestUnit","smokingTest");

    return 0;
}

*/

/* *************************************************************************/

#include "testcasefailureexception.hpp"
#include "testlogger.hpp"
#include "testfixture.hpp"
#include "testregister.hpp"
#include "testrunner.hpp"

/** fixture */
#define NUT_FIXTURE(fixtureName) \
class fixtureName : public nut::TestFixture

/** cases begin */
#define NUT_CASES_BEGIN() \
    virtual int ___runACase(nut::ITestLogger& logger, const int op, const char *caseName) { \
        int index = -1;

/** case */
#define NUT_CASE(caseFunc) \
        ++index; \
        if (op == index || ((-1 == op) && (0 == ::strcmp(#caseFunc, caseName)))) { \
            logger.enterCase(#caseFunc); \
            try { \
                setUp(); \
                caseFunc(); \
                tearDown(); \
            } catch (nut::TestCaseFailureException e) { \
                logger.failedCase(e); \
            } catch (...) { \
                nut::TestCaseFailureException e("Unhandled exception", __FILE__, __LINE__); \
                logger.failedCase(e); \
            } \
            logger.leaveCase(); \
        }

/** cases end */
#define NUT_CASES_END() \
        return index + 1; \
    }

/** test assert */
#define NUT_TA(exp) if (!(exp)) throw nut::TestCaseFailureException(#exp, __FILE__, __LINE__)

/** fixture register */
#define NUT_REGISTER_FIXTURE(fixture, groups) \
    static nut::TestFixture* ___new##fixture() { return new fixture(); } \
    static void ___delete##fixture(nut::TestFixture *p) { delete p; } \
    static nut::TestRegister ___register##fixture(#fixture, groups, ___new##fixture, ___delete##fixture);


#endif /* head file guarder */
