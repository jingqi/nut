/**
 * @file -
 * @author jingqi
 * @date 2013-10-03
 * @last-edit 2014-07-30 01:48:13 jingqi
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

public:
    XmlDom()
        : m_version("1.0"), m_encoding("UTF-8")
    {}

    inline const std::string& getVersion() const
    {
        return m_version;
    }

    inline void setVersion(const std::string& version)
    {
        m_version = version;
    }

    inline const std::string& getEncoding() const
    {
        return m_encoding;
    }

    inline void setEncoding(const std::string& encoding)
    {
        m_encoding = encoding;
    }

    inline ref<XmlElement> getRoot() const
    {
        return m_root;
    }

    inline void setRoot(ref<XmlElement> root)
    {
        m_root = root;
    }

    /**
     * 解析文本为 XML DOM
     *
     * @param ignore_text_blank 忽略文本首尾的空白和换行
     */
    void parse(const std::string& s, bool ignore_text_blank = true)
    {
        m_version.clear();
        m_encoding.clear();
        m_root.clear();

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
    }

    /**
     * 将 XML DOM 序列化为文本
     *
     * @param format 格式化输出，以便于阅读
     */
    void serielize(std::string *out, bool format = true) const
    {
        assert(NULL != out);
        *out += "<?xml version=\"";
        *out += m_version;
        *out += "\" encoding=\"";
        *out += m_encoding;
        *out += "\"?>";
        if (m_root.isNull())
            return;
        if (format)
            out->push_back('\n');
        m_root->serielize(out, format);
    }
};

}

#endif
