
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/platform/os.h>

using namespace std;
using namespace nut;

class TestOS : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        //cout << endl;
        vector<string> subs;
        OS::list_dir(".", &subs);
        //for (size_t i = 0, sz = subs.size(); i < sz; ++i)
            //cout << subs.at(i) << endl;

        //wcout << endl;
        vector<wstring> subsw;
        OS::list_dir(L".", &subsw);
        //for (size_t i = 0, sz = subsw.size(); i < sz; ++i)
            //wcout << subsw.at(i) << endl;
        NUT_TA(subs.size() == subsw.size());
    }

};

NUT_REGISTER_FIXTURE(TestOS, "platform,quiet")
