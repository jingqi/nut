
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

void XmlWriter::start_element(const std::string& name)
{
    assert(!name.empty());

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
        write(state.name.c_str(), state.name.length());
        write(">");
    }
    _elem_path.pop_back();
}

void XmlWriter::write_attribute(const std::string& name, const std::string& value)
{
    assert(!name.empty());
    if (_elem_path.empty())
        return;
    if (_elem_path.at(_elem_path.size() - 1).has_child)
        return;

    write(" ");
    write(name);
    write("=\"");
    write_encode(value.c_str(), value.length());
    write("\"");
}

void XmlWriter::write_text(const std::string& text)
{
    if (!_elem_path.empty())
    {
        ElemState& parent_state = _elem_path[_elem_path.size() - 1];
        if (!parent_state.has_child)
            write(">");
        parent_state.has_child = true;
    }

    write_encode(text.c_str(), text.length());
}

void XmlWriter::write_comment(const std::string& comment)
{
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

void XmlWriter::write(const char *s, ssize_t len)
{
    if (nullptr == _os)
        return;
    if (nullptr == s || 0 == len)
        return;
    if (len < 0)
        *_os << s;
    for (ssize_t i = 0; i < len && '\0' != s[i]; ++i)
        *_os << s[i];
}

void XmlWriter::write(const std::string& s)
{
    *_os << s;
}

void XmlWriter::write_encode(const char *s, ssize_t len)
{
    for (ssize_t i = 0; (len < 0 || i < len) && '\0' != s[i]; ++i)
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
