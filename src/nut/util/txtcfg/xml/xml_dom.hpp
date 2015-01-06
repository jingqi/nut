/**
 * @file -
 * @author jingqi
 * @date 2013-10-03
 * @last-edit 2015-01-06 23:42:38 jingqi
 * @brief
 */

#ifndef ___HEADFILE_99EF5EA5_F01C_4802_A51B_9F51F3C43C84_
#define ___HEADFILE_99EF5EA5_F01C_4802_A51B_9F51F3C43C84_

#include <assert.h>
#include <vector>
#include <string>
#include <map>
#include <fstream>

#include <nut/gc/gc.hpp>
#include <nut/platform/path.hpp>

#include "xml_element.hpp"

namespace nut
{

class XmlDom
{
    NUT_GC_REFERABLE

    std::string m_version;
    std::string m_encoding;
    ref<XmlElement> m_root;
	bool m_dirty;

public:
    XmlDom()
		: m_version("1.0"), m_encoding("UTF-8"), m_dirty(false)
    {}

	bool is_dirty() const
	{
		if (m_dirty)
			return true;
		if (m_root.is_null())
			return false;
		return m_root->is_dirty();
	}

	void set_dirty(bool dirty)
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

    inline const std::string& get_version() const
    {
        return m_version;
    }

    inline void set_version(const std::string& version)
    {
		if (version != m_version)
		{
			m_version = version;
			m_dirty = true;
		}
    }

    inline const std::string& get_encoding() const
    {
        return m_encoding;
    }

    inline void set_encoding(const std::string& encoding)
    {
		if (encoding != m_encoding)
		{
			m_encoding = encoding;
			m_dirty = true;
		}
    }

    inline ref<XmlElement> get_root() const
    {
        return m_root;
    }

    inline void set_root(ref<XmlElement> root)
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
    void parse(const std::string& s, bool ignore_text_blank = true)
    {
        m_version = "1.0";
        m_encoding = "UTF-8";
        m_root.clear();
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
        m_root = gc_new<XmlElement>();
        m_root->parse(s, i, ignore_text_blank);
		if (m_root->getChildrenCount() > 0)
			m_root = m_root->getChild(0);
		else
			m_root.clear();
    }

    /**
     * 将 XML DOM 序列化为文本
     *
     * @param format 格式化输出，以便于阅读
     */
    void serielize(std::string *out, bool format = true) const
    {
        assert(NULL != out);

		// xml header
        *out += "<?xml version=\"";
        *out += m_version;
        *out += "\" encoding=\"";
        *out += m_encoding;
        *out += "\"?>";
        if (m_root.is_null())
            return;
        if (format)
            out->push_back('\n');

        // xml elements
        StdStringWriter sw(out);
        XmlWriter w(&sw);
        m_root->serielize(w, format ? 0 : -1);
    }
};

}

#endif
