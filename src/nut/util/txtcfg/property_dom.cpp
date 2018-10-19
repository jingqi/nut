
#include <assert.h>
#include <stdlib.h>     // for atoi() and so on
#include <stdio.h>      // for sprintf()
#include <string.h>     // for memset()

#include <nut/platform/platform.h>

#include "property_dom.h"
#include "../string/string_utils.h"

#if NUT_PLATFORM_CC_VC
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

void PropertyDom::Line::clear()
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

/**
 * @param line 单行字符串，不包含回车换行
 * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如 ';' 行注释和 '#' 行注释
 * @param space_chars 空白字符，其中出现的字符将被视为空白
 */
void PropertyDom::Line::parse(const std::string& line, const char *line_comment_chars, const char *space_chars)
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

/**
 * 序列化，不包含尾部的 '\n'
 */
std::string PropertyDom::Line::serielize()
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

/**
 * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如 ';' 行注释和 '#' 行注释
 * @param space_chars 空白字符，其中出现的字符将被视为空白
 */
void PropertyDom::parse(const std::string& s, const char *line_comment_chars, const char *space_chars)
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
        _lines.push_back(line);
    } while (std::string::npos != start);
}

/**
 * @param le 换行符
 */
std::string PropertyDom::serielize(const char *le) const
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

bool PropertyDom::is_dirty() const
{
    return _dirty;
}

void PropertyDom::set_dirty(bool dirty)
{
    _dirty = dirty;
}

void PropertyDom::clear()
{
    _lines.clear();
    _dirty = true;
}

void PropertyDom::list_keys(std::vector<std::string> *rs) const
{
    assert(nullptr != rs);
    for (size_t i = 0, sz = _lines.size(); i < sz; ++i)
    {
        const rc_ptr<Line>& line = _lines.at(i);
        if (!line->_equal_sign)
            continue;
        rs->push_back(line->_key);
    }
}

bool PropertyDom::has_key(const char *key) const
{
    assert(nullptr != key);
    for (size_t i = 0, sz = _lines.size(); i < sz; ++i)
    {
        if (_lines.at(i)->_key == key)
            return true;
    }
    return false;
}

bool PropertyDom::remove_key(const char *key)
{
    assert(nullptr != key);
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

const char* PropertyDom::get_string(const char *key, const char *default_value) const
{
    assert(nullptr != key);
    for (size_t i = 0, sz = _lines.size(); i < sz; ++i)
    {
        if (_lines.at(i)->_key == key)
            return _lines.at(i)->_value.c_str();
    }
    return default_value;
}

bool PropertyDom::get_bool(const char *key, bool default_value) const
{
    assert(nullptr != key);
    const char *s = get_string(key);
    if (0 == ::strcmp(s, "0") || 0 == stricmp(s, "false") || 0 == stricmp(s, "no"))
        return false;
    if (0 == ::strcmp(s, "1") || 0 == stricmp(s, "true") || 0 == stricmp(s, "yes"))
        return true;
    return default_value;
}

long PropertyDom::get_num(const char *key, long default_value) const
{
    assert(nullptr != key);
    const char *s = get_string(key);
    if (nullptr == s || '\0' == s[0])
        return default_value;

    return atol(s);
}

double PropertyDom::get_decimal(const char *key, double default_value) const
{
    assert(nullptr != key);
    const char *s = get_string(key);
    if (nullptr == s || '\0' == s[0])
        return default_value;

    return atof(s);
}

void PropertyDom::get_list(const char *key, std::vector<std::string> *rs, char split_char) const
{
    assert(nullptr != key && nullptr != rs);
    std::string s = get_string(key);
    if (s.empty())
        return;

    std::string::size_type begin = 0, end = s.find_first_of(split_char);
    while (std::string::npos != end)
    {
        rs->push_back(s.substr(begin, end - begin));
        begin = end + 1;
        end = s.find_first_of(split_char, begin);
    }
    rs->push_back(s.substr(begin));
}

void PropertyDom::set_string(const char *key, const char *value)
{
    assert(nullptr != key && nullptr != value);
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
    _lines.push_back(line);
    _dirty = true;   // tag the need of saving
}

void PropertyDom::set_bool(const char *key, bool value)
{
    assert(nullptr != key);
    set_string(key, (value ? "true" : "false"));
}

void PropertyDom::set_num(const char *key, long value)
{
    assert(nullptr != key);
    const int BUF_LEN = 30;
    char buf[BUF_LEN];
#if NUT_PLATFORM_OS_WINDOWS
    ::ltoa(value, buf, 10);
#else
    safe_snprintf(buf, BUF_LEN, "%ld", value);
#endif
    set_string(key, buf);
}

void PropertyDom::set_decimal(const char *key, double value)
{
    assert(nullptr != key);
    const int BUF_LEN = 30;
    char buf[BUF_LEN];
    safe_snprintf(buf, BUF_LEN, "%lf", value);
    set_string(key, buf);
}

void PropertyDom::set_list(const char *key, const std::vector<std::string>& values, char split_char)
{
    assert(nullptr != key);
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
