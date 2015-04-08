
#include <assert.h>
#include <stdlib.h>     // for atoi() and so on
#include <stdio.h>      // for sprintf()
#include <string.h>     // for memset()

#include <nut/platform/platform.h>

#include "property_dom.h"
#include "../string/string_util.h"

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

void PropertyDom::Line::clear()
{
    m_space0.clear();
    m_key.clear();
    m_space1.clear();
    m_equal_sign = false;
    m_space2.clear();
    m_value.clear();
    m_space3.clear();
    m_comment.clear();
}

/**
 * @param line 单行字符串，不包含回车换行
 * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如 ';' 行注释和 '#' 行注释
 * @param space_chars 空白字符，其中出现的字符将被视为空白
 */
void PropertyDom::Line::parse(const std::string& line, const char *line_comment_chars, const char *space_chars)
{
    assert(NULL != line_comment_chars && NULL != space_chars);
    clear();

    std::string s = line;

    // comment
    std::string::size_type index = s.find_first_of(line_comment_chars);
    if (std::string::npos != index)
    {
        m_comment = s.substr(index);
        s.erase(index);
    }

    // space0
    index = s.find_first_not_of(space_chars);
    if (std::string::npos != index)
    {
        m_space0 = s.substr(0, index - 0);
        s.erase(0, index);
    }
    else
    {
        m_space0 = s;
        return;
    }

    // space3
    index = s.find_last_not_of(space_chars);
    if (std::string::npos != index)
    {
        m_space3 = s.substr(index + 1);
        s.erase(index + 1);
    }
    else
    {
        m_space3 = s;
        return;
    }

    // '='
    index = s.find_first_of('=');
    std::string key, value;
    if (std::string::npos != index)
    {
        m_equal_sign = true;
        key = s.substr(0, index - 0);
        value = s.substr(index + 1);
    }
    else
    {
        m_equal_sign = false;
        key = s;
    }

    // space1, key
    index = key.find_last_not_of(space_chars);
    if (std::string::npos != index)
    {
        m_space1 = key.substr(index + 1);
        m_key = key.substr(0, index + 1 - 0);
    }
    else
    {
        m_space1 = key;
        m_key.clear();
    }

    // space2, value
    index = value.find_first_not_of(space_chars);
    if (std::string::npos != index)
    {
        m_space2 = value.substr(0, index - 0);
        m_value = value.substr(index);
    }
    else
    {
        m_space2 = value;
        m_value.clear();
    }
}

/**
 * 序列化，不包含尾部的 '\n'
 */
void PropertyDom::Line::serielize(std::string *appended)
{
    assert(NULL != appended);
    *appended += m_space0;
    *appended += m_key;
    *appended += m_space1;
    if (m_equal_sign)
        appended->push_back('=');
    *appended += m_space2;
    *appended += m_value;
    *appended += m_space3;
    *appended += m_comment;
}

PropertyDom::PropertyDom()
    : m_dirty(false)
{}

/**
 * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如 ';' 行注释和 '#' 行注释
 * @param space_chars 空白字符，其中出现的字符将被视为空白
 */
void PropertyDom::parse(const std::string& s, const char *line_comment_chars, const char *space_chars)
{
    assert(NULL != line_comment_chars && NULL != space_chars);

    m_lines.clear();
    m_dirty = true;
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

        rc_ptr<Line> line = RC_NEW(NULL, Line);
        line->parse(ln, line_comment_chars, space_chars);
        m_lines.push_back(line);
    } while (std::string::npos != start);
}

/**
 * @param le 换行符
 */
void PropertyDom::serielize(std::string *appended, const char *le) const
{
    assert(NULL != appended && NULL != le);
    for (size_t i = 0, sz = m_lines.size(); i < sz; ++i)
    {
        if (0 != i)
            *appended += le;
        m_lines.at(i)->serielize(appended);
    }
}

void PropertyDom::clear()
{
    m_lines.clear();
    m_dirty = true;
}

void PropertyDom::list_keys(std::vector<std::string> *rs) const
{
    assert(NULL != rs);
    for (size_t i = 0, sz = m_lines.size(); i < sz; ++i)
    {
        const rc_ptr<Line>& line = m_lines.at(i);
        if (!line->m_equal_sign)
            continue;
        rs->push_back(line->m_key);
    }
}

bool PropertyDom::has_key(const char *key) const
{
    assert(NULL != key);
    for (size_t i = 0, sz = m_lines.size(); i < sz; ++i)
    {
        if (m_lines.at(i)->m_key == key)
            return true;
    }
    return false;
}

bool PropertyDom::remove_key(const char *key)
{
    assert(NULL != key);
    for (size_t i = 0, sz = m_lines.size(); i < sz; ++i)
    {
        if (m_lines.at(i)->m_key == key)
        {
            m_lines.erase(m_lines.begin() + i);
            m_dirty = true;
            return true;
        }
    }
    return false;
}

const char* PropertyDom::get_string(const char *key, const char *default_value) const
{
    assert(NULL != key);
    for (size_t i = 0, sz = m_lines.size(); i < sz; ++i)
    {
        if (m_lines.at(i)->m_key == key)
            return m_lines.at(i)->m_value.c_str();
    }
    return default_value;
}

bool PropertyDom::get_bool(const char *key, bool default_value) const
{
    assert(NULL != key);
    std::string s = get_string(key);
    if (s == "0" || strieq(s,"false") || strieq(s,"no"))
        return false;
    if (s == "1" || strieq(s,"true") || strieq(s,"yes"))
        return true;
    return default_value;
}

long PropertyDom::get_num(const char *key, long default_value) const
{
    assert(NULL != key);
    const char *s = get_string(key);
    if (NULL == s || '\0' == s[0])
        return default_value;

    return atol(s);
}

double PropertyDom::get_decimal(const char *key, double default_value) const
{
    assert(NULL != key);
    const char *s = get_string(key);
    if (NULL == s || '\0' == s[0])
        return default_value;

    return atof(s);
}

void PropertyDom::get_list(const char *key, std::vector<std::string> *rs, char split_char) const
{
    assert(NULL != key && NULL != rs);
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
    assert(NULL != key && NULL != value);
    for (size_t i = 0, sz = m_lines.size(); i < sz; ++i)
    {
        if (m_lines.at(i)->m_key == key)
        {
            m_lines[i]->m_value = value;
            m_dirty = true;  // tag the need of saving
            return;
        }
    }

    // if not found, then add a new record
    rc_ptr<Line> line = RC_NEW(NULL, Line);
    line->m_key = key;
    line->m_equal_sign = true;
    line->m_value = value;
    m_lines.push_back(line);
    m_dirty = true;   // tag the need of saving
}

void PropertyDom::set_bool(const char *key, bool value)
{
    assert(NULL != key);
    set_string(key, (value ? "true" : "false"));
}

void PropertyDom::set_num(const char *key, long value)
{
    assert(NULL != key);
    const int BUF_LEN = 30;
    char buf[BUF_LEN];
    ::memset(buf, 0, BUF_LEN);
#if defined(NUT_PLATFORM_OS_WINDOWS)
    ::ltoa(value, buf, 10);
#else
    ::sprintf(buf, "%ld", value);
#endif
    set_string(key, buf);
}

void PropertyDom::set_decimal(const char *key, double value)
{
    assert(NULL != key);
    const int BUF_LEN = 30;
    char buf[BUF_LEN];
    ::memset(buf, 0, BUF_LEN);
    ::sprintf(buf, "%lf", value);
    set_string(key, buf);
}

void PropertyDom::set_list(const char *key, const std::vector<std::string>& values, char split_char)
{
    assert(NULL != key);
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

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif
