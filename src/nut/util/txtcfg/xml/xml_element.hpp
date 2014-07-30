/**
 * @file -
 * @author jingqi
 * @date 2013-10-03
 * @last-edit 2014-07-30 01:46:04 jingqi
 * @brief
 */

#ifndef ___HEADFILE_B694AC90_3B03_460A_A7B6_AAE8EF5A3560_
#define ___HEADFILE_B694AC90_3B03_460A_A7B6_AAE8EF5A3560_

#include <assert.h>
#include <vector>
#include <string>
#include <map>

#include <nut/gc/gc.hpp>
#include <nut/util/string/stringutil.hpp>

namespace nut
{

class XmlElement
{
    NUT_GC_REFERABLE

    std::string m_name, m_text;
    std::vector<ref<XmlElement> > m_children;
    typedef std::map<std::string, std::string> attr_map_t;
    attr_map_t m_attrs;

private:
    static void encode(const std::string& in, std::string *out)
    {
        assert(NULL != out);
        for (register size_t i = 0, len = in.length(); i < len; ++i)
        {
            char c = in.at(i);
            switch (c)
            {
            case '&':
                *out += "&amp;";
                break;

            case '"':
                *out += "&quot;";
                break;

            case '<':
                *out += "&lt;";
                break;

            case '>':
                *out += "&gt;";
                break;

            default:
                out->push_back(c);
            }
        }
    }

    static void decode(const std::string& in, std::string *out)
    {
        assert(NULL != out);
        for (register size_t i = 0, len = in.length(); i < len; ++i) // loop
        {
            char c = in.at(i);
            if (c == '&' && i + 3 < len)
            {
                c = in.at(i + 1);
                switch (c) // switch
                {
                case 'l':
                    if ('t' == in.at(i + 2) && ';' == in.at(i + 3))
                    {
                        out->push_back('<');
                        i += 3;
                        continue; // loop
                    }
                    break; // switch

                case 'g':
                    if ('t' == in.at(i + 2) && ';' == in.at(i + 3))
                    {
                        out->push_back('>');
                        i += 3;
                        continue; // loop
                    }
                    break; // switch

                case 'a':
                    if (i + 4 < len && 'm' == in.at(i + 2) && 'p' == in.at(i + 3) && ';' == in.at(i + 4))
                    {
                        out->push_back('&');
                        i += 4;
                        continue; // loop
                    }
                    break; // switch

                case 'q':
                    if (i + 5 < len && 'u' == in.at(i + 2) && 'o' == in.at(i + 3) && 't' == in.at(i + 4) && ';' == in.at(i + 5))
                    {
                        out->push_back('\"');
                        i += 5;
                        continue; // loop
                    }
                    break; // switch
                }

                out->push_back('&');
                continue; // loop
            }

            out->push_back(c);
        }
    }

public:
    typedef attr_map_t::iterator attr_iter_t;
    typedef attr_map_t::const_iterator const_attr_iter_t;

public:
    XmlElement()
    {}

    XmlElement(const std::string& name)
        : m_name(name)
    {}

    inline const std::string& getName() const
    {
        return m_name;
    }

    inline void setName(const std::string& name)
    {
        m_name = name;
    }

    inline const std::string& getText() const
    {
        return m_text;
    }

    inline void setText(const std::string& text)
    {
        m_text = text;
    }

    inline size_t getChildrenCount() const
    {
        return m_children.size();
    }

    inline ref<XmlElement> getChild(size_t i) const
    {
        if (i >= m_children.size())
            return ref<XmlElement>();
        return m_children.at(i);
    }

    ref<XmlElement> getChild(const std::string& name) const
    {
        for (register size_t i = 0, s = m_children.size(); i < s; ++i)
        {
            ref<XmlElement> c = m_children.at(i);
            if (c.isNull())
                continue;
            if (c->m_name == name)
                return c;
        }
        return ref<XmlElement>();
    }

    inline void appendChild(ref<XmlElement> child)
    {
        assert(child.isNotNull());
        m_children.push_back(child);
    }

    inline void insertChild(size_t pos, ref<XmlElement> child)
    {
        assert(pos <= m_children.size() && child.isNotNull());
        m_children.insert(m_children.begin() + pos, child);
    }

    inline void removeChild(size_t pos)
    {
        assert(pos < m_children.size());
        m_children.erase(m_children.begin() + pos);
    }

    inline void clearChildren()
    {
        m_children.clear();
    }

    /**
     * 获取属性
     *
     * @param out 用来存储返回的属性值，可以为 NULL
     * @param 改属性是否存在
     */
    bool getAttribute(const std::string& name, std::string *out)
    {
        const_attr_iter_t iter = m_attrs.find(name);
        if (iter == m_attrs.end())
            return false;
        if (NULL != out)
            *out = iter->second;
        return true;
    }

    /**
     * 添加属性
     *
     * @return 是否成功，如果属性已经存在，则添加失败
     */
    bool addAttribute(const std::string& name, const std::string& value)
    {
        if (m_attrs.find(name) != m_attrs.end())
            return false;
        m_attrs.insert(std::pair<std::string,std::string>(name, value));
        return true;
    }

    /**
     * 存在属性则设置属性值，否则添加新属性
     */
    inline void setAttribute(const std::string& name, const std::string& value)
    {
        m_attrs[name] = value;
    }

    void clear()
    {
        m_name.clear();
        m_text.clear();
        m_attrs.clear();
        m_children.clear();
    }

    inline const_attr_iter_t attrBegin() const
    {
        return m_attrs.begin();
    }

    inline attr_iter_t attrBegin()
    {
        return m_attrs.begin();
    }

    inline const_attr_iter_t attrEnd() const
    {
        return m_attrs.end();
    }

    inline attr_iter_t attrEnd()
    {
        return m_attrs.end();
    }

    inline void parse(const std::string& s, bool ignore_text_blank = true)
    {
        parse(s, 0, ignore_text_blank);
    }

    /**
     * @param from 开始分析的位置
     * @param ignore_text_blank 忽略文本首尾的空白和换行
     * @return 已经分析完成位置的下一个位置
     */
    size_t parse(const std::string& s, size_t from, bool ignore_text_blank = true)
    {
        // clear
        clear();

        // define blanks
        const char *blanks = " \t\r\n";
#define BLANK_CASE \
    case ' ': \
    case '\t': \
    case '\r': \
    case '\n'
#define IS_BLANK(c) (' ' == (c) || '\t' == (c) || '\r' == (c) || '\n' == (c))

        // parse name
        const size_t slen = s.length();
        size_t i = s.find_first_of('<', from);
        if (std::string::npos == i)
            return slen;
        ++i;
        i = s.find_first_not_of(blanks, i);
        if (std::string::npos == i)
            return slen;
        while (i < slen)
        {
            const char c = s.at(i);
            if (IS_BLANK(c) || '/' == c || '>' == c)
                break;
            m_name.push_back(c);
            ++i;
        }

        // parse attribute
        enum State
        {
            INIT,               // initial, expect attributes or ">" or "/>"

            EXPECT_NAME,        // expect attribute name
            EXPECT_EQ,          // expect =
            EXPECT_FIRST_QUOT,  // expect first "
            EXPECT_VALUE,       // expect value
            EXPECT_GT,          // expect > and then eof(in case of "\>")

            FINISH_HEAD,        // finish header
            FINISH,             // finish
            FINISH_ERROR        // finish on error
        };
        std::string attr, value;
        State state = INIT;
        while (FINISH_HEAD != state && FINISH != state && FINISH_ERROR != state)
        {
            if (i >= slen)
                return slen;
            const char c = s.at(i++);
            switch (state)
            {
            case INIT:
                switch (c)
                {
                BLANK_CASE:
                    continue;

                case '/':
                    state = EXPECT_GT;
                    continue;

                case '>':
                    state = FINISH_HEAD;
                    continue;

                case '<':
                case '=':
                case '\"':
                    state = FINISH_ERROR;
                    continue;

                default:
                    state = EXPECT_NAME;
                    attr.push_back(c);
                    continue;
                }

            case EXPECT_NAME:
                switch (c)
                {
                BLANK_CASE:
                    state = EXPECT_EQ;
                    continue;

                case '=':
                    state = EXPECT_FIRST_QUOT;
                    continue;

                case '<':
                case '>':
                case '/':
                case '\"':
                    state = FINISH_ERROR;
                    continue;

                default:
                    attr.push_back(c);
                    continue;
                }

            case EXPECT_EQ:
                switch (c)
                {
                BLANK_CASE:
                    continue;

                case '=':
                    state = EXPECT_FIRST_QUOT;
                    continue;

                default:
                    state = FINISH_ERROR;
                    continue;
                }

            case EXPECT_FIRST_QUOT:
                switch (c)
                {
                BLANK_CASE:
                    continue;

                case '\"':
                    state = EXPECT_VALUE;
                    continue;

                default:
                    state = FINISH_ERROR;
                    continue;
                }

            case EXPECT_VALUE:
                switch (c)
                {
                case '\"':
                    state = INIT;
                    {
                        std::string v;
                        decode(value, &v);
                        m_attrs[attr] = v;
                    }
                    attr.clear();
                    value.clear();
                    continue;

                default:
                    value.push_back(c);
                    continue;
                }

            case EXPECT_GT:
                switch (c)
                {
                BLANK_CASE:
                    continue;

                case '>':
                    state = FINISH;
                    continue;

                default:
                    state = FINISH_ERROR;
                    continue;
                }

            default:
                assert(false);
            }
        }
        if (FINISH == state || FINISH_ERROR == state || i >= slen)
            return slen;

        while (true)
        {
            // parse text
            size_t j = s.find_first_of('<', i);
            if (std::string::npos == j)
                return slen;
            std::string text = s.substr(i, j - i);
            if (ignore_text_blank)
                text = trim(text);
            decode(text, &m_text);
            i = j;

            // parse element end
            j = s.find_first_not_of(blanks, i + 1);
            if (std::string::npos == j)
                return slen;
            if ('/' == s.at(j))
            {
                i = s.find_first_of('>', j);
                return i + 1;
            }

            // parse child element
            ref<XmlElement> child = gc_new<XmlElement>();
            i = child->parse(s, i, ignore_text_blank);
            m_children.push_back(child);
            if (i >= slen)
                return slen;
        }
#undef BLANK_CASE
#undef IS_BLANK
    }

    /*
     * @param format 格式化输出，以便于阅读
     */
    inline void serielize(std::string *out, bool format = true) const
    {
        assert(NULL != out);
        serielize(out, format ? 0 : -1);
    }

private:
    void serielize(std::string *out, int tab) const
    {
        assert(NULL != out);

        // name and attributes
        for (register int i = 0; i < tab; ++i)
            out->push_back('\t');
        out->push_back('<');
        *out += m_name;
        for (const_attr_iter_t iter = m_attrs.begin(), end = m_attrs.end();
            iter != end; ++iter)
        {
            out->push_back(' ');
            *out += iter->first;
            *out += "=\"";
            encode(iter->second, out);
            out->push_back('\"');
        }

        const size_t csize = m_children.size();
        if (0 == m_text.length() && 0 == csize)
        {
            *out += " />";
            return;
        }
        out->push_back('>');

        // text
        if (tab < 0)
        {
            encode(m_text, out);
        }
        else
        {
            // need format
            const std::string text = trim(m_text, " \t\r\n");
            if (text.length() > 0)
            {
                out->push_back('\n');
                for (register int i = 0; i < tab + 1; ++i)
                    out->push_back('\t');
                encode(text, out);
            }
        }

        // children element
        for (register size_t i = 0; i < csize; ++i)
        {
            ref<XmlElement> c = m_children.at(i);
            if (c.isNull())
                continue;
            if (tab >= 0)
                out->push_back('\n');
            c->serielize(out, tab >= 0 ? tab + 1 : tab);
        }

        // end
        if (tab >= 0)
            out->push_back('\n');
        for (register int i = 0; i < tab; ++i)
            out->push_back('\t');
        *out += "</";
        *out += m_name;
        out->push_back('>');
    }
};

}

#endif
