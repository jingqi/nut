/**
 * @file -
 * @author jingqi
 * @date 2011-11-12 13:15
 */

#ifndef ___HEADFILE___83BCB398_BF24_4F6B_9BA9_6E0CBCDE2791_
#define ___HEADFILE___83BCB398_BF24_4F6B_9BA9_6E0CBCDE2791_

#include <vector>

#include "testcase_failure_exception.hpp"

namespace nut
{

class ITestLogger
{
public:
	virtual ~ITestLogger() {}

    virtual void start(const char *group_name, const char *fixture_name, const char *case_name) = 0;
    virtual void finish() = 0;

    virtual void enter_fixture(const char *fixture_name) = 0;
    virtual void leave_fixture() = 0;
    virtual void enter_case(const char *case_name) = 0;
    virtual void leave_case() = 0;
    virtual void failed_case(const TestCaseFailureException& e) = 0;
};

}

#endif /* head file guarder */
