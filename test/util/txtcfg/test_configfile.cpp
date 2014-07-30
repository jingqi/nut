
#include <nut/unittest/unittest.hpp>

#include <nut/util/txtcfg/configfile.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestConfigFile)
{
    NUT_CASES_BEGIN()
        NUT_CASE(testReadString)
        NUT_CASE(testReadNum)
        NUT_CASE(testReadList)
        NUT_CASE(testSetString)
    NUT_CASES_END()

    nut::ref<ConfigFile> pf;

    void setUp()
    {
        pf = gc_new<ConfigFile>("testPropertyFile.prop");
    }

    void tearDown() {}

    void testReadString()
    {
        NUT_TA(pf->getString(NULL, "readString1") == "abc");
        NUT_TA(pf->getString(NULL, "readString2") == "abc");
        NUT_TA(pf->getString(NULL, "read String3") == "ab c");
    }

    void testReadNum()
    {
        NUT_TA(pf->getNum(NULL, "readNum1") == 123);
        NUT_TA(pf->getNum(NULL, "readNum2") == 123);
    }

    void testReadList()
    {
        vector<string> vec = pf->getList(NULL, "readList1");
        NUT_TA(vec.size() == 3);
        NUT_TA(vec[0] == "a");
        NUT_TA(vec[1] == "b");
        NUT_TA(vec[2] == "cd");
    }

    void testSetString()
    {
        pf->setString("a","b", "value");
    }
};

NUT_REGISTER_FIXTURE(TestConfigFile, "util, txtcfg, quiet")

