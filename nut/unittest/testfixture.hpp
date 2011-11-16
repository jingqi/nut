/**
 * @file -
 * @author jingqi
 * @date 2011-11-12 12:30
 */

#ifndef ___HEADFILE___E93561C3_AF61_47A7_A9AB_87CF78307C80_
#define ___HEADFILE___E93561C3_AF61_47A7_A9AB_87CF78307C80_

#include "testlogger.hpp"
#include "testcasefailureexception.hpp"

namespace nut
{

class TestFixture
{
public:
    virtual ~TestFixture() {}

    virtual void setUp() = 0;

    virtual void tearDown() = 0;

private:
    friend class TestRunner;

    virtual int ___runACase(ITestLogger& logger, const int op, const char *caseName) = 0;
};

}

#endif /* head file guarder */
