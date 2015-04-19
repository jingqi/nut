
#include <nut/unittest/unit_test.h>

#include <nut/util/txtcfg/xml/xml_element.h>

#include <iostream>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestXmlElement)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_write)
    NUT_CASE(test_write_single)
    NUT_CASE(test_read)
    NUT_CASES_END()

    void set_up() {}
    void tear_down() {}

    void test_write()
    {
        rc_ptr<XmlElement> r = rc_new<XmlElement>("root");
        r->add_attribute("attr1", "size");
        r->add_attribute("attr2", "<good>");
        r->set_text(" nice <> \"one\" ");
        r->add_comment(1, "comment");
        r->add_comment(10, "comment2");

        rc_ptr<XmlElement> e = rc_new<XmlElement>("e");
        e->set_text("element");
        r->append_child(e);

        rc_ptr<XmlElement> e2 = rc_new<XmlElement>("e2");
        r->append_child(e2);

        string s;
        r->serielize(&s);
        // cout << endl << s << endl;
        const char *expect =
            "<root attr1=\"size\" attr2=\"&lt;good&gt;\">\n"
                "\tnice &lt;&gt; &quot;one&quot;\n"
                "\t<e>\n"
                    "\t\telement\n"
                "\t</e>\n"
                "\t<!--comment-->\n"
                "\t<e2 />\n"
                "\t<!--comment2-->\n"
            "</root>";
        /*printf("%s\n%s\n", s.c_str(), expect);
		for (int i = 0; i < s.length(); ++i)
		{
			if (s[i] != expect[i])
			 {
				printf("d: %s", expect + i);
				break;
			}
		}*/
        NUT_TA(s == expect);

        s.clear();
        r->serielize(&s, false);
        // cout << endl << s << endl;
        expect =
            "<root attr1=\"size\" attr2=\"&lt;good&gt;\">"
            " nice &lt;&gt; &quot;one&quot; "
            "<e>"
            "element"
            "</e>"
            "<!--comment-->"
            "<e2 />"
            "<!--comment2-->"
            "</root>";
        //printf("%s\n", s.c_str());
        NUT_TA(s == expect);
    }

    void test_write_single()
    {
        rc_ptr<XmlElement> r = rc_new<XmlElement>("root");
        r->add_attribute("attr1", "size");
        r->add_attribute("attr2", "<good>");

        string s;
        r->serielize(&s);
        // cout << endl << s << endl;
        const char *expect =
            "<root attr1=\"size\" attr2=\"&lt;good&gt;\" />";
        NUT_TA(s == expect);
    }

    void test_read()
    {
        const char *s =
            " <root attr1=\"size\" \t attr2=\"&lt;good&gt;\"> \n"
            "\tnice &lt;&gt; &quot;one&quot;\n"
            "\t<e>\n"
            "\t\telement\n"
            "\t\t<!--comment-->\n"
            "\t\t<!--comment2-->\n"
            "\t</e>\n"
            "\t<e2 at=\"s\" />\n"
            "</root>";
        rc_ptr<XmlElement> r = rc_new<XmlElement>();
        r->parse(s);
        r = r->get_child(0);

        std::string rs;
        r->serielize(&rs, false);
        // cout << endl << rs << endl;
        const char *expect =
            "<root attr1=\"size\" attr2=\"&lt;good&gt;\">"
                "nice &lt;&gt; &quot;one&quot;"
                "<e>"
                    "element"
                    "<!--comment-->"
                    "<!--comment2-->"
                "</e>"
                "<e2 at=\"s\" />"
            "</root>";
        NUT_TA(expect == rs);
    }
};

NUT_REGISTER_FIXTURE(TestXmlElement, "util, txtcfg, quiet")
