
#include <assert.h>
#include <string.h> // for strcmp()

#include "../debugging/exception.h"
#include "test_fixture.h"


namespace nut
{

void TestFixture::run_case(ITestLogger *logger, const char *case_name) noexcept
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
        catch (nut::Exception& e)
        {
            std::string msg("Unhandled nut::Exception: ");
            msg += e.get_message();
            nut::TestCaseFailureException ee(std::move(msg), e.get_source_path(), e.get_source_line());
            logger->failed_case(ee);
        }
        catch (std::exception& e)
        {
            std::string msg("Unhandled std::exception: ");
            msg += e.what();
            nut::TestCaseFailureException ee(std::move(msg), __FILE__, __LINE__);
            logger->failed_case(ee);
        }
        catch (...)
        {
            nut::TestCaseFailureException e("Unhandled exception", __FILE__, __LINE__);
            logger->failed_case(e);
        }
        logger->leave_case();
    }
}

void TestFixture::register_case(const char *name, case_func_type&& func) noexcept
{
    assert(nullptr != name);
    _cases.emplace_back(name, std::forward<case_func_type>(func));
}

void TestFixture::register_case(const char *name, const case_func_type& func) noexcept
{
    assert(nullptr != name);
    _cases.emplace_back(name, func);
}

}
