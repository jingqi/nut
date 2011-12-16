/**
 * @file -
 * @author jingqi
 * @date 2011-11-12 13:17
 */

#ifndef ___HEADFILE___C163E413_5234_4AC7_B0E6_1D85282E4D59_
#define ___HEADFILE___C163E413_5234_4AC7_B0E6_1D85282E4D59_

#include "testlogger.hpp"
#include "testregister.hpp"

namespace nut
{

class TestRunner
{
    ITestLogger &m_logger;

public :
    TestRunner(ITestLogger& logger) : m_logger(logger) {}

    void runAGroup (const char *groupName)
    {
        m_logger.start(groupName, NULL, NULL);
        
        TestRegister *pRegister = nut_get_register_header();
        while (NULL != pRegister)
        {
            if (pRegister->matchGroup(groupName))
            {
                m_logger.enterFixture(pRegister->getFixtureName());
                
                TestFixture *fixture = pRegister->newFixture();
                assert(NULL != fixture);
                int total = fixture->___runACase(m_logger, -2, NULL);

                for (int i = 0; i < total; ++i) {
                    if (NULL == fixture)
                        fixture = pRegister->newFixture();
                    fixture->___runACase(m_logger, i, NULL);
                    pRegister->deleteFixture(fixture);
                    fixture = NULL;
                }
                if (NULL != fixture)
                    pRegister->deleteFixture(fixture);

                m_logger.leaveFixture();
            }
            pRegister = pRegister->getNextRegister();
        }

        m_logger.finish();
    }
    
    void runAFixture (const char *fixtureName)
    {
        m_logger.start(NULL, fixtureName, NULL);
        
        TestRegister *pRegister = nut_get_register_header();
        while (NULL != pRegister)
        {
            if (0 == ::strcmp(fixtureName, pRegister->getFixtureName()))
            {
                m_logger.enterFixture(fixtureName);

                TestFixture *fixture = pRegister->newFixture();
                assert(NULL != fixture);
                int total = fixture->___runACase(m_logger, -2, NULL);

                for (int i = 0; i < total; ++i) {
                    if (NULL == fixture)
                        fixture = pRegister->newFixture();
                    fixture->___runACase(m_logger, i, NULL);
                    pRegister->deleteFixture(fixture);
                    fixture = NULL;
                }
                if (NULL != fixture)
                    pRegister->deleteFixture(fixture);

                m_logger.leaveFixture();
                break;
            }
            pRegister = pRegister->getNextRegister();
        }

        m_logger.finish();
    }
    
    void runACase (const char *fixtureName, const char *caseName)
    {
        m_logger.start(NULL, fixtureName, caseName);
        
        TestRegister *pRegister = nut_get_register_header();
        while (NULL != pRegister)
        {
            if (0 == ::strcmp(fixtureName, pRegister->getFixtureName()))
            {
                m_logger.enterFixture(fixtureName);
                
                TestFixture *fixture = pRegister->newFixture();
                fixture->___runACase(m_logger, -1, caseName);
                pRegister->deleteFixture(fixture);

                m_logger.leaveFixture();
                break;
            }
            pRegister = pRegister->getNextRegister();
        }

        m_logger.finish();
    }
};


}

#endif /* head file guarder */
