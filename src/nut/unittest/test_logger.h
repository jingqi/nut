
#ifndef ___HEADFILE___83BCB398_BF24_4F6B_9BA9_6E0CBCDE2791_
#define ___HEADFILE___83BCB398_BF24_4F6B_9BA9_6E0CBCDE2791_

#include <stddef.h> // for size_t on windows/Mingw
#include <vector>

#include "../nut_config.h"
#include "testcase_failure_exception.h"


namespace nut
{

class NUT_API ITestLogger
{
public:
    virtual ~ITestLogger() = default;

    size_t get_count_of_fixtures() const noexcept;
    size_t get_count_of_failed_fixtures() const noexcept;
    size_t get_count_of_cases() const noexcept;
    size_t get_count_of_failed_cases() const noexcept;

    void start(const char *group_name, const char *fixture_name, const char *case_name) noexcept;
    void finish() noexcept;

    void enter_fixture(const char *fixture_name) noexcept;
    void leave_fixture() noexcept;

    void enter_case(const char *case_name) noexcept;
    void failed_case(const TestCaseFailureException& e) noexcept;
    void leave_case() noexcept;

    virtual void on_start(const char *group_name, const char *fixture_name,
                          const char *case_name) noexcept = 0;
    virtual void on_finish() noexcept = 0;

    virtual void on_enter_fixture(const char *fixture_name) noexcept = 0;
    virtual void on_leave_fixture() noexcept = 0;

    virtual void on_enter_case(const char *case_name) noexcept = 0;
    virtual void on_failed_case(const TestCaseFailureException& e) noexcept = 0;
    virtual void on_leave_case() noexcept = 0;

protected:
    size_t _count_of_fixtures = 0;
    size_t _count_of_failed_fixtures = 0;
    bool _current_fixture_failed = false;

    size_t _count_of_cases = 0;
    size_t _count_of_failed_cases = 0;
    bool _current_case_failed = false;

    std::vector<TestCaseFailureException> _failures;
};

}

#endif /* head file guarder */
