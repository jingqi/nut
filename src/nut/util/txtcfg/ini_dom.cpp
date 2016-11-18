
#include <assert.h>
#include <string.h>

#include <nut/platform/platform.h>

#include "ini_dom.h"
#include "../string/string_util.h"

#if NUT_PLATFORM_CC_VC
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
    ret->_space0 = line.substr(0, index1 - 0);

    if (std::string::npos != index3)
        ret->_space3 = line.substr(index2 + 1, index3 - index2 - 1);
    else
        ret->_space3 = line.substr(index2 + 1);

    if (std::string::npos != index3)
        ret->_comment = line.substr(index3);
    else
        ret->_comment.clear();

    ret->_name = line.substr(index1 + 1, index2 - index1 - 1);

    std::string::size_type index = ret->_name.find_first_not_of(space_chars);
    if (std::string::npos != index)
    {
        ret->_space1 = ret->_name.substr(0, index - 0);
        ret->_name.erase(0, index - 0);
        index = ret->_name.find_last_not_of(space_chars);
        if (std::string::npos != index)
        {
            ret->_space2 = ret->_name.substr(index + 1);
            ret->_name.erase(index + 1);
        }
        else
        {
            ret->_space2.clear();
        }
    }
    else
    {
        ret->_space1 = ret->_name;
        ret->_name.clear();
        ret->_space2.clear();
    }
    return ret;
}

/**
 * @param le 换行符
 */
std::string IniDom::Sector::serielize(const char *le)
{
    assert(NULL != le);
    std::string ret;
    ret += _space0;
    ret.push_back('[');
    ret += _space1;
    ret += _name;
    ret += _space2;
    ret.push_back(']');
    ret += _space3;
    ret += _comment;
    for (size_t i = 0, sz = _lines.size(); i < sz; ++i)
    {
        ret += le;
        ret += _lines.at(i)->serielize();
    }
    return ret;
}

IniDom::IniDom()
{}

/**
 * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如 ';' 行注释和 '#' 行注释
 * @param space_chars 空白字符，其中出现的字符将被视为空白
 */
void IniDom::parse(const std::string& s, const char *line_comment_chars, const char *space_chars)
{
    assert(NULL != line_comment_chars && NULL != space_chars);

    _global_lines.clear();
    _sectors.clear();
    _dirty = true;
    if (s.empty())
        return;

    std::vector<rc_ptr<Line> > *current_lines = &_global_lines;
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
            current_lines = &(sector->_lines);
            _sectors.push_back(sector);
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
std::string IniDom::serielize(const char *le) const
{
    assert(NULL != le);

    // 全局数据
    std::string ret;
    for (size_t i = 0, sz = _global_lines.size(); i < sz; ++i)
    {
        if (0 != i)
            ret += le;
        ret += _global_lines.at(i)->serielize();
    }

    // 各个块
    for (size_t i = 0, sz = _sectors.size(); i < sz; ++i)
    {
        if (0 != i || !_global_lines.empty())
            ret += le;
        ret += _sectors.at(i)->serielize(le);
    }
    return ret;
}

bool IniDom::is_dirty() const
{
    return _dirty;
}

void IniDom::set_dirty(bool dirty)
{
    _dirty = dirty;
}

void IniDom::clear()
{
    _global_lines.clear();
    _sectors.clear();
    _dirty = true;
}

void IniDom::list_sectors(std::vector<std::string> *rs) const
{
    assert(NULL != rs);
    for (size_t i = 0, sz = _sectors.size(); i < sz; ++i)
        rs->push_back(_sectors.at(i)->_name);
}

bool IniDom::has_sector(const char *sector) const
{
    if (NULL == sector)
        return true;

    for (size_t i = 0, sz = _sectors.size(); i < sz; ++i)
    {
        if (_sectors.at(i)->_name == sector)
            return true;
    }
    return false;
}

bool IniDom::remove_sector(const char *sector)
{
    if (NULL == sector)
        return false;

    for (size_t i = 0, sz = _sectors.size(); i < sz; ++i)
    {
        if (_sectors.at(i)->_name == sector)
        {
            _sectors.erase(_sectors.begin() + i);
            _dirty = true;
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
        for (size_t i = 0, sz = _global_lines.size(); i < sz; ++i)
        {
            const rc_ptr<Line>& line = _global_lines.at(i);
            if (!line->_equal_sign)
                continue;
            rs->push_back(line->_key);
        }
        return;
    }

    for (size_t i = 0, sz = _sectors.size(); i < sz; ++i)
    {
        if (_sectors.at(i)->_name == sector)
        {
            const std::vector<rc_ptr<Line> >& lines = _sectors.at(i)->_lines;
            for (size_t j = 0, lsz = lines.size(); j < lsz; ++j)
            {
                const rc_ptr<Line>& line = lines.at(j);
                if (!line->_equal_sign)
                    continue;
                rs->push_back(line->_key);
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
        lines = &_global_lines;
    }
    else
    {
        for (size_t i = 0, sz = _sectors.size(); i < sz; ++i)
        {
            if (_sectors.at(i)->_name == sector)
            {
                lines = &(_sectors.at(i)->_lines);
                break;
            }
        }
    }
    if (NULL == lines)
        return false;

    for (size_t i = 0, sz = lines->size(); i < sz; ++i)
    {
        if (!lines->at(i)->_equal_sign)
            continue;
        if (lines->at(i)->_key == key)
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
        lines = &_global_lines;
    }
    else
    {
        for (size_t i = 0, sz = _sectors.size(); i < sz; ++i)
        {
            if (_sectors.at(i)->_name == sector)
            {
                lines = &(_sectors.at(i)->_lines);
                break;
            }
        }
    }
    if (NULL == lines)
        return false;

    for (size_t i = 0, sz = lines->size(); i < sz; ++i)
    {
        if (lines->at(i)->_key == key)
        {
            lines->erase(lines->begin() + i);
            _dirty = true;
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
        lines = &_global_lines;
    }
    else
    {
        for (size_t i = 0, sz = _sectors.size(); i < sz; ++i)
        {
            if (_sectors.at(i)->_name == sector)
            {
                lines = &(_sectors.at(i)->_lines);
                break;
            }
        }
    }
    if (NULL == lines)
        return default_value;

    for (size_t i = 0, sz = lines->size(); i < sz; ++i)
    {
        if (lines->at(i)->_key == key)
            return lines->at(i)->_value.c_str();
    }
    return default_value;
}

bool IniDom::get_bool(const char *sector, const char *key, bool default_value) const
{
    assert(NULL != key);
    const char *s = get_string(sector, key);
    if (0 == ::strcmp(s, "0") || 0 == stricmp(s, "false") || 0 == stricmp(s, "no"))
        return false;
    if (0 == ::strcmp(s, "1") || 0 == stricmp(s, "true") || 0 == stricmp(s, "yes"))
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
        lines = &_global_lines;
    }
    else
    {
        for (size_t i = 0, sz = _sectors.size(); i < sz; ++i)
        {
            if (_sectors.at(i)->_name == sector)
            {
                lines = &(_sectors.at(i)->_lines);
                break;
            }
        }
    }
    if (NULL == lines)
    {
        rc_ptr<Sector> sec = rc_new<Sector>();
        sec->_name = sector;
        lines = &(sec->_lines);
        _sectors.push_back(sec);
    }

    for (size_t i = 0, sz = lines->size(); i < sz; ++i)
    {
        if (lines->at(i)->_key == key)
        {
            lines->at(i)->_value = value;
            _dirty = true;
            return;
        }
    }
    rc_ptr<Line> line = rc_new<Line>();
    line->_key = key;
    line->_equal_sign = true;
    line->_value = value;
    lines->push_back(line);
    _dirty = true;
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
#if NUT_PLATFORM_OS_WINDOWS
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

#if NUT_PLATFORM_CC_VC
#   pragma warning(pop)
#endif
