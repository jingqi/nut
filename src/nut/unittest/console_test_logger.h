
#ifndef ___HEADFILE_ABC2859B_32EC_4725_A95F_A4EA1F6DD526_
#define ___HEADFILE_ABC2859B_32EC_4725_A95F_A4EA1F6DD526_

#include <vector>

#include "../nut_config.h"
#include "test_logger.h"
#include "testcase_failure_exception.h"


namespace nut
{

class NUT_API ConsoleTestLogger : public ITestLogger
{
    bool _in_a_tty = false;

public:
    ConsoleTestLogger();

    virtual void on_start(const char *group_name, const char *fixture_name, const char *case_name) override;
    virtual void on_finish() override;

    virtual void on_enter_fixture(const char *fixture_name) override;
    virtual void on_leave_fixture() override;

    virtual void on_enter_case(const char *case_name) override;
    virtual void on_leave_case() override;
    virtual void on_failed_case(const TestCaseFailureException& e) override;
};

}

#endif
