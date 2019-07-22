
#include <iostream>
#include <nut/unittest/unittest.h>

#include <nut/logging/log_filter.h>


using namespace std;
using namespace nut;

class TestLogFilter : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_parent_mask);
        NUT_REGISTER_CASE(test_reverse_mask);
    }

    void test_parent_mask()
    {
        LogFilter filter;
        filter.allow(nullptr, LL_ALL_LEVELS);
        filter.forbid("a.b", LL_DEBUG | LL_INFO);
        // cout << filter.to_string() << endl;
        NUT_TA(filter.to_string() == "- 1115280.1115281 DI");
        NUT_TA(!filter.is_allowed("a.b.c", LL_DEBUG));
        NUT_TA(filter.is_allowed("a.b.c", LL_FATAL));
        NUT_TA(filter.is_allowed("m.q.c", LL_DEBUG));

        filter.reset();
        NUT_TA(filter.to_string() == "");
        NUT_TA(filter.is_allowed("a.b.c", LL_DEBUG));

        filter.forbid("", LL_ALL_LEVELS);
        filter.allow("a.b", LL_DEBUG | LL_INFO);
        filter.forbid("a.b.c", LL_DEBUG);
        // cout << filter.to_string() << endl;
        NUT_TA(filter.to_string() ==
               "- * DIWEF\n"
               "+ 1115280.1115281 DI\n"
               "- 1115280.1115281.1115282 D"
            );
        NUT_TA(!filter.is_allowed("a.b.c.d", LL_DEBUG));
        NUT_TA(!filter.is_allowed("a.b.c", LL_DEBUG));
        NUT_TA(filter.is_allowed("a.b", LL_DEBUG));
        NUT_TA(!filter.is_allowed("a.b", LL_FATAL));
        NUT_TA(!filter.is_allowed("a", LL_DEBUG));
    }

    void test_reverse_mask()
    {
        LogFilter filter;
        filter.forbid("a.b", LL_DEBUG | LL_INFO | LL_WARN);
        NUT_TA(filter.to_string() == "- 1115280.1115281 DIW");
        NUT_TA(!filter.is_allowed("a.b", LL_INFO));
        filter.allow("a.b", LL_INFO);
        NUT_TA(filter.to_string() ==
               "+ 1115280.1115281 I\n"
               "- 1115280.1115281 DW"
            );
        NUT_TA(filter.is_allowed("a.b", LL_INFO));
        NUT_TA(!filter.is_allowed("a.b", LL_DEBUG));
        NUT_TA(filter.is_allowed("a", LL_DEBUG));
    }
};

NUT_REGISTER_FIXTURE(TestLogFilter, "logging, quiet")
