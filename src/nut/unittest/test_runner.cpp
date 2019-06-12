
#include <assert.h>
#include <string.h>

#include "test_runner.h"
#include "test_register.h"
#include "test_fixture.h"


namespace nut
{

TestRunner::TestRunner(ITestLogger *logger) noexcept
    : _logger(logger)
{
    assert(nullptr != logger);
}

void TestRunner::run_group(const char *group_name) noexcept
{
    assert(nullptr != group_name);

    _logger->start(group_name, nullptr, nullptr);

    for (TestRegister *reg = *TestRegister::get_link_header();
         nullptr != reg; reg = reg->get_next_register())
    {
        if (!reg->match_group(group_name))
            continue;

        _logger->enter_fixture(reg->get_fixture_name());

        TestFixture *fixture = reg->new_fixture();
        assert(nullptr != fixture);

        fixture->register_cases();
        fixture->run_case(_logger, nullptr);

        reg->delete_fixture(fixture);

        _logger->leave_fixture();
    }

    _logger->finish();
}

void TestRunner::run_fixture(const char *fixture_name) noexcept
{
    assert(nullptr != fixture_name);

    _logger->start(nullptr, fixture_name, nullptr);

    for (TestRegister *reg = *TestRegister::get_link_header();
         nullptr != reg; reg = reg->get_next_register())
    {
        if (0 != ::strcmp(fixture_name, reg->get_fixture_name()))
            continue;

        _logger->enter_fixture(fixture_name);

        TestFixture *fixture = reg->new_fixture();
        assert(nullptr != fixture);

        fixture->register_cases();
        fixture->run_case(_logger, nullptr);

        reg->delete_fixture(fixture);

        _logger->leave_fixture();
    }

    _logger->finish();
}

void TestRunner::run_case(const char *fixture_name, const char *case_name) noexcept
{
    assert(nullptr != fixture_name && nullptr != case_name);

    _logger->start(nullptr, fixture_name, case_name);

    for (TestRegister *reg = *TestRegister::get_link_header();
         nullptr != reg; reg = reg->get_next_register())
    {
        if (0 != ::strcmp(fixture_name, reg->get_fixture_name()))
            continue;

        _logger->enter_fixture(fixture_name);

        TestFixture *fixture = reg->new_fixture();
        assert(nullptr != fixture);

        fixture->register_cases();
        fixture->run_case(_logger, case_name);

        reg->delete_fixture(fixture);

        _logger->leave_fixture();
    }

    _logger->finish();
}

}
