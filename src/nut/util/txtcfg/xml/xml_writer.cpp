
#include <assert.h>

#include "xml_writer.h"

namespace nut
{

XmlWriter::XmlWriter(std::ostream *os)
    : _os(os)
{}

std::ostream* XmlWriter::get_output_stream() const
{
    return _os;
}

void XmlWriter::set_output_stream(std::ostream *os)
{
    _os = os;
}

void XmlWriter::start_element(const char *name)
{
    assert(nullptr != name && '\0' != name[0]);

    if (!_elem_path.empty())
    {
        ElemState& parent_state = _elem_path[_elem_path.size() - 1];
        if (!parent_state.has_child)
            write(">");
        parent_state.has_child = true;
    }
    _elem_path.emplace_back(name);

    write("<");
    write(name);
}

void XmlWriter::end_element()
{
    if (_elem_path.empty())
        return;

    const ElemState& state = _elem_path.at(_elem_path.size() - 1);
    if (!state.has_child)
    {
        write(" />");
    }
    else
    {
        write("</");
        write(state.name.c_str(), (int) state.name.length());
        write(">");
    }
    _elem_path.pop_back();
}

void XmlWriter::write_attribute(const char *name, const char *value)
{
    assert(nullptr != name && '\0' != name[0] && nullptr != value);
    if (_elem_path.empty())
        return;
    if (_elem_path.at(_elem_path.size() - 1).has_child)
        return;

    write(" ");
    write(name);
    write("=\"");
    write_encode(value);
    write("\"");
}

void XmlWriter::write_text(const char *text)
{
    assert(nullptr != text);
    if (!_elem_path.empty())
    {
        ElemState& parent_state = _elem_path[_elem_path.size() - 1];
        if (!parent_state.has_child)
            write(">");
        parent_state.has_child = true;
    }

    write_encode(text);
}

void XmlWriter::write_comment(const char *comment)
{
    assert(nullptr != comment);
    if (!_elem_path.empty())
    {
        ElemState& parent_state = _elem_path[_elem_path.size() - 1];
        if (!parent_state.has_child)
            write(">");
        parent_state.has_child = true;
    }

    write("<!--");
    write(comment);
    write("-->");
}

void XmlWriter::write(const char *s, int len)
{
    if (nullptr == _os)
        return;
    if (nullptr == s || 0 == len)
        return;
    if (len < 0)
        *_os << s;
    for (int i = 0; i < len && '\0' != s[i]; ++i)
        *_os << s[i];
}

void XmlWriter::write_encode(const char *s, int len)
{
    for (int i = 0; (len < 0 || i < len) && '\0' != s[i]; ++i)
    {
        switch (s[i])
        {
        case '&':
            write("&amp;");
            break;

        case '"':
            write("&quot;");
            break;

        case '<':
            write("&lt;");
            break;

        case '>':
            write("&gt;");
            break;

        default:
            write(s + i, 1);
            break;
        }
    }
}

}
