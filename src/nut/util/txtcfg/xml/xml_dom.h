
#ifndef ___HEADFILE_99EF5EA5_F01C_4802_A51B_9F51F3C43C84_
#define ___HEADFILE_99EF5EA5_F01C_4802_A51B_9F51F3C43C84_

#include <vector>
#include <string>
#include <map>

#include <nut/nut_config.h>
#include <nut/rc/rc_new.h>

#include "xml_element.h"

namespace nut
{

class NUT_API XmlDom
{
    NUT_REF_COUNTABLE

    std::string _version;
    std::string _encoding;
    rc_ptr<XmlElement> _root;
    bool _dirty = false;

public:
    XmlDom();

    bool is_dirty() const;

    void set_dirty(bool dirty);

    const std::string& get_version() const
    {
        return _version;
    }

    void set_version(const std::string& version);

    const std::string& get_encoding() const
    {
        return _encoding;
    }

    void set_encoding(const std::string& encoding);

    rc_ptr<XmlElement> get_root() const
    {
        return _root;
    }

    void set_root(rc_ptr<XmlElement> root);

    /**
     * 解析文本为 XML DOM
     *
     * @param ignore_text_blank 忽略文本首尾的空白和换行
     */
    void parse(const std::string& s, bool ignore_text_blank = true);

    /**
     * 将 XML DOM 序列化为文本
     *
     * @param format 格式化输出，以便于阅读
     */
    std::string serielize(bool format = true) const;
};

}

#endif
