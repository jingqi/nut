
#include "element_handler.h"


namespace nut
{

XmlElementHandler::XmlElementHandler(uint8_t mask) noexcept
    : _handle_mask(mask)
{}

void XmlElementHandler::set_name(const std::string& name) noexcept
{
    _name = name;
}

const std::string& XmlElementHandler::get_name() const noexcept
{
    return _name;
}

uint8_t XmlElementHandler::get_handle_mask() const noexcept
{
    return _handle_mask;
}

void XmlElementHandler::set_busy(bool busy) noexcept
{
    _busy = busy;
}

bool XmlElementHandler::is_busy() const noexcept
{
    return _busy;
}

void XmlElementHandler::handle_attribute(const std::string& name, const std::string& value) noexcept
{
    UNUSED(name);
    UNUSED(value);
}

void XmlElementHandler::handle_text(const std::string& text) noexcept
{
    UNUSED(text);
}

void XmlElementHandler::handle_comment(const std::string& comment) noexcept
{
    UNUSED(comment);
}

XmlElementHandler* XmlElementHandler::handle_child(const std::string& name) noexcept
{
    UNUSED(name);
    return nullptr;
}

void XmlElementHandler::handle_child_finish(XmlElementHandler *child) noexcept
{
    UNUSED(child);
}

void XmlElementHandler::handle_finish() noexcept
{}

}
