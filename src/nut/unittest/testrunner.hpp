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

public:
    TestRunner(ITestLogger& logger) : m_logger(logger) {}

    void run_a_group(const char *group_name)
    {
        m_logger.start(group_name, NULL, NULL);

        TestRegister *pRegister = *(TestRegister**)nut_get_register_header();
        while (NULL != pRegister)
        {
            if (pRegister->match_group(group_name))
            {
                m_logger.enter_fixture(pRegister->get_fixture_name());

                TestFixture *fixture = pRegister->new_fixture();
                assert(NULL != fixture);
                int total = fixture->___run_a_case(m_logger, -2, NULL);

                for (int i = 0; i < total; ++i) {
                    if (NULL == fixture)
                        fixture = pRegister->new_fixture();
                    fixture->___run_a_case(m_logger, i, NULL);
                    pRegister->delete_fixture(fixture);
                    fixture = NULL;
                }
                if (NULL != fixture)
                    pRegister->delete_fixture(fixture);

                m_logger.leave_fixture();
            }
            pRegister = pRegister->get_next_register();
        }

        m_logger.finish();
    }

    void run_a_fixture(const char *fixture_name)
    {
        m_logger.start(NULL, fixture_name, NULL);

        TestRegister *pRegister = *(TestRegister**)nut_get_register_header();
        while (NULL != pRegister)
        {
            if (0 == ::strcmp(fixture_name, pRegister->get_fixture_name()))
            {
                m_logger.enter_fixture(fixture_name);

                TestFixture *fixture = pRegister->new_fixture();
                assert(NULL != fixture);
                int total = fixture->___run_a_case(m_logger, -2, NULL);

                for (int i = 0; i < total; ++i) {
                    if (NULL == fixture)
                        fixture = pRegister->new_fixture();
                    fixture->___run_a_case(m_logger, i, NULL);
                    pRegister->delete_fixture(fixture);
                    fixture = NULL;
                }
                if (NULL != fixture)
                    pRegister->delete_fixture(fixture);

                m_logger.leave_fixture();
                break;
            }
            pRegister = pRegister->get_next_register();
        }

        m_logger.finish();
    }

    void run_a_case(const char *fixture_name, const char *case_name)
    {
        m_logger.start(NULL, fixture_name, case_name);

        TestRegister *pRegister = *(TestRegister**)nut_get_register_header();
        while (NULL != pRegister)
        {
            if (0 == ::strcmp(fixture_name, pRegister->get_fixture_name()))
            {
                m_logger.enter_fixture(fixture_name);

                TestFixture *fixture = pRegister->new_fixture();
                fixture->___run_a_case(m_logger, -1, case_name);
                pRegister->delete_fixture(fixture);

                m_logger.leave_fixture();
                break;
            }
            pRegister = pRegister->get_next_register();
        }

        m_logger.finish();
    }
};


}

#endif /* head file guarder */
