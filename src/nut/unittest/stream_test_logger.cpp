
#include <assert.h>

#include "stream_test_logger.h"


namespace nut
{

StreamTestLogger::StreamTestLogger(std::ostream *os) noexcept
    : _out_stream(os)
{
    assert(nullptr != os);
}

void StreamTestLogger::on_start(const char *group_name, const char *fixture_name,
                                const char *case_name) noexcept
{
    *_out_stream << std::endl;
    if (nullptr != group_name)
    {
        assert(nullptr == fixture_name && nullptr == case_name);
        *_out_stream << "> run group: " << group_name;
    }
    else if (nullptr != case_name)
    {
        assert(nullptr != fixture_name);
        *_out_stream << "> run case: " << fixture_name << "::" << case_name << "()";
    }
    else
    {
        assert(nullptr != fixture_name);
        *_out_stream << "> run fixture: " << fixture_name;
    }
    *_out_stream << std::endl << std::endl;
}

void StreamTestLogger::on_finish() noexcept
{
    *_out_stream << std::endl <<
        "> total fixtures  : " << _count_of_fixtures << std::endl <<
        "> failed fixtures : " << _count_of_failed_fixtures <<
        (0 == _count_of_failed_fixtures ? "  OK" : "  FAILED") << std::endl <<
        "> total cases  : " << _count_of_cases << std::endl <<
        "> failed cases : " << _count_of_failed_cases <<
        (0 == _count_of_failed_cases ? "  OK" : "  FAILED") << std::endl;

    if (0 != _failures.size())
    {
        *_out_stream << "> failures are followed :" << std::endl << std::endl;

        for (size_t i = 0, sz = _failures.size(); i < sz; ++i)
        {
            const TestCaseFailureException& failure = _failures.at(i);
            *_out_stream << failure.get_message() << std::endl <<
                failure.get_source_path() << " " << failure.get_source_line() <<
                std::endl << std::endl;
        }
    }

    *_out_stream << std::endl;
}

void StreamTestLogger::on_enter_fixture(const char *fixture_name) noexcept
{
    assert(nullptr != fixture_name);
    *_out_stream << "  + " << fixture_name << std::endl;
}

void StreamTestLogger::on_leave_fixture() noexcept
{
    *_out_stream << std::endl;
}

void StreamTestLogger::on_enter_case(const char *case_name) noexcept
{
    assert(nullptr != case_name);
    *_out_stream << "    - " << case_name << "()";
}

void StreamTestLogger::on_failed_case(const TestCaseFailureException&) noexcept
{}

void StreamTestLogger::on_leave_case() noexcept
{
    if (_current_case_failed)
        *_out_stream << "  FAILED";
    else
        *_out_stream << "  OK";
    *_out_stream << std::endl;
}

}
