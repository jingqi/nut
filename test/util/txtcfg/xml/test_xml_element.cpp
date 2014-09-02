
#include <nut/unittest/unittest.hpp>

#include <nut/util/txtcfg/xml/xml_element.hpp>

#include <iostream>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestXmlElement)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testWrite)
    NUT_CASE(testWriteSingle)
    NUT_CASE(testRead)
    NUT_CASES_END()

    void setUp() {}
    void tearDown() {}

    void testWrite()
    {
        nut::ref<XmlElement> r = gc_new<XmlElement>("root");
        r->addAttribute("attr1", "size");
        r->addAttribute("attr2", "<good>");
        r->setText(" nice <> \"one\" ");
        r->addComment(1, "comment");
        r->addComment(10, "comment2");

        nut::ref<XmlElement> e = gc_new<XmlElement>("e");
        e->setText("element");
        r->appendChild(e);

        nut::ref<XmlElement> e2 = gc_new<XmlElement>("e2");
        r->appendChild(e2);

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
		/*printf("%s\n%s\n", s.data(), expect);
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
		//printf("%s\n", s.data());
        NUT_TA(s == expect);
    }

    void testWriteSingle()
    {
        nut::ref<XmlElement> r = gc_new<XmlElement>("root");
        r->addAttribute("attr1", "size");
        r->addAttribute("attr2", "<good>");

        string s;
        r->serielize(&s);
        // cout << endl << s << endl;
        const char *expect =
            "<root attr1=\"size\" attr2=\"&lt;good&gt;\" />";
        NUT_TA(s == expect);
    }

    void testRead()
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
        nut::ref<XmlElement> r = gc_new<XmlElement>();
        r->parse(s);
		r = r->getChild(0);

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
