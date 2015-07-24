
#ifndef ___HEADFILE_ABC2859B_32EC_4725_A95F_A4EA1F6DD526_
#define ___HEADFILE_ABC2859B_32EC_4725_A95F_A4EA1F6DD526_

#include <vector>

#include "test_logger.h"
#include "testcase_failure_exception.h"

namespace nut
{

class ConsoleTestLogger : public ITestLogger
{
    std::vector<TestCaseFailureException> _failures;

    int _count_of_fixtures = 0;
    int _count_of_failed_fixtures = 0;
    bool _current_fixture_failed = false;

    int _count_of_cases = 0;
    int _count_of_failed_cases = 0;
    bool _current_case_failed = false;

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
