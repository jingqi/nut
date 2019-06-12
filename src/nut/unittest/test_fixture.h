
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
        Case(const char *n, case_func_type&& f) noexcept
            : name(n), func(std::forward<case_func_type>(f))
        {}

        Case(const char *n, const case_func_type& f) noexcept
            : name(n), func(f)
        {}

        Case(const Case&) = default;

    public:
        const char *name = nullptr;
        case_func_type func;
    };

public:
    virtual ~TestFixture() = default;

    virtual void register_cases() noexcept = 0;

    void run_case(ITestLogger *logger, const char *case_name) noexcept;

protected:
    virtual void set_up() {}
    virtual void tear_down() {}

    void register_case(const char *name, case_func_type&& func) noexcept;
    void register_case(const char *name, const case_func_type& func) noexcept;

private:
    std::vector<Case> _cases;
};

}

#endif /* head file guarder */
