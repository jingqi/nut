/**
 * @file -
 * @author jingqi
 * @date 2014-09-03
 * @last-edit 2014-09-03 00:17:46 jingqi
 * @brief
 */

#ifndef ___HEADFILE_66E380A0_1B5F_4503_AEB6_19DB1A28D1AE_
#define ___HEADFILE_66E380A0_1B5F_4503_AEB6_19DB1A28D1AE_

#include <assert.h>
#include <string>
#include <vector>

#include "string_writer.hpp"

namespace nut
{

class XmlWriter
{
    struct ElemState
    {
        std::string name;
        bool has_child;

        ElemState(const char *n)
            : name(n), has_child(false)
        {}
    };

    StringWriter *m_writer;
    std::vector<ElemState> m_elem_path;

public:
    XmlWriter(StringWriter *writer = NULL)
        : m_writer(writer)
    {}

    inline StringWriter* get_writer() const
    {
        return m_writer;
    }

    inline void set_writer(StringWriter *writer)
    {
        m_writer = writer;
    }

    void start_element(const char *name)
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

    void end_element()
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
            write(state.name.data(), state.name.length());
            write(">");
        }
		m_elem_path.pop_back();
    }

    void write_attribute(const char *name, const char *value)
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

    void write_text(const char *text)
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

    void write_comment(const char *comment)
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

private:
    inline void write(const char *s, int len = -1)
    {
        if (NULL == m_writer)
            return;
        m_writer->write(s, len);
    }

    void write_encode(const char *s, int len = -1)
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
};

}

#endif
