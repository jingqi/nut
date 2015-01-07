
#include <iostream>
#include <nut/unittest/unittest.hpp>
#include <nut/platform/os.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestOS)
{
	NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void set_up() {}
    void tear_down() {}

    void test_smoking()
	{
        //cout << endl;
        vector<string> subs = OS::listdir(".");
        //for (size_t i = 0, sz = subs.size(); i < sz; ++i)
            //cout << subs.at(i) << endl;

        //wcout << endl;
        vector<wstring> subsw = OS::listdir(L".");
        //for (size_t i = 0, sz = subsw.size(); i < sz; ++i)
            //wcout << subsw.at(i) << endl;
        NUT_TA(subs.size() == subsw.size());
	}

};

NUT_REGISTER_FIXTURE(TestOS, "platform,quiet")
