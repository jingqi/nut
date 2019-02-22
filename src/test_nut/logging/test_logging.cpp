
#include <nut/unittest/unittest.h>

#include <nut/rc/rc_new.h>
#include <nut/logging/logger.h>
#include <nut/logging/log_handler/console_log_handler.h>
#include <nut/logging/log_handler/stream_log_handler.h>
#include <nut/logging/log_handler/syslog_log_handler.h>


using namespace nut;

class TestLogging : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
        NUT_REGISTER_CASE(test_console_handler);
        NUT_REGISTER_CASE(test_filter);
        NUT_REGISTER_CASE(test_xml_config);
    }

    void test_smoking()
    {
        Logger *l = Logger::get_instance();
        l->clear_handlers();
        l->get_filter().reset();

        l->add_handler(rc_new<StreamLogHandler>(std::cout));

        NUT_LOG_D("a", "debug msg");
        NUT_LOG_I("a.b", "info msg");
        NUT_LOG_W("b.c", "warn msg");
        NUT_LOG_E("a.m", "error msg");
        NUT_LOG_F(nullptr, "fatal msg");

        NUT_LOG_D("a", "debug msg");
        NUT_LOG_I("a.b", "info msg");
        NUT_LOG_W("a.c", "warn msg");
        NUT_LOG_E("b.c", "error msg");
        NUT_LOG_F("a.b.m", "fatal msg");
    }

    void test_console_handler()
    {
        Logger *l = Logger::get_instance();
        l->clear_handlers();
        l->get_filter().reset();

        l->add_handler(rc_new<ConsoleLogHandler>());

        NUT_LOG_D("a", "debug msg");
        NUT_LOG_I("a.b", "info msg");
        NUT_LOG_W("b.c", "warn msg");
        NUT_LOG_E("a.m", "error msg");
        NUT_LOG_F(nullptr, "fatal msg");

        NUT_LOG_D("a", "debug msg");
        NUT_LOG_I("a.b", "info msg");
        NUT_LOG_W("a.c", "warn msg");
        NUT_LOG_E("b.c", "error msg");
        NUT_LOG_F("a.b.m", "fatal msg");
    }

    void test_filter()
    {
        Logger *l = Logger::get_instance();
        l->clear_handlers();
        l->get_filter().reset();

        l->add_handler(rc_new<StreamLogHandler>(std::cout));

        l->get_filter().forbid(nullptr, LL_INFO);
        l->get_filter().forbid("a.b", LL_ERROR | LL_FATAL);
        l->get_filter().allow("a.b", LL_ERROR);

        l->get_filter().forbid("a.b.c.m", LL_FATAL);
        l->get_filter().allow("a.b.c.m", LL_ALL_LEVELS);

        NUT_LOG_D("a", "debug should show");
        NUT_LOG_I("a.b", "info should NOT show----------");
        NUT_LOG_W("a.c", "warn should show");
        NUT_LOG_E("a.b", "error should show");
        NUT_LOG_F("a.b.c.m", "fatal should show");
    }

    void test_xml_config()
    {
        const char *config =
                "<Logger>"
                    "<Filter>"
                        "<Tag forbid=\"info\" />"
                        "<Tag name=\"a.b\" forbid=\"fatal\" />"
                    "</Filter>"
                    "<Handler type=\"stdout\" flush=\"debug,info,warn,error,fatal\" >"
                        "<Filter>"
                            "<Tag forbid=\"error\" />"
                        "</Filter>"
                    "</Handler>"
                    "<Handler type=\"stderr\" flush=\"debug,info,warn,error,fatal\" >"
                        "<Filter>"
                            "<Tag name=\"a\" forbid=\"debug,info,warn\" />"
                        "</Filter>"
                    "</Handler>"
                "</Logger>";
        Logger::get_instance()->load_xml_config(config);

        NUT_LOG_D("a", "debug should show");
        NUT_LOG_I("a.b", "info should NOT show----------");
        NUT_LOG_W("a.c", "warn should show");
        NUT_LOG_E("a.b", "error should show");
        NUT_LOG_F("a.b.c.m", "fatal should NOT show---------");
    }
};

NUT_REGISTER_FIXTURE(TestLogging, "logging")
