
#ifndef ___HEADFILE___EB7AF659_F8F6_4075_AB30_D8F9BC85487E_
#define ___HEADFILE___EB7AF659_F8F6_4075_AB30_D8F9BC85487E_

#include <assert.h>
#include <ostream>
#include <vector>

#include "../nut_config.h"
#include "test_logger.h"


namespace nut
{

class NUT_API StreamTestLogger : public ITestLogger
{
public:
    StreamTestLogger(std::ostream *os);

    virtual void on_start(const char *group_name, const char *fixture_name, const char *case_name) override;
    virtual void on_finish() override;

    virtual void on_enter_fixture(const char *fixture_name) override;
    virtual void on_leave_fixture() override;

    virtual void on_enter_case(const char *case_name) override;
    virtual void on_failed_case(const TestCaseFailureException& e) override;
    virtual void on_leave_case() override;

private:
    std::ostream *_out_stream = nullptr;
};

}

#endif /* head file guarder */
