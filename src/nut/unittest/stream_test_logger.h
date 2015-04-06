
#ifndef ___HEADFILE___EB7AF659_F8F6_4075_AB30_D8F9BC85487E_
#define ___HEADFILE___EB7AF659_F8F6_4075_AB30_D8F9BC85487E_

#include <assert.h>
#include <ostream>
#include <vector>

#include "testlogger.h"

namespace nut
{

class StreamTestLogger : public ITestLogger
{
    std::ostream& m_ostream;
    std::vector<TestCaseFailureException> m_failures;

    int m_count_of_fixtures;
    int m_count_of_failed_fixtures;
    bool m_current_fixture_failed;

    int m_count_of_cases;
    int m_count_of_failed_cases;
    bool m_current_case_failed;

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
