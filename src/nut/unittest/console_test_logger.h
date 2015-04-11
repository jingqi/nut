
#ifndef ___HEADFILE_ABC2859B_32EC_4725_A95F_A4EA1F6DD526_
#define ___HEADFILE_ABC2859B_32EC_4725_A95F_A4EA1F6DD526_

#include <vector>

#include "testlogger.h"
#include "testcase_failure_exception.h"

namespace nut
{

class ConsoleTestLogger : public ITestLogger
{
    std::vector<TestCaseFailureException> m_failures;

    int m_count_of_fixtures;
    int m_count_of_failed_fixtures;
    bool m_current_fixture_failed;

    int m_count_of_cases;
    int m_count_of_failed_cases;
    bool m_current_case_failed;

public:
    virtual void start(const char *group_name, const char *fixture_name, const char *case_name) override;
    virtual void finish() override;

    virtual void enter_fixture(const char *fixture_name) override;
    virtual void leave_fixture() override;

    virtual void enter_case(const char *case_name) override;
    virtual void leave_case() override;
    virtual void failed_case(const TestCaseFailureException& e) override;
};

}

#endif
