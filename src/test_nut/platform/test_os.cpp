
#include <iostream>
#include <nut/unittest/unit_test.h>
#include <nut/platform/os.h>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestOS)
{
	NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

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
