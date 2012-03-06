
#include <nut/unittest/unittest.hpp>

#include <nut/util/propertyfile.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestPropertyFile)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testReadString)
    NUT_CASE(testReadNum)
    NUT_CASE(testReadList)
    NUT_CASES_END()

    ref<PropertyFile> pf;

    void setUp()
    {
         pf = gc_new<PropertyFile>("testPropertyFile.prop");
    }

    void tearDown() {}

    void testReadString()
    {
        NUT_TA(pf->getString("readString1") == "abc");
        NUT_TA(pf->getString("readString2") == "abc");
        NUT_TA(pf->getString("read String3") == "ab c");
    }

    void testReadNum()
    {
        NUT_TA(pf->getNum("readNum1") == 123);
        NUT_TA(pf->getNum("readNum2") == 123);
    }

    void testReadList()
    {
        vector<string> vec = pf->getList("readList1");
        NUT_TA(vec.size() == 3);
        NUT_TA(vec[0] == "a");
        NUT_TA(vec[1] == "b");
        NUT_TA(vec[2] == "cd");
    }
};

NUT_REGISTER_FIXTURE(TestPropertyFile, "util, quiet")
