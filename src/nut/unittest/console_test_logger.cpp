
#include <assert.h>
#include <iostream>

#include <nut/util/console_util.h>

#include "console_test_logger.h"

namespace nut
{

void ConsoleTestLogger::start(const char *group_name, const char *fixture_name, const char *case_name)
{
    m_count_of_fixtures = 0;
    m_count_of_failed_fixtures = 0;
    m_current_fixture_failed = false;
    m_count_of_cases = 0;
    m_count_of_failed_cases = 0;
    m_current_case_failed = false;
    m_failures.clear();

    if (NULL != group_name)
    {
        assert(NULL == fixture_name && NULL == case_name);
        std::cout << std::endl << "> run group: ";
        ConsoleUtil::set_text_color(ConsoleUtil::BLUE);
        std::cout << group_name;
        ConsoleUtil::set_text_color();
        std::cout << std::endl << std::endl;
    }
    else if (NULL != case_name)
    {
        assert(NULL != fixture_name);
        std::cout << std::endl << "> run case: ";
        ConsoleUtil::set_text_color(ConsoleUtil::BLUE);
        std::cout << fixture_name << "::" << case_name << "()";
        ConsoleUtil::set_text_color();
        std::cout << std::endl << std::endl;
    }
    else
    {
        assert(NULL != fixture_name);
        std::cout << std::endl << "> run fixture: ";
        ConsoleUtil::set_text_color(ConsoleUtil::BLUE);
        std::cout << fixture_name;
        ConsoleUtil::set_text_color();
        std::cout << std::endl << std::endl;
    }
}

void ConsoleTestLogger::finish()
{
    std::cout << std::endl << "> total fixtures  : ";
    ConsoleUtil::set_text_color(ConsoleUtil::BLUE);
    std::cout << m_count_of_fixtures;
    ConsoleUtil::set_text_color();
    std::cout << std::endl << "> failed fixtures : ";
    ConsoleUtil::set_text_color(0 == m_count_of_failed_fixtures ? ConsoleUtil::GREEN : ConsoleUtil::RED);
    std::cout << m_count_of_failed_fixtures;
    ConsoleUtil::set_text_color();
    std::cout << "  ";
    ConsoleUtil::set_text_color(0 == m_count_of_failed_fixtures ? ConsoleUtil::GREEN : ConsoleUtil::RED);
    std::cout << (0 == m_count_of_failed_fixtures ? "√" : "×");
    ConsoleUtil::set_text_color();
    std::cout << std::endl << "> total cases  : ";
    ConsoleUtil::set_text_color(ConsoleUtil::BLUE);
    std::cout << m_count_of_cases;
    ConsoleUtil::set_text_color();
    std::cout << std::endl << "> failed cases : ";
    ConsoleUtil::set_text_color(0 == m_count_of_failed_cases ? ConsoleUtil::GREEN : ConsoleUtil::RED);
    std::cout << m_count_of_failed_cases;
    ConsoleUtil::set_text_color();
    std::cout << "  ";
    ConsoleUtil::set_text_color(0 == m_count_of_failed_cases ? ConsoleUtil::GREEN : ConsoleUtil::RED);
    std::cout << (0 == m_count_of_failed_cases ? "√" : "×");
    ConsoleUtil::set_text_color();
    std::cout << std::endl;

    if (0 != m_failures.size())
    {
        std::cout << "> failures are followed :" << std::endl << std::endl;

        for (std::vector<TestCaseFailureException>::iterator i = m_failures.begin(); i != m_failures.end(); ++i)
        {
            ConsoleUtil::set_text_color(ConsoleUtil::RED);
            std::cout << i->get_description();
            ConsoleUtil::set_text_color();
            std::cout << std::endl << i->get_file() << " " << i->get_line() << std::endl << std::endl;
        }
    }

    std::cout << std::endl;
}

void ConsoleTestLogger::enter_fixture(const char *fixture_name)
{
    ++m_count_of_fixtures;
    m_current_fixture_failed = false;

    std::cout << "  + " << fixture_name << std::endl;
}

void ConsoleTestLogger::leave_fixture()
{
    if (m_current_fixture_failed)
        ++m_count_of_failed_fixtures;

    std::cout << std::endl;
}

void ConsoleTestLogger::enter_case(const char *case_name)
{
    ++m_count_of_cases;
    m_current_case_failed = false;

    std::cout << "    - " << case_name << " ()";
}

void ConsoleTestLogger::leave_case()
{
    if (m_current_case_failed)
    {
        ++m_count_of_failed_cases;
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

void ConsoleTestLogger::failed_case(const TestCaseFailureException& e)
{
    m_current_case_failed = true;
    m_current_fixture_failed = true;

    m_failures.push_back(e);
}

}
