
#include "element_handler.h"


namespace nut
{

XmlElementHandler::XmlElementHandler(uint8_t mask)
    : _handle_mask(mask)
{}

void XmlElementHandler::set_name(const std::string& name)
{
    _name = name;
}

const std::string& XmlElementHandler::get_name() const
{
    return _name;
}

uint8_t XmlElementHandler::get_handle_mask() const
{
    return _handle_mask;
}

void XmlElementHandler::set_busy(bool busy)
{
    _busy = busy;
}

bool XmlElementHandler::is_busy() const
{
    return _busy;
}

void XmlElementHandler::handle_attribute(const std::string& name, const std::string& value)
{
    UNUSED(name);
    UNUSED(value);
}

void XmlElementHandler::handle_text(const std::string& text)
{
    UNUSED(text);
}

void XmlElementHandler::handle_comment(const std::string& comment)
{
    UNUSED(comment);
}

// 返回新的 handler
XmlElementHandler* XmlElementHandler::handle_child(const std::string& name)
{
    UNUSED(name);
    return nullptr;
}

// 回收旧的 handler
void XmlElementHandler::handle_child_finish(XmlElementHandler *child)
{
    UNUSED(child);
}

void XmlElementHandler::handle_finish()
{}

}
