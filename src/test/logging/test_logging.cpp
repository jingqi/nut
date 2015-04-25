
#include <nut/unittest/unit_test.h>

#include <nut/logging/logger.h>

using namespace nut;


NUT_FIXTURE(TestLogging)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASE(test_filter)
    NUT_CASE(test_config)
    NUT_CASES_END()

    void set_up() {}
    void tear_down() {}

    void test_smoking()
    {
        Logger *l = Logger::get_instance();
        l->clear_handlers();
        l->get_filter().clear_forbids();

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

    void test_filter()
    {
        Logger *l = Logger::get_instance();
        l->clear_handlers();
        l->get_filter().clear_forbids();

        l->add_handler(rc_new<StreamLogHandler>(std::cout));

        l->get_filter().forbid(NULL, LL_INFO);
        l->get_filter().forbid("a.b", LL_ERROR | LL_FATAL);
        l->get_filter().unforbid("a.b", LL_ERROR);

        l->get_filter().forbid("a.b.c.m", LL_FATAL);
        l->get_filter().unforbid("a.b.c.m", LL_ALL_MASK);

        NUT_LOG_D("a", "debug should show");
        NUT_LOG_I("a.b", "info should NOT show----------");
        NUT_LOG_W("a.c", "warn should show");
        NUT_LOG_E("a.b", "error should show");
        NUT_LOG_F("a.b.c.m", "fatal should NOT show---------");
    }

    void test_config()
    {
        const char *config =
                "<Logger>"
                    "<Filter>"
                        "<Tag forbids=\"info\" />"
                        "<Tag name=\"a.b\" forbids=\"fatal\" />"
                    "</Filter>"
                    "<Handler type=\"stdout\" />"
                "</Logger>";
        Logger::get_instance()->load_config(config);

        NUT_LOG_D("a", "debug should show");
        NUT_LOG_I("a.b", "info should NOT show----------");
        NUT_LOG_W("a.c", "warn should show");
        NUT_LOG_E("a.b", "error should show");
        NUT_LOG_F("a.b.c.m", "fatal should NOT show---------");
    }
};

NUT_REGISTER_FIXTURE(TestLogging, "logging")
