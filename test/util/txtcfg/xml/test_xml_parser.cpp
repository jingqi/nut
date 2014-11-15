
#include <nut/unittest/unittest.hpp>

#include <nut/util/txtcfg/xml/xml_parser.hpp>

#include <iostream>

using namespace std;
using namespace nut;

namespace
{

string tag;

class Handle : public XmlElementHandler
{

public:
    virtual void handle_attribute(const std::string &name, const std::string &value)
    {
        //printf("attrib %s=%s\n", name.data(), value.data());
        if (name == "a" && value == "b&\"<>")
            tag += "a";
    }

    virtual void handle_text(const std::string& text)
    {
        //printf("text %s\n", text.data());
        if (text == "c")
            tag += "t";
    }

    virtual void handle_comment(const std::string& comment)
    {
        //printf("comment %s\n", comment.data());
        if (comment == "d")
            tag += "c";
    }

    virtual XmlElementHandler* handle_child(const std::string &name)
    {
        //printf("child %s\n", name.data());
        if (name == "ch")
            tag += "h";
        return this;
    }

    virtual void handle_child_finish(XmlElementHandler *child)
    {
        //printf("fin child\n");
        if (child == this)
            tag += "x";
    }

    virtual void handle_finish()
    {
        //printf("fin\n");
        tag += "y";
    }
};

}

NUT_FIXTURE(TestXmlParser)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoke)
    NUT_CASES_END()

    void setUp()
    {
        tag.clear();
    }

    void tearDown() {}

    void testSmoke()
    {
        Handle h;
        XmlParser p(&h);
        const char *s =
        "<a><ch a=\"b&amp;&quot;&lt;&gt;\"/>c<!--d--></a>";
        p.input(s);
        p.finish();
        if (p.has_error())
            printf("xml error %d:%d %s\n", (int)p.line(), (int)p.column(), p.error_message().data());
        NUT_TA(!p.has_error());
        //printf("%s\n", tag.data());
        NUT_TA(tag == "hayxtcyx");
    }
};

NUT_REGISTER_FIXTURE(TestXmlParser, "util, txtcfg, quiet");
