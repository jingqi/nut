/**
 * @file -
 * @author jingqi
 * @date 2011-11-12 13:15
 */

#ifndef ___HEADFILE___83BCB398_BF24_4F6B_9BA9_6E0CBCDE2791_
#define ___HEADFILE___83BCB398_BF24_4F6B_9BA9_6E0CBCDE2791_

#include <vector>
#include "testcasefailureexception.hpp"

namespace nut
{

class ITestLogger
{
public:
	virtual ~ITestLogger() {}

    virtual void start(const char *groupName, const char *fixtureName, const char *caseName) = 0;
    virtual void finish() = 0;

    virtual void enterFixture(const char *fixtureName) = 0;
    virtual void leaveFixture() = 0;
    virtual void enterCase(const char *caseName) = 0;
    virtual void leaveCase() = 0;
    virtual void failedCase(const TestCaseFailureException& e) = 0;
};

}

#endif /* head file guarder */
