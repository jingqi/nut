
#ifndef ___HEADFILE___EB7AF659_F8F6_4075_AB30_D8F9BC85487E_
#define ___HEADFILE___EB7AF659_F8F6_4075_AB30_D8F9BC85487E_

#include <assert.h>
#include <ostream>
#include <vector>

#include "test_logger.h"

namespace nut
{

class StreamTestLogger : public ITestLogger
{
    std::ostream& _out_stream;
    std::vector<TestCaseFailureException> _failures;

    int _count_of_fixtures = 0;
    int _count_of_failed_fixtures = 0;
    bool _current_fixture_failed = false;

    int _count_of_cases = 0;
    int _count_of_failed_cases = 0;
    bool _current_case_failed = false;

public:
    StreamTestLogger(std::ostream& os);

    virtual void start(const char *group_name, const char *fixture_name, const char *case_name) override;
    virtual void finish() override;

    virtual void enter_fixture(const char *fixture_name) override;
    virtual void leave_fixture() override;

    virtual void enter_case(const char *case_name) override;
    virtual void leave_case() override;
    virtual void failed_case(const TestCaseFailureException& e) override;
};

}

#endif /* head file guarder */
