
#include <nut/unittest/unittest.h>

#include <nut/util/txtcfg/xml/xml_writer.h>

#include <iostream>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestXmlWriter)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoke)
    NUT_CASES_END()

    void set_up() {}

    void tear_down() {}
    
    void test_smoke()
    {
        string s;
        StdStringWriter sw(&s);
        XmlWriter w(&sw);
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
        const char *ss =
			"<a b=\"c&amp;\" c=\"&quot;\">"
				"<d e=\"&lt;\" />"
				"<!--efg-->"
				"h&gt;i"
			"</a>";
        NUT_TA(s == ss);
    }
};

NUT_REGISTER_FIXTURE(TestXmlWriter, "util, txtcfg, quiet");
