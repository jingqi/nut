
#include <nut/unittest/unittest.h>

#include <nut/util/txtcfg/xml/xml_parser.h>

#include <stdio.h>
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
        //printf("attrib %s=%s\n", name.c_str(), value.c_str());
        if (name == "a" && value == "b&\"<>")
            tag += "a";
    }

    virtual void handle_text(const std::string& text)
    {
        //printf("text %s\n", text.c_str());
        if (text == "c")
            tag += "t";
    }

    virtual void handle_comment(const std::string& comment)
    {
        //printf("comment %s\n", comment.c_str());
        if (comment == "d")
            tag += "c";
    }

    virtual XmlElementHandler* handle_child(const std::string &name)
    {
        //printf("child %s\n", name.c_str());
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
    NUT_CASE(test_smoke)
    NUT_CASES_END()

    void set_up()
    {
        tag.clear();
    }

    void tear_down() {}

    void test_smoke()
    {
        Handle h;
        XmlParser p(&h);
        const char *s =
        "<a><ch a=\"b&amp;&quot;&lt;&gt;\"/>c<!--d--></a>";
        p.input(s);
        p.finish();
        if (p.has_error())
            printf("xml error %d:%d %s\n", (int)p.line(), (int)p.column(), p.error_message().c_str());
        NUT_TA(!p.has_error());
        //printf("%s\n", tag.c_str());
        NUT_TA(tag == "hayxtcyx");
    }
};

NUT_REGISTER_FIXTURE(TestXmlParser, "util, txtcfg, quiet");
