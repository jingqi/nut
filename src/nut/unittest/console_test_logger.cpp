
#include <assert.h>
#include <iostream>

#include <nut/util/console_util.h>

#include "console_test_logger.h"

namespace nut
{

void ConsoleTestLogger::on_start(const char *group_name, const char *fixture_name, const char *case_name)
{
    std::cout << std::endl;
    if (NULL != group_name)
    {
        assert(NULL == fixture_name && NULL == case_name);
        std::cout << "> run group: ";
        ConsoleUtil::set_text_color(ConsoleUtil::BLUE);
        std::cout << group_name;
        ConsoleUtil::set_text_color();
    }
    else if (NULL != case_name)
    {
        assert(NULL != fixture_name);
        std::cout << "> run case: ";
        ConsoleUtil::set_text_color(ConsoleUtil::BLUE);
        std::cout << fixture_name << "::" << case_name << "()";
        ConsoleUtil::set_text_color();
    }
    else
    {
        assert(NULL != fixture_name);
        std::cout << "> run fixture: ";
        ConsoleUtil::set_text_color(ConsoleUtil::BLUE);
        std::cout << fixture_name;
        ConsoleUtil::set_text_color();
    }
    std::cout << std::endl << std::endl;
}

void ConsoleTestLogger::on_finish()
{
    std::cout << std::endl << "> total fixtures  : ";
    ConsoleUtil::set_text_color(ConsoleUtil::BLUE);
    std::cout << _count_of_fixtures;
    ConsoleUtil::set_text_color();
    std::cout << std::endl << "> failed fixtures : ";
    ConsoleUtil::set_text_color(0 == _count_of_failed_fixtures ? ConsoleUtil::GREEN : ConsoleUtil::RED);
    std::cout << _count_of_failed_fixtures;
    ConsoleUtil::set_text_color();
    std::cout << "  ";
    ConsoleUtil::set_text_color(0 == _count_of_failed_fixtures ? ConsoleUtil::GREEN : ConsoleUtil::RED);
    std::cout << (0 == _count_of_failed_fixtures ? "√" : "×");
    ConsoleUtil::set_text_color();
    std::cout << std::endl << "> total cases  : ";
    ConsoleUtil::set_text_color(ConsoleUtil::BLUE);
    std::cout << _count_of_cases;
    ConsoleUtil::set_text_color();
    std::cout << std::endl << "> failed cases : ";
    ConsoleUtil::set_text_color(0 == _count_of_failed_cases ? ConsoleUtil::GREEN : ConsoleUtil::RED);
    std::cout << _count_of_failed_cases;
    ConsoleUtil::set_text_color();
    std::cout << "  ";
    ConsoleUtil::set_text_color(0 == _count_of_failed_cases ? ConsoleUtil::GREEN : ConsoleUtil::RED);
    std::cout << (0 == _count_of_failed_cases ? "√" : "×");
    ConsoleUtil::set_text_color();
    std::cout << std::endl;

    if (0 != _failures.size())
    {
        std::cout << "> failures are followed :" << std::endl << std::endl;

        for (size_t i = 0, sz = _failures.size(); i < sz; ++i)
        {
            const TestCaseFailureException& failure = _failures.at(i);
            ConsoleUtil::set_text_color(ConsoleUtil::RED);
            std::cout << failure.get_description();
            ConsoleUtil::set_text_color();
            std::cout << std::endl << failure.get_file() << " " << failure.get_line() << std::endl << std::endl;
        }
    }

    std::cout << std::endl;
}

void ConsoleTestLogger::on_enter_fixture(const char *fixture_name)
{
    assert(NULL != fixture_name);
    std::cout << "  + " << fixture_name << std::endl;
}

void ConsoleTestLogger::on_leave_fixture()
{
    std::cout << std::endl;
}

void ConsoleTestLogger::on_enter_case(const char *case_name)
{
    assert(NULL != case_name);
    std::cout << "    - " << case_name << "()";
}

void ConsoleTestLogger::on_failed_case(const TestCaseFailureException&)
{}

void ConsoleTestLogger::on_leave_case()
{
    if (_current_case_failed)
    {
        std::cout << "  ";
        ConsoleUtil::set_text_color(ConsoleUtil::RED);
        std::cout << "×";
        ConsoleUtil::set_text_color();
        std::cout << std::endl;
    }
    else
    {
        std::cout << "  ";
        ConsoleUtil::set_text_color(ConsoleUtil::GREEN);
        std::cout << "√";
        ConsoleUtil::set_text_color();
        std::cout << std::endl;
    }
}

}
