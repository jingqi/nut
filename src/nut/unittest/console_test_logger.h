
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
public:
    ConsoleTestLogger() noexcept;

    virtual void on_start(const char *group_name, const char *fixture_name,
                          const char *case_name) noexcept override;
    virtual void on_finish() noexcept override;

    virtual void on_enter_fixture(const char *fixture_name) noexcept override;
    virtual void on_leave_fixture() noexcept override;

    virtual void on_enter_case(const char *case_name) noexcept override;
    virtual void on_leave_case() noexcept override;
    virtual void on_failed_case(const TestCaseFailureException& e) noexcept override;

private:
    bool _in_a_tty = false;
};

}

#endif
