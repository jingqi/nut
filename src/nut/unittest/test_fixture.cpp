
#include <assert.h>
#include <string.h> // for strcmp()

#include "test_fixture.h"


namespace nut
{

void TestFixture::run_case(ITestLogger *logger, const char *case_name)
{
    assert(nullptr != logger);

    for (size_t i = 0, sz = _cases.size(); i < sz; ++i)
    {
        Case& c = _cases.at(i);
        if (nullptr != case_name && 0 != ::strcmp(c.name, case_name))
            continue;

        logger->enter_case(c.name);
        try
        {
            set_up();
            c.func();
            tear_down();
        }
        catch (nut::TestCaseFailureException& e)
        {
            logger->failed_case(e);
        }
        catch (...)
        {
            nut::TestCaseFailureException e("Unhandled exception",
                                            __FILE__, __LINE__);
            logger->failed_case(e);
        }
        logger->leave_case();
    }
}

void TestFixture::register_case(const char *name, case_func_type&& func)
{
    assert(nullptr != name);
    _cases.emplace_back(name, std::forward<case_func_type>(func));
}

}
