
#include <nut/unittest/unittest.hpp>

#include <nut/util/txtcfg/ini_dom.hpp>
#include <nut/util/txtcfg/txt_file.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestIniDom)
{
    NUT_CASES_BEGIN()
        NUT_CASE(testReadString)
        NUT_CASE(testReadNum)
        NUT_CASE(testReadList)
        NUT_CASE(testSetString)
    NUT_CASES_END()

    nut::ref<IniDom> pf;

    void setUp()
    {
		pf = gc_new<IniDom>();
		std::string all;
		TxtFile::read_file("testPropertyFile.prop", &all);
		pf->parse(all);
    }

    void tearDown() {}

    void testReadString()
    {
		NUT_TA(pf->getString(NULL, "readString1") == string("abc"));
		NUT_TA(pf->getString(NULL, "readString2") == string("abc"));
		NUT_TA(pf->getString(NULL, "read String3") == string("ab c"));
    }

    void testReadNum()
    {
        NUT_TA(pf->getNum(NULL, "readNum1") == 123);
        NUT_TA(pf->getNum(NULL, "readNum2") == 123);
    }

    void testReadList()
    {
		vector<string> vec;
		pf->getList(NULL, "readList1", &vec);
        NUT_TA(vec.size() == 3);
        NUT_TA(vec[0] == "a");
        NUT_TA(vec[1] == "b");
        NUT_TA(vec[2] == "cd");
    }

    void testSetString()
    {
        pf->setString("a","b", "value");
		NUT_TA(pf->getString("a","b") == string("value"));
    }
};

NUT_REGISTER_FIXTURE(TestIniDom, "util, txtcfg, quiet")

