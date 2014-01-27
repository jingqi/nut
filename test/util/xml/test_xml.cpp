
#include <nut/unittest/unittest.hpp>

#include <nut/util/xml/xmldocument.hpp>

#include <iostream>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestXml)
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

        nut::ref<XmlElement> e = gc_new<XmlElement>("e");
        e->setText("element");

        r->appendChild(e);
        r->appendChild(e);

        string s;
        r->serielize(&s);
        // cout << endl << s << endl;
        const char *expect = 
            "<root attr1=\"size\" attr2=\"&lt;good&gt;\">\n"
                "\tnice &lt;&gt; &quot;one&quot;\n"
                "\t<e>\n"
                    "\t\telement\n"
                "\t</e>\n"
                "\t<e>\n"
                    "\t\telement\n"
                "\t</e>\n"
            "</root>";
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
            "<e>"
            "element"
            "</e>"
            "</root>";
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
            "\t</e>\n"
            "\t<e2 at=\"s\" />\n"
            "</root>";
        nut::ref<XmlElement> r = gc_new<XmlElement>();
        r->parse(s);

        std::string out;
        r->serielize(&out, false);
        // cout << endl << out << endl;
        const char *expect = 
            "<root attr1=\"size\" attr2=\"&lt;good&gt;\">"
                "nice &lt;&gt; &quot;one&quot;"
                "<e>"
                    "element"
                "</e>"
                "<e2 at=\"s\" />"
            "</root>";
        NUT_TA(expect == out);
    }
};

NUT_REGISTER_FIXTURE(TestXml, "util, quiet")
