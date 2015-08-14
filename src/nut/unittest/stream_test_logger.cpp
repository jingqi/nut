
#include <assert.h>

#include "stream_test_logger.h"

namespace nut
{

StreamTestLogger::StreamTestLogger(std::ostream *os)
    : _out_stream(os)
{
    assert(NULL != os);
}

void StreamTestLogger::on_start(const char *group_name, const char *fixture_name, const char *case_name)
{
    *_out_stream << std::endl;
    if (NULL != group_name)
    {
        assert(NULL == fixture_name && NULL == case_name);
        *_out_stream << "> run group: " << group_name;
    }
    else if (NULL != case_name)
    {
        assert(NULL != fixture_name);
        *_out_stream << "> run case: " << fixture_name << "::" << case_name << "()";
    }
    else
    {
        assert(NULL != fixture_name);
        *_out_stream << "> run fixture: " << fixture_name;
    }
    *_out_stream << std::endl << std::endl;
}

void StreamTestLogger::on_finish()
{
    *_out_stream << std::endl <<
        "> total fixtures  : " << _count_of_fixtures << std::endl <<
        "> failed fixtures : " << _count_of_failed_fixtures << (0 == _count_of_failed_fixtures ? "  OK" : "  FAILED") << std::endl <<
        "> total cases  : " << _count_of_cases << std::endl <<
        "> failed cases : " << _count_of_failed_cases << (0 == _count_of_failed_cases ? "  OK" : "  FAILED") << std::endl;

    if (0 != _failures.size())
    {
        *_out_stream << "> failures are followed :" << std::endl << std::endl;

        for (size_t i = 0, sz = _failures.size(); i < sz; ++i)
        {
            const TestCaseFailureException& failure = _failures.at(i);
            *_out_stream << failure.get_description() << std::endl <<
                failure.get_file() << " " << failure.get_line() << std::endl << std::endl;
        }
    }

    *_out_stream << std::endl;
}

void StreamTestLogger::on_enter_fixture(const char *fixture_name)
{
    assert(NULL != fixture_name);
    *_out_stream << "  + " << fixture_name << std::endl;
}

void StreamTestLogger::on_leave_fixture()
{
    *_out_stream << std::endl;
}

void StreamTestLogger::on_enter_case(const char *case_name)
{
    assert(NULL != case_name);
    *_out_stream << "    - " << case_name << "()";
}

void StreamTestLogger::on_failed_case(const TestCaseFailureException&)
{}

void StreamTestLogger::on_leave_case()
{
    if (_current_case_failed)
        *_out_stream << "  FAILED";
    else
        *_out_stream << "  OK";
    *_out_stream << std::endl;
}

}
