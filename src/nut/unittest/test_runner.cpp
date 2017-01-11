
#include <assert.h>
#include <string.h>

#include "test_runner.h"
#include "test_register.h"
#include "test_fixture.h"

namespace nut
{

TestRunner::TestRunner(ITestLogger *logger)
    : _logger(logger)
{
    assert(nullptr != logger);
}

void TestRunner::run_group(const char *group_name)
{
    assert(nullptr != group_name);

    _logger->start(group_name, nullptr, nullptr);

    TestRegister *pregister = *TestRegister::get_link_header();
    while (nullptr != pregister)
    {
        if (pregister->match_group(group_name))
        {
            _logger->enter_fixture(pregister->get_fixture_name());

            TestFixture *fixture = pregister->new_fixture();
            assert(nullptr != fixture);
            int total = fixture->___run_case(_logger, __NUT_UNITTEST_SUM_CASES_OP, nullptr);

            for (int i = 0; i < total; ++i)
            {
                if (nullptr == fixture)
                    fixture = pregister->new_fixture();
                fixture->___run_case(_logger, i, nullptr);
                pregister->delete_fixture(fixture);
                fixture = nullptr;
            }
            if (nullptr != fixture)
                pregister->delete_fixture(fixture);

            _logger->leave_fixture();
        }
        pregister = pregister->get_next_register();
    }

    _logger->finish();
}

void TestRunner::run_fixture(const char *fixture_name)
{
    assert(nullptr != fixture_name);

    _logger->start(nullptr, fixture_name, nullptr);

    TestRegister *pregister = *TestRegister::get_link_header();
    while (nullptr != pregister)
    {
        if (0 == ::strcmp(fixture_name, pregister->get_fixture_name()))
        {
            _logger->enter_fixture(fixture_name);

            TestFixture *fixture = pregister->new_fixture();
            assert(nullptr != fixture);
            int total = fixture->___run_case(_logger, __NUT_UNITTEST_SUM_CASES_OP, nullptr);

            for (int i = 0; i < total; ++i)
            {
                if (nullptr == fixture)
                    fixture = pregister->new_fixture();
                fixture->___run_case(_logger, i, nullptr);
                pregister->delete_fixture(fixture);
                fixture = nullptr;
            }
            if (nullptr != fixture)
                pregister->delete_fixture(fixture);

            _logger->leave_fixture();
            break;
        }
        pregister = pregister->get_next_register();
    }

    _logger->finish();
}

void TestRunner::run_case(const char *fixture_name, const char *case_name)
{
    assert(nullptr != fixture_name && nullptr != case_name);

    _logger->start(nullptr, fixture_name, case_name);

    TestRegister *pregister = *TestRegister::get_link_header();
    while (nullptr != pregister)
    {
        if (0 == ::strcmp(fixture_name, pregister->get_fixture_name()))
        {
            _logger->enter_fixture(fixture_name);

            TestFixture *fixture = pregister->new_fixture();
            fixture->___run_case(_logger, __NUT_UNITTEST_RUN_NAMED_CASE_OP, case_name);
            pregister->delete_fixture(fixture);

            _logger->leave_fixture();
            break;
        }
        pregister = pregister->get_next_register();
    }

    _logger->finish();
}

}
