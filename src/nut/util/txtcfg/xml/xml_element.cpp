﻿
#include <assert.h>
#include <sstream>

#include "../../../util/string/string_utils.h"
#include "xml_element.h"
#include "xml_parser.h"


namespace nut
{

XmlElement::XmlElement(const std::string& name) noexcept
    : _name(name)
{}

bool XmlElement::is_dirty() const noexcept
{
    if (_dirty)
        return true;
    for (size_t i = 0, sz = _children.size(); i < sz; ++i)
    {
        if (_children.at(i)->is_dirty())
            return true;
    }
    return false;
}

void XmlElement::set_dirty(bool dirty) noexcept
{
    if (dirty)
    {
        _dirty = true;
        return;
    }

    _dirty = false;
    for (size_t i = 0, sz = _children.size(); i < sz; ++i)
        _children.at(i)->set_dirty(false);
}

const std::string& XmlElement::get_name() const noexcept
{
    return _name;
}

void XmlElement::set_name(const std::string& name) noexcept
{
    if (name != _name)
    {
        _name = name;
        _dirty = true;
    }
}

const std::string& XmlElement::get_text() const noexcept
{
    return _text;
}

void XmlElement::set_text(const std::string& text) noexcept
{
    if (text != _text)
    {
        _text = text;
        _dirty = true;
    }
}

size_t XmlElement::get_children_count() const noexcept
{
    return _children.size();
}

rc_ptr<XmlElement> XmlElement::get_child(size_t i) const noexcept
{
    if (i >= _children.size())
        return rc_ptr<XmlElement>();
    return _children.at(i);
}

rc_ptr<XmlElement> XmlElement::get_child(const std::string& name) const noexcept
{
    for (size_t i = 0, s = _children.size(); i < s; ++i)
    {
        rc_ptr<XmlElement> c = _children.at(i);
        if (c.is_null())
            continue;
        if (c->_name == name)
            return c;
    }
    return rc_ptr<XmlElement>();
}

void XmlElement::append_child(rc_ptr<XmlElement> child) noexcept
{
    assert(child.is_not_null());
    _children.push_back(std::move(child));
    _dirty = true;
}

void XmlElement::insert_child(size_t pos, rc_ptr<XmlElement> child) noexcept
{
    assert(pos <= _children.size() && child.is_not_null());
    _children.insert(_children.begin() + pos, child);
    _dirty = true;
}

void XmlElement::remove_child(size_t pos) noexcept
{
    assert(pos < _children.size());
    _children.erase(_children.begin() + pos);
    _dirty = true;
}

void XmlElement::clear_children() noexcept
{
    _children.clear();
    _dirty = true;
}

const std::string* XmlElement::get_attribute(const std::string& name) const noexcept
{
    const_attr_iter_type iter = _attrs.find(name);
    if (iter == _attrs.end())
        return nullptr;
    return &iter->second;
}

bool XmlElement::add_attribute(const std::string& name, const std::string& value) noexcept
{
    if (_attrs.find(name) != _attrs.end())
        return false;
    _attrs.insert(std::pair<std::string,std::string>(name, value));
    _dirty = true;
    return true;
}

void XmlElement::set_attribute(const std::string& name, const std::string& value) noexcept
{
    _attrs[name] = value;
    _dirty = true;
}

bool XmlElement::remove_attribute(const std::string& name) noexcept
{
    attr_iter_type iter = _attrs.find(name);
    if (iter == _attrs.end())
        return false;
    _attrs.erase(iter);
    _dirty = true;
    return true;
}

void XmlElement::clear_attributes() noexcept
{
    _attrs.clear();
    _dirty = true;
}

void XmlElement::add_comment(size_t pos, const std::string& text) noexcept
{
    // binary search
    int left = -1, right = (int) _comments.size();
    while (left + 1 < right)
    {
        const int mid = (left + right) / 2;
        if (_comments.at(mid).pos <= pos)
            left = mid;
        else
            right = mid;
    }
    _comments.insert(_comments.begin() + right, Comment(pos, text));
}

void XmlElement::remove_comment(size_t pos) noexcept
{
    // binary search
    const int size = (int) _comments.size();
    int left = -1, right = size;
    while (left + 1 < right)
    {
        const int mid = (left + right) / 2;
        if (_comments.at(mid).pos < pos)
        {
            left = mid;
        }
        else if (_comments.at(mid).pos > pos)
        {
            right = mid;
        }
        else
        {
            // found
            left = mid;
            while (left > 0 && _comments.at(left - 1).pos == pos)
                --left;
            right = mid + 1;
            while (right < size && _comments.at(right).pos == pos)
                ++right;
            _comments.erase(_comments.begin() + left, _comments.begin() + right);
            return;
        }
    }
}

void XmlElement::clear() noexcept
{
    _name.clear();
    _text.clear();
    _attrs.clear();
    _children.clear();
    _comments.clear();
    _dirty = true;
}

XmlElement::const_attr_iter_type XmlElement::attr_const_begin() const noexcept
{
    return _attrs.begin();
}

XmlElement::const_attr_iter_type XmlElement::attr_const_end() const noexcept
{
    return _attrs.end();
}

XmlElement::attr_iter_type XmlElement::attr_begin() noexcept
{
    _dirty = true; // in case of modification
    return _attrs.begin();
}

XmlElement::attr_iter_type XmlElement::attr_end() noexcept
{
    _dirty = true; // in case of modification
    return _attrs.end();
}

void XmlElement::parse(const std::string& s, size_t start_index, bool ignore_text_blank) noexcept
{
    assert(start_index <= s.length());

    class Handler : public XmlElementHandler
    {
        XmlElement *_elem = nullptr;
        bool _ignore_text_blank = false;

    public:
        Handler(XmlElement *e, bool ignore_text_blank)
            : _elem(e), _ignore_text_blank(ignore_text_blank)
        {}

        virtual void handle_attribute(const std::string &name, const std::string &value) noexcept override
        {
            _elem->add_attribute(name, value);
        }

        virtual void handle_text(const std::string& text) noexcept override
        {
            _elem->_text += text;
        }

        virtual void handle_comment(const std::string& comment) noexcept override
        {
            _elem->add_comment(_elem->_children.size(), comment);
        }

        virtual XmlElementHandler* handle_child(const std::string &name) noexcept override
        {
            rc_ptr<XmlElement> c = rc_new<XmlElement>(name);
            _elem->append_child(c);
            return new Handler(c, _ignore_text_blank);
        }

        virtual void handle_child_finish(XmlElementHandler *child) noexcept override
        {
            delete child;
        }

        virtual void handle_finish() noexcept override
        {
            if (_ignore_text_blank)
                _elem->_text = trim(_elem->_text);
        }
    };

    clear();
    Handler h(this, ignore_text_blank);
    XmlParser p(&h);
    p.input(s.c_str() + start_index, (int) (s.length() - start_index));
    p.finish();
}

std::string XmlElement::serielize(bool format) const noexcept
{
    std::stringstream ss;
    XmlWriter w(&ss);
    serielize(w, format ? 0 : -1);
    return ss.str();
}

void XmlElement::serielize(XmlWriter &writer, int tab) const noexcept
{
    // name
    for (int i = 0; i < tab; ++i)
        writer.write_text("\t");
    writer.start_element(_name.c_str());

    // attributes
    for (const_attr_iter_type iter = _attrs.begin(), end = _attrs.end();
        iter != end; ++iter)
    {
        writer.write_attribute(iter->first.c_str(), iter->second.c_str());
    }

    // text
    bool has_child = false;
    if (!_text.empty())
    {
        if (tab >= 0)
        {
            std::string text = trim(_text);
            if (!text.empty())
            {
                writer.write_text("\n");
                for (int i = 0; i < tab + 1; ++i)
                    writer.write_text("\t");
                writer.write_text(text.c_str());
                has_child = true;
            }
        }
        else
        {
            writer.write_text(_text.c_str());
            has_child = true;
        }
    }

    // children and comments
    const size_t comments_size = _comments.size();
    size_t comment_pos = 0;
    for (size_t i = 0, csize = _children.size(); i < csize; ++i)
    {
        // serialize comment
        while (comment_pos < comments_size && _comments.at(comment_pos).pos <= i)
        {
            if (tab >= 0)
            {
                writer.write_text("\n");
                for (int j = 0; j < tab + 1; ++j)
                    writer.write_text("\t");
            }
            writer.write_comment(_comments.at(comment_pos).text.c_str());
            ++comment_pos;
            has_child = true;
        }

        // children element
        rc_ptr<XmlElement> c = _children.at(i);
        if (c.is_null())
            continue;
        if (tab >= 0)
            writer.write_text("\n");
        c->serielize(writer, tab >= 0 ? tab + 1 : tab);
        has_child = true;
    }

    // tail comments
    while (comment_pos < comments_size)
    {
        if (tab >= 0)
        {
            writer.write_text("\n");
            for (int j = 0; j < tab + 1; ++j)
                writer.write_text("\t");
        }
        writer.write_comment(_comments.at(comment_pos).text.c_str());
        ++comment_pos;
        has_child = true;
    }

    // finish
    if (has_child && tab >= 0)
    {
        writer.write_text("\n");
        for (int i = 0; i < tab; ++i)
            writer.write_text("\t");
    }
    writer.end_element();
}

}
