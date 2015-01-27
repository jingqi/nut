
#include <nut/unittest/unittest.hpp>

#include <nut/util/txtcfg/xml/xml_element.hpp>

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
        nut::ref<XmlElement> r = GC_NEW(NULL, XmlElement, "root");
        r->add_attribute("attr1", "size");
        r->add_attribute("attr2", "<good>");
        r->set_text(" nice <> \"one\" ");
        r->add_comment(1, "comment");
        r->add_comment(10, "comment2");

        nut::ref<XmlElement> e = GC_NEW(NULL, XmlElement, "e");
        e->set_text("element");
        r->append_child(e);

        nut::ref<XmlElement> e2 = GC_NEW(NULL, XmlElement, "e2");
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
        nut::ref<XmlElement> r = GC_NEW(NULL, XmlElement, "root");
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
        nut::ref<XmlElement> r = GC_NEW(NULL, XmlElement);
        r->parse(s);
        r = r->get_child(0);

        std::string out;
        r->serielize(&out, false);
        // cout << endl << out << endl;
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
        NUT_TA(expect == out);
    }
};

NUT_REGISTER_FIXTURE(TestXmlElement, "util, txtcfg, quiet")
