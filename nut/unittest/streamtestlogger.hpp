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

    int m_countOfFixtures;
    int m_countOfFailedFixtures;
    bool m_currentFixtureFailed;

    int m_countOfCases;
    int m_countOfFailedCases;
    bool m_currentCaseFailed;

public:
    StreamTestLogger(std::ostream& os) : m_ostream(os) {}

    virtual void start(const char *groupName, const char *fixtureName, const char *caseName)
    {
        m_countOfFixtures = 0;
        m_countOfFailedFixtures = 0;
        m_currentFixtureFailed = false;
        m_countOfCases = 0;
        m_countOfFailedCases = 0;
        m_currentCaseFailed = false;
        m_failures.clear();

        if (NULL != groupName)
        {
            assert(NULL == fixtureName && NULL == caseName);
            m_ostream << std::endl <<
                "> run group: " << groupName <<
                std::endl << std::endl;
        }
        else if (NULL != caseName)
        {
            assert(NULL != fixtureName);
            m_ostream << std::endl <<
                "> run case: " << fixtureName << "::" << caseName << "()" <<
                std::endl << std::endl;
        }
        else
        {
            assert(NULL != fixtureName);
            m_ostream << std::endl <<
                "> run fixture: " << fixtureName <<
                std::endl << std::endl;
        }
    }

    virtual void finish()
    {
        m_ostream << std::endl <<
            "> total fixtures  : " << m_countOfFixtures << std::endl <<
            "> failed fixtures : " << m_countOfFailedFixtures << (0 == m_countOfFailedFixtures ? "  √" : "  ×") << std::endl <<
            "> total cases  : " << m_countOfCases << std::endl <<
            "> failed cases : " << m_countOfFailedCases << (0 == m_countOfFailedCases ? "  √" : "  ×") << std::endl;

        if (0 != m_failures.size())
        {
            m_ostream << "> failures are followed :" << std::endl << std::endl;

            for (std::vector<TestCaseFailureException>::iterator i = m_failures.begin(); i != m_failures.end(); ++i)
            {
                m_ostream << i->getDescription() << std::endl <<
                    i->getFile() << " " << i->getLine() << std::endl << std::endl;
            }
        }

        m_ostream << std::endl;
    }

    virtual void enterFixture(const char *fixtureName)
    {
        ++m_countOfFixtures;
        m_currentFixtureFailed = false;

        m_ostream << "  + " << fixtureName << std::endl;
    }

    virtual void leaveFixture()
    {
        if (m_currentFixtureFailed)
            ++m_countOfFailedFixtures;

        m_ostream << std::endl;
    }

    virtual void enterCase(const char *caseName)
    {
        ++m_countOfCases;
        m_currentCaseFailed = false;

        m_ostream << "    - " << caseName << " ()";
    }

    virtual void leaveCase()
    {
        if (m_currentCaseFailed)
        {
            ++m_countOfFailedCases;
            m_ostream << "  ×" << std::endl;
        }
        else
        {
            m_ostream << "  √" << std::endl;
        }
    }

    virtual void failedCase(const TestCaseFailureException& e)
    {
        m_currentCaseFailed = true;
        m_currentFixtureFailed = true;

        m_failures.push_back(e);
    }
};

}

#endif /* head file guarder */
