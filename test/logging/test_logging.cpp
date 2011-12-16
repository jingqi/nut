
#include <nut/unittest/unittest.hpp>

#include <nut/logging/macros.hpp>

using namespace nut;


NUT_FIXTURE(TestLogging)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASES_END()

    void setUp() {}
    void tearDown() {}

    void testSmoking() {
        weak_ref<Logger> l = LogManager::getLogger("");
        l->addHandler(gc_new<ConsoleLogHandler>());
        NUT_LOGGING_DEBUG(l, "debug msg");
        NUT_LOGGING_INFO(l, "info msg");
        NUT_LOGGING_WARN(l, "warn msg");
        NUT_LOGGING_ERROR(l, "error msg");
        NUT_LOGGING_FATAL(l, "fatal msg");
    }
};

NUT_REGISTER_FIXTURE(TestLogging, "logging")
