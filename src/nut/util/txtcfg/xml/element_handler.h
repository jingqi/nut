
#ifndef ___HEADFILE_543264C6_1906_4757_AFBF_6B9D023B2EA6_
#define ___HEADFILE_543264C6_1906_4757_AFBF_6B9D023B2EA6_

#include <stdint.h>
#include <string>

#include "../../../nut_config.h"


namespace nut
{

/**
 * 读取 xml 时处理各个 XML element 的 handler
 */
class NUT_API XmlElementHandler
{
public:
    // 控制处理哪些子元素
    enum HANDLE_MASK
    {
        HANDLE_ATTRIBUTE = 0x01,
        HANDLE_TEXT = 0x02,
        HANDLE_COMMENT = 0x04,
        HANDLE_CHILD = 0x08
    };

private:
    // 名称
    std::string _name;

    // 控制处理哪些子元素
    const uint8_t _handle_mask = 0xff;

    // 防止递归路径上的重复使用
    bool _busy = false;

public:
    explicit XmlElementHandler(uint8_t mask = 0xff);
    virtual ~XmlElementHandler() = default;

    void set_name(const std::string& name);
    const std::string& get_name() const;

    uint8_t get_handle_mask() const;

    void set_busy(bool busy);
    bool is_busy() const;

public:
    virtual void handle_attribute(const std::string& name, const std::string& value);

    virtual void handle_text(const std::string& text);

    virtual void handle_comment(const std::string& comment);

    // 返回新的 handler
    virtual XmlElementHandler* handle_child(const std::string& name);

    // 回收旧的 handler
    virtual void handle_child_finish(XmlElementHandler *child);

    virtual void handle_finish();
};

}

#endif
