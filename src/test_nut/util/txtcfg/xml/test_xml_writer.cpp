
#include <sstream>

#include <nut/unittest/unit_test.h>

#include <nut/util/txtcfg/xml/xml_writer.h>

#include <iostream>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestXmlWriter)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoke)
    NUT_CASES_END()

    void test_smoke()
    {
        std::stringstream ss;
        XmlWriter w(&ss);
        w.start_element("a");
        w.write_attribute("b","c&");
        w.write_attribute("c","\"");
    
        w.start_element("d");
        w.write_attribute("e","<");
        w.end_element();
        
        w.write_comment("efg");
        w.write_text("h>i");
        
        w.end_element();
        
        //printf("%s", s.c_str());
        const char *s =
            "<a b=\"c&amp;\" c=\"&quot;\">"
                "<d e=\"&lt;\" />"
                "<!--efg-->"
                "h&gt;i"
            "</a>";
        NUT_TA(ss.str() == s);
    }
};

NUT_REGISTER_FIXTURE(TestXmlWriter, "util, txtcfg, quiet");
