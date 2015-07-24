
#include <assert.h>

#include "stream_test_logger.h"

namespace nut
{

StreamTestLogger::StreamTestLogger(std::ostream& os)
    : _out_stream(os)
{}

void StreamTestLogger::start(const char *group_name, const char *fixture_name, const char *case_name)
{
    _count_of_fixtures = 0;
    _count_of_failed_fixtures = 0;
    _current_fixture_failed = false;
    _count_of_cases = 0;
    _count_of_failed_cases = 0;
    _current_case_failed = false;
    _failures.clear();

    if (NULL != group_name)
    {
        assert(NULL == fixture_name && NULL == case_name);
        _out_stream << std::endl <<
            "> run group: " << group_name <<
            std::endl << std::endl;
    }
    else if (NULL != case_name)
    {
        assert(NULL != fixture_name);
        _out_stream << std::endl <<
            "> run case: " << fixture_name << "::" << case_name << "()" <<
            std::endl << std::endl;
    }
    else
    {
        assert(NULL != fixture_name);
        _out_stream << std::endl <<
            "> run fixture: " << fixture_name <<
            std::endl << std::endl;
    }
}

void StreamTestLogger::finish()
{
    _out_stream << std::endl <<
        "> total fixtures  : " << _count_of_fixtures << std::endl <<
        "> failed fixtures : " << _count_of_failed_fixtures << (0 == _count_of_failed_fixtures ? "  OK" : "  FAILED") << std::endl <<
        "> total cases  : " << _count_of_cases << std::endl <<
        "> failed cases : " << _count_of_failed_cases << (0 == _count_of_failed_cases ? "  OK" : "  FAILED") << std::endl;

    if (0 != _failures.size())
    {
        _out_stream << "> failures are followed :" << std::endl << std::endl;

        for (std::vector<TestCaseFailureException>::iterator i = _failures.begin(); i != _failures.end(); ++i)
        {
            _out_stream << i->get_description() << std::endl <<
                i->get_file() << " " << i->get_line() << std::endl << std::endl;
        }
    }

    _out_stream << std::endl;
}

void StreamTestLogger::enter_fixture(const char *fixture_name)
{
    ++_count_of_fixtures;
    _current_fixture_failed = false;

    _out_stream << "  + " << fixture_name << std::endl;
}

void StreamTestLogger::leave_fixture()
{
    if (_current_fixture_failed)
        ++_count_of_failed_fixtures;

    _out_stream << std::endl;
}

void StreamTestLogger::enter_case(const char *case_name)
{
    ++_count_of_cases;
    _current_case_failed = false;

    _out_stream << "    - " << case_name << " ()";
}

void StreamTestLogger::leave_case()
{
    if (_current_case_failed)
    {
        ++_count_of_failed_cases;
        _out_stream << "  FAILED" << std::endl;
    }
    else
    {
        _out_stream << "  OK" << std::endl;
    }
}

void StreamTestLogger::failed_case(const TestCaseFailureException& e)
{
    _current_case_failed = true;
    _current_fixture_failed = true;

    _failures.push_back(e);
}

}
