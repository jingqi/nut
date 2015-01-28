
#include <nut/unittest/unittest.hpp>

#include <nut/util/txtcfg/property_dom.hpp>
#include <nut/util/txtcfg/txt_file.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestPropertyDom)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_read_string)
    NUT_CASE(test_read_num)
    NUT_CASE(test_read_list)
    NUT_CASES_END()

    nut::ref<PropertyDom> pf;

    void set_up()
    {
        pf = GC_NEW(NULL, PropertyDom);
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

    void tear_down() {}

    void test_read_string()
    {
        NUT_TA(pf->get_string("readString1") == string("abc"));
        NUT_TA(pf->get_string("readString2") == string("abc"));
        NUT_TA(pf->get_string("read String3") == string("ab c"));
    }

    void test_read_num()
    {
        NUT_TA(pf->get_num("readNum1") == 123);
        NUT_TA(pf->get_num("readNum2") == 123);
    }

    void test_read_list()
    {
		vector<string> vec;
        pf->get_list("readList1", &vec);
        NUT_TA(vec.size() == 3);
        NUT_TA(vec[0] == "a");
        NUT_TA(vec[1] == "b");
        NUT_TA(vec[2] == "cd");
    }
};

NUT_REGISTER_FIXTURE(TestPropertyDom, "util, txtcfg, quiet")
