
#ifndef ___HEADFILE_99EF5EA5_F01C_4802_A51B_9F51F3C43C84_
#define ___HEADFILE_99EF5EA5_F01C_4802_A51B_9F51F3C43C84_

#include <vector>
#include <string>
#include <map>

#include "../../../nut_config.h"
#include "../../../rc/rc_new.h"
#include "xml_element.h"


namespace nut
{

class NUT_API XmlDom
{
    NUT_REF_COUNTABLE

public:
    XmlDom() noexcept;

    bool is_dirty() const noexcept;
    void set_dirty(bool dirty) noexcept;

    const std::string& get_version() const noexcept;
    void set_version(const std::string& version) noexcept;

    const std::string& get_encoding() const noexcept;
    void set_encoding(const std::string& encoding) noexcept;

    rc_ptr<XmlElement> get_root() const noexcept;
    void set_root(rc_ptr<XmlElement> root) noexcept;

    /**
     * 解析文本为 XML DOM
     *
     * @param ignore_text_blank 忽略文本首尾的空白和换行
     */
    void parse(const std::string& s, bool ignore_text_blank = true) noexcept;

    /**
     * 将 XML DOM 序列化为文本
     *
     * @param format 格式化输出，以便于阅读
     */
    std::string serielize(bool format = true) const noexcept;

private:
    std::string _version;
    std::string _encoding;
    rc_ptr<XmlElement> _root;
    bool _dirty = false;
};

}

#endif
