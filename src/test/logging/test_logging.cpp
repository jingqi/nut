
#include <nut/unittest/unit_test.h>

#include <nut/logging/macros.h>
#include <nut/logging/log_handler.h>

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
        l->add_handler(rc_new<StreamLogHandler>(std::cout));

        NUT_LOGGING_DEBUG(l, "debug msg");
        NUT_LOGGING_INFO(l, "info msg");
        NUT_LOGGING_WARN(l, "warn msg");
        NUT_LOGGING_ERROR(l, "error msg");
        NUT_LOGGING_FATAL(l, "fatal msg");

        NUT_LOG_D("a", "debug msg");
        NUT_LOG_I("a.b", "info msg");
        NUT_LOG_W("a.c", "warn msg");
        NUT_LOG_E("b.c", "error msg");
        NUT_LOG_F("a.b.m", "fatal msg");
    }
};

NUT_REGISTER_FIXTURE(TestLogging, "logging")
