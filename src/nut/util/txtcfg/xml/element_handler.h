
#ifndef ___HEADFILE_543264C6_1906_4757_AFBF_6B9D023B2EA6_
#define ___HEADFILE_543264C6_1906_4757_AFBF_6B9D023B2EA6_

#include <stdint.h>
#include <string>

namespace nut
{

/**
 * 读取 xml 时处理各个 XML element 的 handler
 */
class XmlElementHandler
{
public:
    enum HANDLE_MASK
    {
        HANDLE_ATTRIBUTE = 0x01,
        HANDLE_TEXT = 0x02,
        HANDLE_COMMENT = 0x04,
        HANDLE_CHILD = 0x08
    };

    const char *name = NULL;
    const uint8_t handle_mask = 0xFF;

    XmlElementHandler(const char *n = NULL, uint8_t mask = 0xFF)
        : name(n), handle_mask(mask)
    {}

    virtual ~XmlElementHandler()
    {}

    virtual void handle_attribute(const std::string& name, const std::string& value)
    {
        (void) name;
        (void) value;
    }

    virtual void handle_text(const std::string& text)
    {
        (void) text;
    }

    virtual void handle_comment(const std::string& comment)
    {
        (void) comment;
    }

    // 返回新的 handler
    virtual XmlElementHandler* handle_child(const std::string& name)
    {
        (void) name;
        return NULL;
    }

    // 回收旧的 handler
    virtual void handle_child_finish(XmlElementHandler *child)
    {
        (void) child;
    }

    virtual void handle_finish()
    {}

};

}

#endif
