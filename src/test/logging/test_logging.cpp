
#include <nut/unittest/unit_test.h>

#include <nut/logging/logger.h>

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
        Logger *l = Logger::get_instance();
        l->add_handler(rc_new<StreamLogHandler>(std::cout));

        NUT_LOG_D("a", "debug msg");
        NUT_LOG_I("a.b", "info msg");
        NUT_LOG_W("b.c", "warn msg");
        NUT_LOG_E("a.m", "error msg");
        NUT_LOG_F(NULL, "fatal msg");

        NUT_LOG_D("a", "debug msg");
        NUT_LOG_I("a.b", "info msg");
        NUT_LOG_W("a.c", "warn msg");
        NUT_LOG_E("b.c", "error msg");
        NUT_LOG_F("a.b.m", "fatal msg");
    }
};

NUT_REGISTER_FIXTURE(TestLogging, "logging")
