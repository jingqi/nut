
#include <assert.h>

#include "xml_writer.h"

namespace nut
{

XmlWriter::XmlWriter(StringWriter *writer)
    : m_writer(writer)
{}

void XmlWriter::start_element(const char *name)
{
    assert(NULL != name && '\0' != name[0]);

    if (!m_elem_path.empty())
    {
        ElemState& parent_state = m_elem_path[m_elem_path.size() - 1];
        if (!parent_state.has_child)
            write(">");
        parent_state.has_child = true;
    }
    m_elem_path.push_back(name);

    write("<");
    write(name);
}

void XmlWriter::end_element()
{
    if (m_elem_path.empty())
        return;

    const ElemState& state = m_elem_path.at(m_elem_path.size() - 1);
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
    m_elem_path.pop_back();
}

void XmlWriter::write_attribute(const char *name, const char *value)
{
    assert(NULL != name && '\0' != name[0] && NULL != value);
    if (m_elem_path.empty())
        return;
    if (m_elem_path.at(m_elem_path.size() - 1).has_child)
        return;

    write(" ");
    write(name);
    write("=\"");
    write_encode(value);
    write("\"");
}

void XmlWriter::write_text(const char *text)
{
    assert(NULL != text);
    if (!m_elem_path.empty())
    {
        ElemState& parent_state = m_elem_path[m_elem_path.size() - 1];
        if (!parent_state.has_child)
            write(">");
        parent_state.has_child = true;
    }

    write_encode(text);
}

void XmlWriter::write_comment(const char *comment)
{
    assert(NULL != comment);
    if (!m_elem_path.empty())
    {
        ElemState& parent_state = m_elem_path[m_elem_path.size() - 1];
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
    if (NULL == m_writer)
        return;
    m_writer->write(s, len);
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
