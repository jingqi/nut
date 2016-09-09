
#include "test_logger.h"

namespace nut
{

size_t ITestLogger::get_count_of_fixtures() const
{
    return _count_of_fixtures;
}

size_t ITestLogger::get_count_of_failed_fixtures() const
{
    return _count_of_failed_fixtures;
}

size_t ITestLogger::get_count_of_cases() const
{
    return _count_of_cases;
}

size_t ITestLogger::get_count_of_failed_cases() const
{
    return _count_of_failed_cases;
}

void ITestLogger::start(const char *group_name, const char *fixture_name, const char *case_name)
{
    _count_of_fixtures = 0;
    _count_of_failed_fixtures = 0;
    _current_fixture_failed = false;
    _count_of_cases = 0;
    _count_of_failed_cases = 0;
    _current_case_failed = false;
    _failures.clear();

    on_start(group_name, fixture_name, case_name);
}

void ITestLogger::finish()
{
    on_finish();
}

void ITestLogger::enter_fixture(const char *fixture_name)
{
    ++_count_of_fixtures;
    _current_fixture_failed = false;

    on_enter_fixture(fixture_name);
}

void ITestLogger::leave_fixture()
{
    if (_current_fixture_failed)
        ++_count_of_failed_fixtures;

    on_leave_fixture();
}

void ITestLogger::enter_case(const char *case_name)
{
    ++_count_of_cases;
    _current_case_failed = false;

    on_enter_case(case_name);
}

void ITestLogger::failed_case(const TestCaseFailureException& e)
{
    _current_case_failed = true;
    _current_fixture_failed = true;
    _failures.push_back(e);

    on_failed_case(e);
}

void ITestLogger::leave_case()
{
    if (_current_case_failed)
        ++_count_of_failed_cases;

    on_leave_case();
}

}
