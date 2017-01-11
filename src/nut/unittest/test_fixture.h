
#ifndef ___HEADFILE___E93561C3_AF61_47A7_A9AB_87CF78307C80_
#define ___HEADFILE___E93561C3_AF61_47A7_A9AB_87CF78307C80_

#include "test_logger.h"
#include "testcase_failure_exception.h"

namespace nut
{

class TestFixture
{
public:
    virtual ~TestFixture() = default;

    virtual void set_up()
    {}

    virtual void tear_down()
    {}

private:
    friend class TestRunner;

    virtual int ___run_case(ITestLogger *logger, const int op, const char *case_name) = 0;
};

}

#endif /* head file guarder */
