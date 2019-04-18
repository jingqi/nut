
#include <assert.h>
#include <iostream>

#include "../platform/platform.h"
#include "../util/console_util.h"
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
    if (nullptr != group_name)
    {
        assert(nullptr == fixture_name && nullptr == case_name);
        std::cout << "> run group: ";
        if (_in_a_tty)
            ConsoleUtil::set_text_color(ConsoleColor::Blue);
        std::cout << group_name;
        if (_in_a_tty)
            ConsoleUtil::set_text_color();
    }
    else if (nullptr != case_name)
    {
        assert(nullptr != fixture_name);
        std::cout << "> run case: ";
        if (_in_a_tty)
            ConsoleUtil::set_text_color(ConsoleColor::Blue);
        std::cout << fixture_name << "::" << case_name << "()";
        if (_in_a_tty)
            ConsoleUtil::set_text_color();
    }
    else
    {
        assert(nullptr != fixture_name);
        std::cout << "> run fixture: ";
        if (_in_a_tty)
            ConsoleUtil::set_text_color(ConsoleColor::Blue);
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
        ConsoleUtil::set_text_color(ConsoleColor::Blue);
    std::cout << _count_of_fixtures;
    if (_in_a_tty)
        ConsoleUtil::set_text_color();
    std::cout << std::endl << "> failed fixtures : ";
    if (_in_a_tty)
        ConsoleUtil::set_text_color(0 == _count_of_failed_fixtures ?
                                    ConsoleColor::Green : ConsoleColor::Red);
    std::cout << _count_of_failed_fixtures;
    if (_in_a_tty)
        ConsoleUtil::set_text_color();
    std::cout << "  ";
    if (_in_a_tty)
        ConsoleUtil::set_text_color(0 == _count_of_failed_fixtures ?
                                    ConsoleColor::Green : ConsoleColor::Red);
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_GCC
    std::cout << (0 == _count_of_failed_fixtures ? "OK" : "FAILED");
#else
    std::cout << (0 == _count_of_failed_fixtures ? (_in_a_tty ? "√" : "OK") : (_in_a_tty ? "×" : "FAILED"));
#endif
    if (_in_a_tty)
        ConsoleUtil::set_text_color();
    std::cout << std::endl << "> total cases  : ";
    if (_in_a_tty)
        ConsoleUtil::set_text_color(ConsoleColor::Blue);
    std::cout << _count_of_cases;
    if (_in_a_tty)
        ConsoleUtil::set_text_color();
    std::cout << std::endl << "> failed cases : ";
    if (_in_a_tty)
        ConsoleUtil::set_text_color(0 == _count_of_failed_cases ?
                                    ConsoleColor::Green : ConsoleColor::Red);
    std::cout << _count_of_failed_cases;
    if (_in_a_tty)
        ConsoleUtil::set_text_color();
    std::cout << "  ";
    if (_in_a_tty)
        ConsoleUtil::set_text_color(0 == _count_of_failed_cases ?
                                    ConsoleColor::Green : ConsoleColor::Red);
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_GCC
    std::cout << (0 == _count_of_failed_cases ? "OK" : "FAILED");
#else
    std::cout << (0 == _count_of_failed_cases ? (_in_a_tty ? "√" : "OK") : (_in_a_tty ? "×" : "FAILED"));
#endif
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
                ConsoleUtil::set_text_color(ConsoleColor::Red);
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
    assert(nullptr != fixture_name);
    std::cout << "  + " << fixture_name << std::endl;
}

void ConsoleTestLogger::on_leave_fixture()
{
    std::cout << std::endl;
}

void ConsoleTestLogger::on_enter_case(const char *case_name)
{
    assert(nullptr != case_name);
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
            ConsoleUtil::set_text_color(ConsoleColor::Red);
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_GCC
        std::cout << "FAILED";
#else
        std::cout << (_in_a_tty ? "×" : "FAILED");
#endif
        if (_in_a_tty)
            ConsoleUtil::set_text_color();
        std::cout << std::endl;
    }
    else
    {
        std::cout << "  ";
        if (_in_a_tty)
            ConsoleUtil::set_text_color(ConsoleColor::Green);
#if NUT_PLATFORM_OS_WINDOWS && NUT_PLATFORM_CC_GCC
        std::cout << "OK";
#else
        std::cout << (_in_a_tty ? "√" : "OK");
#endif
        if (_in_a_tty)
            ConsoleUtil::set_text_color();
        std::cout << std::endl;
    }
}

}
