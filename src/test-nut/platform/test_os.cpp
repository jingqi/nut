
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/platform/os.h>
#include <nut/platform/path.h>

using namespace std;
using namespace nut;

class TestOS : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        // cout << endl << "---- " << Path::abspath(".") << endl;
        vector<string> subs = OS::listdir(".");
        // for (size_t i = 0, sz = subs.size(); i < sz; ++i)
        //     cout << subs.at(i) << endl;

        // wcout << endl << L"---- " << Path::abspath(L".") << endl;
        vector<wstring> subsw = OS::listdir(L".");
        // for (size_t i = 0, sz = subsw.size(); i < sz; ++i)
        //     wcout << subsw.at(i) << endl;
        NUT_TA(subs.size() == subsw.size());
    }

};

NUT_REGISTER_FIXTURE(TestOS, "platform,quiet")
