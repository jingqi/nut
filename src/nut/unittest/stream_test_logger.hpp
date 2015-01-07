/**
 * @file -
 * @author jingqi
 * @date 2011-11-12 14:12
 */

#ifndef ___HEADFILE___EB7AF659_F8F6_4075_AB30_D8F9BC85487E_
#define ___HEADFILE___EB7AF659_F8F6_4075_AB30_D8F9BC85487E_

#include <assert.h>
#include <ostream>
#include <vector>

#include "testlogger.hpp"

namespace nut
{

class StreamTestLogger : public ITestLogger
{
    std::ostream& m_ostream;
    std::vector<TestCaseFailureException> m_failures;

    int m_count_of_fixtures;
    int m_count_of_failed_fixtures;
    bool m_current_fixture_failed;

    int m_count_of_cases;
    int m_count_of_failed_cases;
    bool m_current_case_failed;

public:
    StreamTestLogger(std::ostream& os) : m_ostream(os) {}

    virtual void start(const char *group_name, const char *fixture_name, const char *case_name)
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
            m_ostream << std::endl <<
                "> run group: " << group_name <<
                std::endl << std::endl;
        }
        else if (NULL != case_name)
        {
            assert(NULL != fixture_name);
            m_ostream << std::endl <<
                "> run case: " << fixture_name << "::" << case_name << "()" <<
                std::endl << std::endl;
        }
        else
        {
            assert(NULL != fixture_name);
            m_ostream << std::endl <<
                "> run fixture: " << fixture_name <<
                std::endl << std::endl;
        }
    }

    virtual void finish()
    {
        m_ostream << std::endl <<
            "> total fixtures  : " << m_count_of_fixtures << std::endl <<
            "> failed fixtures : " << m_count_of_failed_fixtures << (0 == m_count_of_failed_fixtures ? "  OK" : "  FAILED") << std::endl <<
            "> total cases  : " << m_count_of_cases << std::endl <<
            "> failed cases : " << m_count_of_failed_cases << (0 == m_count_of_failed_cases ? "  OK" : "  FAILED") << std::endl;

        if (0 != m_failures.size())
        {
            m_ostream << "> failures are followed :" << std::endl << std::endl;

            for (std::vector<TestCaseFailureException>::iterator i = m_failures.begin(); i != m_failures.end(); ++i)
            {
                m_ostream << i->get_description() << std::endl <<
                    i->get_file() << " " << i->get_line() << std::endl << std::endl;
            }
        }

        m_ostream << std::endl;
    }

    virtual void enter_fixture(const char *fixture_name)
    {
        ++m_count_of_fixtures;
        m_current_fixture_failed = false;

        m_ostream << "  + " << fixture_name << std::endl;
    }

    virtual void leave_fixture()
    {
        if (m_current_fixture_failed)
            ++m_count_of_failed_fixtures;

        m_ostream << std::endl;
    }

    virtual void enter_case(const char *case_name)
    {
        ++m_count_of_cases;
        m_current_case_failed = false;

        m_ostream << "    - " << case_name << " ()";
    }

    virtual void leave_case()
    {
        if (m_current_case_failed)
        {
            ++m_count_of_failed_cases;
            m_ostream << "  FAILED" << std::endl;
        }
        else
        {
            m_ostream << "  OK" << std::endl;
        }
    }

    virtual void failed_case(const TestCaseFailureException& e)
    {
        m_current_case_failed = true;
        m_current_fixture_failed = true;

        m_failures.push_back(e);
    }
};

}

#endif /* head file guarder */
