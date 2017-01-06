
#include <assert.h>
#include <iostream>

#include <nut/util/console_util.h>

#include "console_test_logger.h"

namespace nut
{

ConsoleTestLogger::ConsoleTestLogger()
{
    _in_a_tty = ConsoleUtil::isatty();
}

void ConsoleTestLogger::on_start(const char *group_name, const char *fixture_name, const char *case_name)
{
    std::cout << std::endl;
    if (NULL != group_name)
    {
        assert(NULL == fixture_name && NULL == case_name);
        std::cout << "> run group: ";
        if (_in_a_tty)
            ConsoleUtil::set_text_color(ConsoleUtil::BLUE);
        std::cout << group_name;
        if (_in_a_tty)
            ConsoleUtil::set_text_color();
    }
    else if (NULL != case_name)
    {
        assert(NULL != fixture_name);
        std::cout << "> run case: ";
        if (_in_a_tty)
            ConsoleUtil::set_text_color(ConsoleUtil::BLUE);
        std::cout << fixture_name << "::" << case_name << "()";
        if (_in_a_tty)
            ConsoleUtil::set_text_color();
    }
    else
    {
        assert(NULL != fixture_name);
        std::cout << "> run fixture: ";
        if (_in_a_tty)
            ConsoleUtil::set_text_color(ConsoleUtil::BLUE);
        std::cout << fixture_name;
        if (_in_a_tty)
            ConsoleUtil::set_text_color();
    }
    std::cout << std::endl << std::endl;
}

void ConsoleTestLogger::on_finish()
{
    std::cout << std::endl << "> total fixtures  : ";
    if (_in_a_tty)
        ConsoleUtil::set_text_color(ConsoleUtil::BLUE);
    std::cout << _count_of_fixtures;
    if (_in_a_tty)
        ConsoleUtil::set_text_color();
    std::cout << std::endl << "> failed fixtures : ";
    if (_in_a_tty)
        ConsoleUtil::set_text_color(0 == _count_of_failed_fixtures ? ConsoleUtil::GREEN : ConsoleUtil::RED);
    std::cout << _count_of_failed_fixtures;
    if (_in_a_tty)
        ConsoleUtil::set_text_color();
    std::cout << "  ";
    if (_in_a_tty)
        ConsoleUtil::set_text_color(0 == _count_of_failed_fixtures ? ConsoleUtil::GREEN : ConsoleUtil::RED);
    std::cout << (0 == _count_of_failed_fixtures ? (_in_a_tty ? "√" : "OK") : (_in_a_tty ? "×" : "FAILED"));
    if (_in_a_tty)
        ConsoleUtil::set_text_color();
    std::cout << std::endl << "> total cases  : ";
    if (_in_a_tty)
        ConsoleUtil::set_text_color(ConsoleUtil::BLUE);
    std::cout << _count_of_cases;
    if (_in_a_tty)
        ConsoleUtil::set_text_color();
    std::cout << std::endl << "> failed cases : ";
    if (_in_a_tty)
        ConsoleUtil::set_text_color(0 == _count_of_failed_cases ? ConsoleUtil::GREEN : ConsoleUtil::RED);
    std::cout << _count_of_failed_cases;
    if (_in_a_tty)
        ConsoleUtil::set_text_color();
    std::cout << "  ";
    if (_in_a_tty)
        ConsoleUtil::set_text_color(0 == _count_of_failed_cases ? ConsoleUtil::GREEN : ConsoleUtil::RED);
    std::cout << (0 == _count_of_failed_cases ? (_in_a_tty ? "√" : "OK") : (_in_a_tty ? "×" : "FAILED"));
    if (_in_a_tty)
        ConsoleUtil::set_text_color();
    std::cout << std::endl;

    if (0 != _failures.size())
    {
        std::cout << "> failures are followed :" << std::endl << std::endl;

        for (size_t i = 0, sz = _failures.size(); i < sz; ++i)
        {
            const TestCaseFailureException& failure = _failures.at(i);
            if (_in_a_tty)
                ConsoleUtil::set_text_color(ConsoleUtil::RED);
            std::cout << failure.get_description();
            if (_in_a_tty)
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
        if (_in_a_tty)
            ConsoleUtil::set_text_color(ConsoleUtil::RED);
        std::cout << (_in_a_tty ? "×" : "FAILED");
        if (_in_a_tty)
            ConsoleUtil::set_text_color();
        std::cout << std::endl;
    }
    else
    {
        std::cout << "  ";
        if (_in_a_tty)
            ConsoleUtil::set_text_color(ConsoleUtil::GREEN);
        std::cout << (_in_a_tty ? "√" : "OK");
        if (_in_a_tty)
            ConsoleUtil::set_text_color();
        std::cout << std::endl;
    }
}

}
