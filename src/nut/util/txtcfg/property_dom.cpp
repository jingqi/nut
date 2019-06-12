﻿
#include <assert.h>
#include <stdlib.h>     // for atol() and so on
#include <stdio.h>      // for sprintf()
#include <string.h>     // for memset()

#include "../../platform/platform.h"
#include "../string/string_utils.h"
#include "property_dom.h"


#if NUT_PLATFORM_CC_VC
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

void PropertyDom::Line::clear() noexcept
{
    _space0.clear();
    _key.clear();
    _space1.clear();
    _equal_sign = false;
    _space2.clear();
    _value.clear();
    _space3.clear();
    _comment.clear();
}

void PropertyDom::Line::parse(const std::string& line, const char *line_comment_chars,
                              const char *space_chars) noexcept
{
    assert(nullptr != line_comment_chars && nullptr != space_chars);
    clear();

    std::string s = line;

    // comment
    std::string::size_type index = s.find_first_of(line_comment_chars);
    if (std::string::npos != index)
    {
        _comment = s.substr(index);
        s.erase(index);
    }

    // space0
    index = s.find_first_not_of(space_chars);
    if (std::string::npos != index)
    {
        _space0 = s.substr(0, index - 0);
        s.erase(0, index);
    }
    else
    {
        _space0 = s;
        return;
    }

    // space3
    index = s.find_last_not_of(space_chars);
    if (std::string::npos != index)
    {
        _space3 = s.substr(index + 1);
        s.erase(index + 1);
    }
    else
    {
        _space3 = s;
        return;
    }

    // '='
    index = s.find_first_of('=');
    std::string key, value;
    if (std::string::npos != index)
    {
        _equal_sign = true;
        key = s.substr(0, index - 0);
        value = s.substr(index + 1);
    }
    else
    {
        _equal_sign = false;
        key = s;
    }

    // space1, key
    index = key.find_last_not_of(space_chars);
    if (std::string::npos != index)
    {
        _space1 = key.substr(index + 1);
        _key = key.substr(0, index + 1 - 0);
    }
    else
    {
        _space1 = key;
        _key.clear();
    }

    // space2, value
    index = value.find_first_not_of(space_chars);
    if (std::string::npos != index)
    {
        _space2 = value.substr(0, index - 0);
        _value = value.substr(index);
    }
    else
    {
        _space2 = value;
        _value.clear();
    }
}

std::string PropertyDom::Line::serielize() noexcept
{
    std::string ret;
    ret += _space0;
    ret += _key;
    ret += _space1;
    if (_equal_sign)
        ret.push_back('=');
    ret += _space2;
    ret += _value;
    ret += _space3;
    ret += _comment;
    return ret;
}

void PropertyDom::parse(const std::string& s, const char *line_comment_chars,
                        const char *space_chars) noexcept
{
    assert(nullptr != line_comment_chars && nullptr != space_chars);

    _lines.clear();
    _dirty = true;
    if (s.empty())
        return;

    size_t start = 0;
    do
    {
        size_t i = s.find_first_of("\r\n", start);
        std::string ln;
        if (std::string::npos == i)
        {
            ln = s.substr(start);
        }
        else
        {
            ln = s.substr(start, i - start);
            ++i;
            if (i < s.length() && s.at(i - 1) != s.at(i) &&
                ('\r' == s.at(i) || '\n' == s.at(i)))
                ++i; // 兼容跨平台换行符不同的问题
        }
        start = i;

        rc_ptr<Line> line = rc_new<Line>();
        line->parse(ln, line_comment_chars, space_chars);
        _lines.push_back(std::move(line));
    } while (std::string::npos != start);
}

std::string PropertyDom::serielize(const char *le) const noexcept
{
    assert(nullptr != le);
    std::string ret;
    for (size_t i = 0, sz = _lines.size(); i < sz; ++i)
    {
        if (0 != i)
            ret += le;
        ret += _lines.at(i)->serielize();
    }
    return ret;
}

bool PropertyDom::is_dirty() const noexcept
{
    return _dirty;
}

void PropertyDom::set_dirty(bool dirty) noexcept
{
    _dirty = dirty;
}

void PropertyDom::clear() noexcept
{
    _lines.clear();
    _dirty = true;
}

std::vector<std::string> PropertyDom::list_keys() const noexcept
{
    std::vector<std::string> ret;
    for (size_t i = 0, sz = _lines.size(); i < sz; ++i)
    {
        const rc_ptr<Line>& line = _lines.at(i);
        if (!line->_equal_sign)
            continue;
        ret.push_back(line->_key);
    }
    return ret;
}

bool PropertyDom::has_key(const std::string& key) const noexcept
{
    for (size_t i = 0, sz = _lines.size(); i < sz; ++i)
    {
        if (_lines.at(i)->_key == key)
            return true;
    }
    return false;
}

bool PropertyDom::remove_key(const std::string& key) noexcept
{
    for (size_t i = 0, sz = _lines.size(); i < sz; ++i)
    {
        if (_lines.at(i)->_key == key)
        {
            _lines.erase(_lines.begin() + i);
            _dirty = true;
            return true;
        }
    }
    return false;
}

const char* PropertyDom::get_string(const std::string& key, const char *default_value) const noexcept
{
    for (size_t i = 0, sz = _lines.size(); i < sz; ++i)
    {
        if (_lines.at(i)->_key == key)
            return _lines.at(i)->_value.c_str();
    }
    return default_value;
}

bool PropertyDom::get_bool(const std::string& key, bool default_value) const noexcept
{
    const char *s = get_string(key);
    if (0 == ::strcmp(s, "0") || 0 == stricmp(s, "false") || 0 == stricmp(s, "no"))
        return false;
    if (0 == ::strcmp(s, "1") || 0 == stricmp(s, "true") || 0 == stricmp(s, "yes"))
        return true;
    return default_value;
}

long PropertyDom::get_int(const std::string& key, long default_value) const noexcept
{
    const char *s = get_string(key);
    if (nullptr == s || '\0' == s[0])
        return default_value;

    return ::atol(s);
}

double PropertyDom::get_decimal(const std::string& key, double default_value) const noexcept
{
    const char *s = get_string(key);
    if (nullptr == s || '\0' == s[0])
        return default_value;

    return atof(s);
}

std::vector<std::string> PropertyDom::get_list(const std::string& key, char split_char) const noexcept
{
    std::vector<std::string> ret;
    std::string s = get_string(key);
    if (s.empty())
        return ret;

    std::string::size_type begin = 0, end = s.find_first_of(split_char);
    while (std::string::npos != end)
    {
        ret.push_back(s.substr(begin, end - begin));
        begin = end + 1;
        end = s.find_first_of(split_char, begin);
    }
    ret.push_back(s.substr(begin));
    return ret;
}

void PropertyDom::set_string(const std::string& key, const std::string& value) noexcept
{
    for (size_t i = 0, sz = _lines.size(); i < sz; ++i)
    {
        if (_lines.at(i)->_key == key)
        {
            _lines[i]->_value = value;
            _dirty = true;  // tag the need of saving
            return;
        }
    }

    // if not found, then add a new record
    rc_ptr<Line> line = rc_new<Line>();
    line->_key = key;
    line->_equal_sign = true;
    line->_value = value;
    _lines.push_back(std::move(line));
    _dirty = true;   // tag the need of saving
}

void PropertyDom::set_bool(const std::string& key, bool value) noexcept
{
    set_string(key, (value ? "true" : "false"));
}

void PropertyDom::set_int(const std::string& key, long value) noexcept
{
    const int BUF_LEN = 30;
    char buf[BUF_LEN];
#if NUT_PLATFORM_OS_WINDOWS
    ::ltoa(value, buf, 10);
#else
    safe_snprintf(buf, BUF_LEN, "%ld", value);
#endif
    set_string(key, buf);
}

void PropertyDom::set_decimal(const std::string& key, double value) noexcept
{
    const int BUF_LEN = 30;
    char buf[BUF_LEN];
    safe_snprintf(buf, BUF_LEN, "%lf", value);
    set_string(key, buf);
}

void PropertyDom::set_list(const std::string& key, const std::vector<std::string>& values, char split_char) noexcept
{
    std::string s;
    if (values.size() > 0)
        s = values.at(0);
    for (size_t i = 1, sz = values.size(); i < sz; ++i)
    {
        s.push_back(split_char);
        s += values.at(i);
    }
    set_string(key, s.c_str());
}

}

#if NUT_PLATFORM_CC_VC
#   pragma warning(pop)
#endif
