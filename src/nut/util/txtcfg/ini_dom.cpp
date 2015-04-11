
#include <assert.h>
#include <string.h>

#include <nut/platform/platform.h>

#include "ini_dom.h"
#include "../string/string_util.h"

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

static bool contains(const char *s, char c)
{
    assert(NULL != s);
    for (size_t i = 0; 0 != s[i]; ++i)
    {
        if (s[i] == c)
            return true;
    }
    return false;
}

/**
 * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如 ';' 行注释和 '#' 行注释
 * @param space_chars 空白字符，其中出现的字符将被视为空白
 */
rc_ptr<IniDom::Sector> IniDom::Sector::parse_sector_name(const std::string& line, const char *line_comment_chars, const char *space_chars)
{
    const std::string::size_type index1 = line.find_first_of('[');
    const std::string::size_type index2 = line.find_first_of(']');
    if (std::string::npos == index1 || std::string::npos == index2 || index1 > index2)
        return rc_ptr<Sector>(NULL);
    const std::string::size_type index3 = line.find_first_of(line_comment_chars, index2);

    for (size_t i = 0; i < index1; ++i)
    {
        if (!contains(space_chars, line.at(i)))
            return rc_ptr<Sector>(NULL);
    }

    for (size_t i = index2 + 1, len = line.length(); i < len && i != index3; ++i)
    {
        if (!contains(space_chars, line.at(i)))
            return rc_ptr<Sector>(NULL);
    }

    rc_ptr<Sector> ret = rc_new<Sector>();
    ret->m_space0 = line.substr(0, index1 - 0);

    if (std::string::npos != index3)
        ret->m_space3 = line.substr(index2 + 1, index3 - index2 - 1);
    else
        ret->m_space3 = line.substr(index2 + 1);

    if (std::string::npos != index3)
        ret->m_comment = line.substr(index3);
    else
        ret->m_comment.clear();

    ret->m_name = line.substr(index1 + 1, index2 - index1 - 1);

    std::string::size_type index = ret->m_name.find_first_not_of(space_chars);
    if (std::string::npos != index)
    {
        ret->m_space1 = ret->m_name.substr(0, index - 0);
        ret->m_name.erase(0, index - 0);
        index = ret->m_name.find_last_not_of(space_chars);
        if (std::string::npos != index)
        {
            ret->m_space2 = ret->m_name.substr(index + 1);
            ret->m_name.erase(index + 1);
        }
        else
        {
            ret->m_space2.clear();
        }
    }
    else
    {
        ret->m_space1 = ret->m_name;
        ret->m_name.clear();
        ret->m_space2.clear();
    }
    return ret;
}

/**
 * @param le 换行符
 */
void IniDom::Sector::serielize(std::string *appended, const char *le)
{
    assert(NULL != appended);
    *appended += m_space0;
    appended->push_back('[');
    *appended += m_space1;
    *appended += m_name;
    *appended += m_space2;
    appended->push_back(']');
    *appended += m_space3;
    *appended += m_comment;
    for (size_t i = 0, sz = m_lines.size(); i < sz; ++i)
    {
        *appended += le;
        m_lines.at(i)->serielize(appended);
    }
}

IniDom::IniDom()
    : m_dirty(false)
{}

/**
 * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如 ';' 行注释和 '#' 行注释
 * @param space_chars 空白字符，其中出现的字符将被视为空白
 */
void IniDom::parse(const std::string& s, const char *line_comment_chars, const char *space_chars)
{
    assert(NULL != line_comment_chars && NULL != space_chars);

    m_global_lines.clear();
    m_sectors.clear();
    m_dirty = true;
    if (s.empty())
        return;

    std::vector<rc_ptr<Line> > *current_lines = &m_global_lines;
    size_t start = 0;
    while (std::string::npos != start)
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

        rc_ptr<Sector> sector = Sector::parse_sector_name(ln, line_comment_chars, space_chars);
        if (sector.is_not_null())
        {
            current_lines = &(sector->m_lines);
            m_sectors.push_back(sector);
            continue;
        }

        rc_ptr<Line> line = rc_new<Line>();
        line->parse(ln, line_comment_chars, space_chars);
        current_lines->push_back(line);
    }
}

/**
 * @param le 换行符
 */
void IniDom::serielize(std::string *appended, const char *le) const
{
    // 全局数据
    for (size_t i = 0, sz = m_global_lines.size(); i < sz; ++i)
    {
        if (0 != i)
            *appended += le;
        m_global_lines.at(i)->serielize(appended);
    }

    // 各个块
    for (size_t i = 0, sz = m_sectors.size(); i < sz; ++i)
    {
        if (0 != i || !m_global_lines.empty())
            *appended += le;
        m_sectors.at(i)->serielize(appended, le);
    }
}

void IniDom::clear()
{
    m_global_lines.clear();
    m_sectors.clear();
    m_dirty = true;
}

void IniDom::list_sectors(std::vector<std::string> *rs) const
{
    assert(NULL != rs);
    for (size_t i = 0, sz = m_sectors.size(); i < sz; ++i)
        rs->push_back(m_sectors.at(i)->m_name);
}

bool IniDom::has_sector(const char *sector) const
{
    if (NULL == sector)
        return true;

    for (size_t i = 0, sz = m_sectors.size(); i < sz; ++i)
    {
        if (m_sectors.at(i)->m_name == sector)
            return true;
    }
    return false;
}

bool IniDom::remove_sector(const char *sector)
{
    if (NULL == sector)
        return false;

    for (size_t i = 0, sz = m_sectors.size(); i < sz; ++i)
    {
        if (m_sectors.at(i)->m_name == sector)
        {
            m_sectors.erase(m_sectors.begin() + i);
            m_dirty = true;
            return true;
        }
    }
    return false;
}

void IniDom::list_keys(const char *sector, std::vector<std::string> *rs) const
{
    assert(NULL != rs);
    if (NULL == sector)
    {
        for (size_t i = 0, sz = m_global_lines.size(); i < sz; ++i)
        {
            const rc_ptr<Line>& line = m_global_lines.at(i);
            if (!line->m_equal_sign)
                continue;
            rs->push_back(line->m_key);
        }
        return;
    }

    for (size_t i = 0, sz = m_sectors.size(); i < sz; ++i)
    {
        if (m_sectors.at(i)->m_name == sector)
        {
            const std::vector<rc_ptr<Line> >& lines = m_sectors.at(i)->m_lines;
            for (size_t j = 0, lsz = lines.size(); j < lsz; ++j)
            {
                const rc_ptr<Line>& line = lines.at(j);
                if (!line->m_equal_sign)
                    continue;
                rs->push_back(line->m_key);
            }
            return;
        }
    }
}

bool IniDom::has_key(const char *sector, const char *key) const
{
    assert(NULL != key);
    const std::vector<rc_ptr<Line> > *lines = NULL;
    if (NULL == sector)
    {
        lines = &m_global_lines;
    }
    else
    {
        for (size_t i = 0, sz = m_sectors.size(); i < sz; ++i)
        {
            if (m_sectors.at(i)->m_name == sector)
            {
                lines = &(m_sectors.at(i)->m_lines);
                break;
            }
        }
    }
    if (NULL == lines)
        return false;

    for (size_t i = 0, sz = lines->size(); i < sz; ++i)
    {
        if (!lines->at(i)->m_equal_sign)
            continue;
        if (lines->at(i)->m_key == key)
            return true;
    }
    return false;
}

bool IniDom::remove_key(const char *sector, const char *key)
{
    assert(NULL != key);
    std::vector<rc_ptr<Line> > *lines = NULL;
    if (NULL == sector)
    {
        lines = &m_global_lines;
    }
    else
    {
        for (size_t i = 0, sz = m_sectors.size(); i < sz; ++i)
        {
            if (m_sectors.at(i)->m_name == sector)
            {
                lines = &(m_sectors.at(i)->m_lines);
                break;
            }
        }
    }
    if (NULL == lines)
        return false;

    for (size_t i = 0, sz = lines->size(); i < sz; ++i)
    {
        if (lines->at(i)->m_key == key)
        {
            lines->erase(lines->begin() + i);
            m_dirty = true;
            return true;
        }
    }
    return false;
}

const char* IniDom::get_string(const char *sector, const char *key, const char *default_value) const
{
    assert(NULL != key);
    const std::vector<rc_ptr<Line> > *lines = NULL;
    if (NULL == sector)
    {
        lines = &m_global_lines;
    }
    else
    {
        for (size_t i = 0, sz = m_sectors.size(); i < sz; ++i)
        {
            if (m_sectors.at(i)->m_name == sector)
            {
                lines = &(m_sectors.at(i)->m_lines);
                break;
            }
        }
    }
    if (NULL == lines)
        return default_value;

    for (size_t i = 0, sz = lines->size(); i < sz; ++i)
    {
        if (lines->at(i)->m_key == key)
            return lines->at(i)->m_value.c_str();
    }
    return default_value;
}

bool IniDom::get_bool(const char *sector, const char *key, bool default_value) const
{
    assert(NULL != key);
    std::string s = get_string(sector, key);
    if (s == "0" || strieq(s,"false") || strieq(s,"no"))
        return false;
    if (s == "1" || strieq(s,"true") || strieq(s,"yes"))
        return true;
    return default_value;
}

long IniDom::get_num(const char *sector, const char *key, long default_value) const
{
    assert(NULL != key);
    const char *s = get_string(sector, key);
    if (NULL == s || '\0' == s[0])
        return default_value;

    return atol(s);
}

double IniDom::get_decimal(const char *sector, const char *key, double default_value) const
{
    assert(NULL != key);
    const char *s = get_string(sector, key);
    if (NULL == s || '\0' == s[0])
        return default_value;

    return atof(s);
}

void IniDom::get_list(const char *sector, const char *key, std::vector<std::string> *rs, char split_char) const
{
    assert(NULL != key && NULL != rs);
    std::string s = get_string(sector, key);
    if (s.length() == 0)
        return;

    std::string::size_type begin = 0, end = s.find_first_of(split_char);
    while (end != std::string::npos)
    {
        rs->push_back(s.substr(begin, end - begin));
        begin = end + 1;
        end = s.find_first_of(split_char, begin);
    }
    rs->push_back(s.substr(begin));
}

void IniDom::set_string(const char *sector, const char *key, const char *value)
{
    assert(NULL != key && NULL != value);
    std::vector<rc_ptr<Line> > *lines = NULL;
    if (NULL == sector)
    {
        lines = &m_global_lines;
    }
    else
    {
        for (size_t i = 0, sz = m_sectors.size(); i < sz; ++i)
        {
            if (m_sectors.at(i)->m_name == sector)
            {
                lines = &(m_sectors.at(i)->m_lines);
                break;
            }
        }
    }
    if (NULL == lines)
    {
        rc_ptr<Sector> sec = rc_new<Sector>();
        sec->m_name = sector;
        lines = &(sec->m_lines);
        m_sectors.push_back(sec);
    }

    for (size_t i = 0, sz = lines->size(); i < sz; ++i)
    {
        if (lines->at(i)->m_key == key)
        {
            lines->at(i)->m_value = value;
            m_dirty = true;
            return;
        }
    }
    rc_ptr<Line> line = rc_new<Line>();
    line->m_key = key;
    line->m_equal_sign = true;
    line->m_value = value;
    lines->push_back(line);
    m_dirty = true;
}

void IniDom::set_bool(const char *sector, const char *key, bool value)
{
    assert(NULL != key);
    set_string(sector, key, (value ? "true" : "false"));
}

void IniDom::set_num(const char *sector, const char *key, long value)
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
    set_string(sector, key, buf);
}

void IniDom::set_decimal(const char *sector, const char *key, double value)
{
    assert(NULL != key);
    const int BUF_LEN = 30;
    char buf[BUF_LEN];
    ::memset(buf, 0, BUF_LEN);
    ::sprintf(buf, "%lf", value);
    set_string(sector, key, buf);
}

void IniDom::set_list(const char *sector, const char *key, const std::vector<std::string>& values, char split_char)
{
    assert(NULL != key);
    std::string s;
    if (values.size() > 0)
        s = values.at(0);
    for (size_t i = 1, sz = values.size(); i < sz; ++i)
        s += std::string() + split_char + values.at(i);
    set_string(sector, key, s.c_str());
}

}

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif
