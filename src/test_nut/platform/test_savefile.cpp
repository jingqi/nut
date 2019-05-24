
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/platform/savefile.h>
#include <nut/platform/path.h>
#include <nut/platform/os.h>
#include <nut/util/txtcfg/text_file.h>

using namespace std;
using namespace nut;

class TestSaveFile : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_smoking);
    }

    void test_smoking()
    {
        const char *filename = "test-savefile.data";
        SaveFile sf(filename);
        NUT_TA(sf.open());
        NUT_TA(sf.write("abcdef", 6));
        NUT_TA(sf.commit());

        std::string text;
        TextFile::read_file(filename, &text);
        NUT_TA(text == "abcdef");
        OS::removefile(filename);
    }

};

NUT_REGISTER_FIXTURE(TestSaveFile, "platform,quiet")
