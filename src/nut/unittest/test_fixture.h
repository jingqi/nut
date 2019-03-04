
#ifndef ___HEADFILE___E93561C3_AF61_47A7_A9AB_87CF78307C80_
#define ___HEADFILE___E93561C3_AF61_47A7_A9AB_87CF78307C80_

#include <vector>
#include <functional>

#include "../nut_config.h"
#include "test_logger.h"
#include "testcase_failure_exception.h"


namespace nut
{

class NUT_API TestFixture
{
protected:
    typedef std::function<void()> case_func_type;

private:
    class Case
    {
    public:
        Case(const char *n, case_func_type&& f)
            : name(n), func(f)
        {}

        Case(const Case&) = default;

    public:
        const char *name = nullptr;
        case_func_type func;
    };

public:
    virtual ~TestFixture() = default;

    virtual void register_cases() = 0;

    void run_case(ITestLogger *logger, const char *case_name);

protected:
    virtual void set_up() {}
    virtual void tear_down() {}

    void register_case(const char *name, case_func_type&& func);

private:
    std::vector<Case> _cases;
};

}

#endif /* head file guarder */
