/**
 * @file -
 * @author jingqi
 * @data 2011-12-24
 * @last-edit 2012-03-25 22:42:57 jingqi
 */

#ifndef ___HEADFILE_ABC2859B_32EC_4725_A95F_A4EA1F6DD526_
#define ___HEADFILE_ABC2859B_32EC_4725_A95F_A4EA1F6DD526_

#include <assert.h>
#include <iostream>
#include <vector>

#include <nut/util/consoleutil.hpp>

#include "testlogger.hpp"

namespace nut
{

class ConsoleTestLogger : public ITestLogger
{
    std::vector<TestCaseFailureException> m_failures;

    int m_countOfFixtures;
    int m_countOfFailedFixtures;
    bool m_currentFixtureFailed;

    int m_countOfCases;
    int m_countOfFailedCases;
    bool m_currentCaseFailed;

public:
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
            std::cout << std::endl << "> run group: ";
            ConsoleUtil::setTextColor(ConsoleUtil::BLUE);
            std::cout << groupName;
            ConsoleUtil::setTextColor();
            std::cout << std::endl << std::endl;
        }
        else if (NULL != caseName)
        {
            assert(NULL != fixtureName);
            std::cout << std::endl << "> run case: ";
            ConsoleUtil::setTextColor(ConsoleUtil::BLUE);
            std::cout << fixtureName << "::" << caseName << "()";
            ConsoleUtil::setTextColor();
            std::cout << std::endl << std::endl;
        }
        else
        {
            assert(NULL != fixtureName);
            std::cout << std::endl << "> run fixture: ";
            ConsoleUtil::setTextColor(ConsoleUtil::BLUE);
            std::cout << fixtureName;
            ConsoleUtil::setTextColor();
            std::cout << std::endl << std::endl;
        }
    }

    virtual void finish()
    {
        std::cout << std::endl << "> total fixtures  : ";
        ConsoleUtil::setTextColor(ConsoleUtil::BLUE);
        std::cout << m_countOfFixtures;
        ConsoleUtil::setTextColor();
        std::cout << std::endl << "> failed fixtures : ";
        ConsoleUtil::setTextColor(0 == m_countOfFailedFixtures ? ConsoleUtil::GREEN : ConsoleUtil::RED);
        std::cout << m_countOfFailedFixtures;
        ConsoleUtil::setTextColor();
        std::cout << "  ";
        ConsoleUtil::setTextColor(0 == m_countOfFailedFixtures ? ConsoleUtil::GREEN : ConsoleUtil::RED);
        std::cout << (0 == m_countOfFailedFixtures ? "√" : "×");
        ConsoleUtil::setTextColor();
        std::cout << std::endl << "> total cases  : ";
        ConsoleUtil::setTextColor(ConsoleUtil::BLUE);
        std::cout << m_countOfCases;
        ConsoleUtil::setTextColor();
        std::cout << std::endl << "> failed cases : ";
        ConsoleUtil::setTextColor(0 == m_countOfFailedCases ? ConsoleUtil::GREEN : ConsoleUtil::RED);
        std::cout << m_countOfFailedCases;
        ConsoleUtil::setTextColor();
        std::cout << "  ";
        ConsoleUtil::setTextColor(0 == m_countOfFailedCases ? ConsoleUtil::GREEN : ConsoleUtil::RED);
        std::cout << (0 == m_countOfFailedCases ? "√" : "×");
        ConsoleUtil::setTextColor();
        std::cout << std::endl;

        if (0 != m_failures.size())
        {
            std::cout << "> failures are followed :" << std::endl << std::endl;

            for (std::vector<TestCaseFailureException>::iterator i = m_failures.begin(); i != m_failures.end(); ++i)
            {
                ConsoleUtil::setTextColor(ConsoleUtil::RED);
                std::cout << i->getDescription();
                ConsoleUtil::setTextColor();
                std::cout << std::endl << i->getFile() << " " << i->getLine() << std::endl << std::endl;
            }
        }

        std::cout << std::endl;
    }

    virtual void enterFixture(const char *fixtureName)
    {
        ++m_countOfFixtures;
        m_currentFixtureFailed = false;

        std::cout << "  + " << fixtureName << std::endl;
    }

    virtual void leaveFixture()
    {
        if (m_currentFixtureFailed)
            ++m_countOfFailedFixtures;

        std::cout << std::endl;
    }

    virtual void enterCase(const char *caseName)
    {
        ++m_countOfCases;
        m_currentCaseFailed = false;

        std::cout << "    - " << caseName << " ()";
    }

    virtual void leaveCase()
    {
        if (m_currentCaseFailed)
        {
            ++m_countOfFailedCases;
            std::cout << "  ";
            ConsoleUtil::setTextColor(ConsoleUtil::RED);
            std::cout << "×";
            ConsoleUtil::setTextColor();
            std::cout << std::endl;
        }
        else
        {
            std::cout << "  ";
            ConsoleUtil::setTextColor(ConsoleUtil::GREEN);
            std::cout << "√";
            ConsoleUtil::setTextColor();
            std::cout << std::endl;
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

#endif

