
#include <assert.h>

#include <nut/platform/path.h>

#include "xml_dom.h"

namespace nut
{

XmlDom::XmlDom()
    : m_version("1.0"), m_encoding("UTF-8"), m_dirty(false)
{}

bool XmlDom::is_dirty() const
{
    if (m_dirty)
        return true;
    if (m_root.is_null())
        return false;
    return m_root->is_dirty();
}

void XmlDom::set_dirty(bool dirty)
{
    if (dirty)
    {
        m_dirty = true;
        return;
    }

    m_dirty = false;
    if (m_root.is_not_null())
        m_root->set_dirty(false);
}

void XmlDom::set_version(const std::string& version)
{
    if (version != m_version)
    {
        m_version = version;
        m_dirty = true;
    }
}

void XmlDom::set_encoding(const std::string& encoding)
{
    if (encoding != m_encoding)
    {
        m_encoding = encoding;
        m_dirty = true;
    }
}

void XmlDom::set_root(rc_ptr<XmlElement> root)
{
    if (root != m_root)
    {
        m_root = root;
        m_dirty = true;
    }
}

/**
 * 解析文本为 XML DOM
 *
 * @param ignore_text_blank 忽略文本首尾的空白和换行
 */
void XmlDom::parse(const std::string& s, bool ignore_text_blank)
{
    m_version = "1.0";
    m_encoding = "UTF-8";
    m_root.set_null();
    m_dirty = true;

    size_t i = s.find("<?");
    if (std::string::npos == i)
        return;
    i += 2; // length of "<?"
    i = s.find("xml", i);
    if (std::string::npos == i)
        return;
    i += 4; // length of "xml" and a space
    i = s.find("version", i);
    if (std::string::npos == i)
        return;
    i += 8; // length of "version" and a space or '='
    i = s.find('\"', i);
    if (std::string::npos == i)
        return;
    ++i;
    size_t start = i;
    i = s.find('\"', i);
    if (std::string::npos == i)
        return;
    m_version = s.substr(start, i - start);
    i += 2; // length of '\"' and a space

    i = s.find("encoding", i);
    if (std::string::npos == i)
        return;
    i += 9; // length of "encoding" and a space or '='
    i = s.find('\"', i);
    if (std::string::npos == i)
        return;
    ++i;
    start = i;
    i = s.find('\"', i);
    if (std::string::npos == i)
        return;
    m_encoding = s.substr(start, i - start);
    ++i;
    i = s.find("?>", i);
    if (std::string::npos == i)
        return;
    i += 2;

    i = s.find('<', i);
    if (std::string::npos == i)
        return; // no element found
    m_root = RC_NEW(NULL, XmlElement);
    m_root->parse(s, i, ignore_text_blank);
    if (m_root->get_children_count() > 0)
        m_root = m_root->get_child(0);
    else
        m_root.set_null();
}

/**
 * 将 XML DOM 序列化为文本
 *
 * @param format 格式化输出，以便于阅读
 */
void XmlDom::serielize(std::string *appended, bool format) const
{
    assert(NULL != appended);

    // xml header
    *appended += "<?xml version=\"";
    *appended += m_version;
    *appended += "\" encoding=\"";
    *appended += m_encoding;
    *appended += "\"?>";
    if (m_root.is_null())
        return;
    if (format)
        appended->push_back('\n');

    // xml elements
    StdStringWriter sw(appended);
    XmlWriter w(&sw);
    m_root->serielize(w, format ? 0 : -1);
}

}
