
#include <nut/unittest/unit_test.h>

#include <nut/util/txtcfg/ini_dom.h>
#include <nut/util/txtcfg/txt_file.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestIniDom)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_read_string)
    NUT_CASE(test_read_num)
    NUT_CASE(test_read_list)
    NUT_CASE(test_set_string)
    NUT_CASES_END()

    rc_ptr<IniDom> pf;

    virtual void set_up() override
    {
        pf = rc_new<IniDom>();
        const char *all =
            "readString1=abc\n"
            "readString2=abc # comment\r"
            "read String3  = ab c # comment\r\n"
            "\n\r"
            "readNum1 = 123\n"
            "readNum2 = 123 # com\n"
            "\n"
            "readList1 = a,b,cd\n"
            "[a]\n"
            "b=value\n";
        pf->parse(all);
    }

    void test_read_string()
    {
        NUT_TA(pf->get_string(nullptr, "readString1") == string("abc"));
        NUT_TA(pf->get_string(nullptr, "readString2") == string("abc"));
        NUT_TA(pf->get_string(nullptr, "read String3") == string("ab c"));
    }

    void test_read_num()
    {
        NUT_TA(pf->get_num(nullptr, "readNum1") == 123);
        NUT_TA(pf->get_num(nullptr, "readNum2") == 123);
    }

    void test_read_list()
    {
        vector<string> vec;
        pf->get_list(nullptr, "readList1", &vec);
        NUT_TA(vec.size() == 3);
        NUT_TA(vec[0] == "a");
        NUT_TA(vec[1] == "b");
        NUT_TA(vec[2] == "cd");
    }

    void test_set_string()
    {
        pf->set_string("a","b", "value");
        NUT_TA(pf->get_string("a","b") == string("value"));
    }
};

NUT_REGISTER_FIXTURE(TestIniDom, "util, txtcfg, quiet")
