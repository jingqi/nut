
#include <nut/unittest/unittest.hpp>

#include <nut/logging/macros.hpp>

using namespace nut;


NUT_FIXTURE(TestLogging)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void set_up() {}
    void tear_down() {}

    void test_smoking()
    {
        Logger *l = LogManager::get_logger("");
        l->add_handler(RC_NEW(NULL, ConsoleLogHandler));
        NUT_LOGGING_DEBUG(l, "debug msg");
        NUT_LOGGING_INFO(l, "info msg");
        NUT_LOGGING_WARN(l, "warn msg");
        NUT_LOGGING_ERROR(l, "error msg");
        NUT_LOGGING_FATAL(l, "fatal msg");
    }
};

NUT_REGISTER_FIXTURE(TestLogging, "logging")
