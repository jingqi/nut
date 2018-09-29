﻿
#include <assert.h>
#include <sstream>

#include <nut/platform/path.h>

#include "xml_dom.h"

namespace nut
{

XmlDom::XmlDom()
    : _version("1.0"), _encoding("UTF-8")
{}

bool XmlDom::is_dirty() const
{
    if (_dirty)
        return true;
    if (_root.is_null())
        return false;
    return _root->is_dirty();
}

void XmlDom::set_dirty(bool dirty)
{
    if (dirty)
    {
        _dirty = true;
        return;
    }

    _dirty = false;
    if (_root.is_not_null())
        _root->set_dirty(false);
}

const std::string& XmlDom::get_version() const
{
    return _version;
}

void XmlDom::set_version(const std::string& version)
{
    if (version != _version)
    {
        _version = version;
        _dirty = true;
    }
}

const std::string& XmlDom::get_encoding() const
{
    return _encoding;
}

void XmlDom::set_encoding(const std::string& encoding)
{
    if (encoding != _encoding)
    {
        _encoding = encoding;
        _dirty = true;
    }
}

rc_ptr<XmlElement> XmlDom::get_root() const
{
    return _root;
}

void XmlDom::set_root(rc_ptr<XmlElement> root)
{
    if (root != _root)
    {
        _root = root;
        _dirty = true;
    }
}

/**
 * 解析文本为 XML DOM
 *
 * @param ignore_text_blank 忽略文本首尾的空白和换行
 */
void XmlDom::parse(const std::string& s, bool ignore_text_blank)
{
    _version = "1.0";
    _encoding = "UTF-8";
    _root.set_null();
    _dirty = true;

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
    _version = s.substr(start, i - start);
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
    _encoding = s.substr(start, i - start);
    ++i;
    i = s.find("?>", i);
    if (std::string::npos == i)
        return;
    i += 2;

    i = s.find('<', i);
    if (std::string::npos == i)
        return; // no element found
    _root = rc_new<XmlElement>();
    _root->parse(s, i, ignore_text_blank);
    if (_root->get_children_count() > 0)
        _root = _root->get_child(0);
    else
        _root.set_null();
}

/**
 * 将 XML DOM 序列化为文本
 *
 * @param format 格式化输出，以便于阅读
 */
std::string XmlDom::serielize(bool format) const
{
    // xml header
    std::string ret;
    ret += "<?xml version=\"";
    ret += _version;
    ret += "\" encoding=\"";
    ret += _encoding;
    ret += "\"?>";
    if (_root.is_null())
        return ret;
    if (format)
        ret.push_back('\n');

    // xml elements
    std::stringstream ss;
    XmlWriter w(&ss);
    _root->serielize(w, format ? 0 : -1);
    ret += ss.str();
    return ret;
}

}
