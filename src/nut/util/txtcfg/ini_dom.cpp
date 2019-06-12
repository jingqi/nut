
#include <assert.h>
#include <string.h>

#include "../../platform/platform.h"
#include "../string/string_utils.h"
#include "ini_dom.h"


#if NUT_PLATFORM_CC_VC
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

static bool contains(const char *s, char c) noexcept
{
    assert(nullptr != s);
    for (size_t i = 0; 0 != s[i]; ++i)
    {
        if (s[i] == c)
            return true;
    }
    return false;
}

rc_ptr<IniDom::Sector> IniDom::Sector::parse_sector_name(
    const std::string& line, const char *line_comment_chars, const char *space_chars) noexcept
{
    const std::string::size_type index1 = line.find_first_of('[');
    const std::string::size_type index2 = line.find_first_of(']');
    if (std::string::npos == index1 || std::string::npos == index2 || index1 > index2)
        return rc_ptr<Sector>(nullptr);
    const std::string::size_type index3 = line.find_first_of(line_comment_chars, index2);

    for (size_t i = 0; i < index1; ++i)
    {
        if (!contains(space_chars, line.at(i)))
            return rc_ptr<Sector>(nullptr);
    }

    for (size_t i = index2 + 1, len = line.length(); i < len && i != index3; ++i)
    {
        if (!contains(space_chars, line.at(i)))
            return rc_ptr<Sector>(nullptr);
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
std::string IniDom::Sector::serielize(const char *le) noexcept
{
    assert(nullptr != le);
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

/**
 * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如 ';' 行注释和 '#' 行注释
 * @param space_chars 空白字符，其中出现的字符将被视为空白
 */
void IniDom::parse(const std::string& s, const char *line_comment_chars, const char *space_chars) noexcept
{
    assert(nullptr != line_comment_chars && nullptr != space_chars);

    _anonymous_lines.clear();
    _sectors.clear();
    _dirty = true;
    if (s.empty())
        return;

    std::vector<rc_ptr<Line> > *current_lines = &_anonymous_lines;
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
            _sectors.push_back(std::move(sector));
            continue;
        }

        rc_ptr<Line> line = rc_new<Line>();
        line->parse(ln, line_comment_chars, space_chars);
        current_lines->push_back(std::move(line));
    }
}

/**
 * @param le 换行符
 */
std::string IniDom::serielize(const char *le) const noexcept
{
    assert(nullptr != le);

    // 全局数据
    std::string ret;
    for (size_t i = 0, sz = _anonymous_lines.size(); i < sz; ++i)
    {
        if (0 != i)
            ret += le;
        ret += _anonymous_lines.at(i)->serielize();
    }

    // 各个块
    for (size_t i = 0, sz = _sectors.size(); i < sz; ++i)
    {
        if (0 != i || !_anonymous_lines.empty())
            ret += le;
        ret += _sectors.at(i)->serielize(le);
    }
    return ret;
}

bool IniDom::is_dirty() const noexcept
{
    return _dirty;
}

void IniDom::set_dirty(bool dirty) noexcept
{
    _dirty = dirty;
}

void IniDom::clear() noexcept
{
    _anonymous_lines.clear();
    _sectors.clear();
    _dirty = true;
}

std::vector<std::string> IniDom::list_sectors() const noexcept
{
    std::vector<std::string> ret;
    for (size_t i = 0, sz = _sectors.size(); i < sz; ++i)
        ret.push_back(_sectors.at(i)->_name);
    return ret;
}

bool IniDom::has_sector(const std::string& sector) const noexcept
{
    if (sector.empty())
        return true; // anonymous
    for (size_t i = 0, sz = _sectors.size(); i < sz; ++i)
    {
        if (_sectors.at(i)->_name == sector)
            return true;
    }
    return false;
}

bool IniDom::remove_sector(const std::string& sector) noexcept
{
    if (sector.empty())
        return false; // anonymous

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

std::vector<std::string> IniDom::list_keys(const std::string& sector) const noexcept
{
    std::vector<std::string> ret;
    if (sector.empty()) // anonymous
    {
        for (size_t i = 0, sz = _anonymous_lines.size(); i < sz; ++i)
        {
            const rc_ptr<Line>& line = _anonymous_lines.at(i);
            if (!line->_equal_sign)
                continue;
            ret.push_back(line->_key);
        }
        return ret;
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
                ret.push_back(line->_key);
            }
            return ret;
        }
    }
    return ret;
}

bool IniDom::has_key(const std::string& sector, const std::string& key) const noexcept
{
    const std::vector<rc_ptr<Line>> *lines = nullptr;
    if (sector.empty())
    {
        lines = &_anonymous_lines;
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
    if (nullptr == lines)
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

bool IniDom::remove_key(const std::string& sector, const std::string& key) noexcept
{
    std::vector<rc_ptr<Line> > *lines = nullptr;
    if (sector.empty())
    {
        lines = &_anonymous_lines;
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
    if (nullptr == lines)
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

const char* IniDom::get_string(const std::string& sector, const std::string& key,
                               const char *default_value) const noexcept
{
    const std::vector<rc_ptr<Line> > *lines = nullptr;
    if (sector.empty())
    {
        lines = &_anonymous_lines;
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
    if (nullptr == lines)
        return default_value;

    for (size_t i = 0, sz = lines->size(); i < sz; ++i)
    {
        if (lines->at(i)->_key == key)
            return lines->at(i)->_value.c_str();
    }
    return default_value;
}

bool IniDom::get_bool(const std::string& sector, const std::string& key, bool default_value) const noexcept
{
    const char *s = get_string(sector, key);
    if (0 == ::strcmp(s, "0") || 0 == stricmp(s, "false") || 0 == stricmp(s, "no"))
        return false;
    if (0 == ::strcmp(s, "1") || 0 == stricmp(s, "true") || 0 == stricmp(s, "yes"))
        return true;
    return default_value;
}

long IniDom::get_int(const std::string& sector, const std::string& key, long default_value) const noexcept
{
    const char *s = get_string(sector, key);
    if (nullptr == s || '\0' == s[0])
        return default_value;

    return atol(s);
}

double IniDom::get_decimal(const std::string& sector, const std::string& key, double default_value) const noexcept
{
    const char *s = get_string(sector, key);
    if (nullptr == s || '\0' == s[0])
        return default_value;

    return atof(s);
}

std::vector<std::string> IniDom::get_list(const std::string& sector, const std::string& key, char split_char) const noexcept
{
    std::vector<std::string> ret;
    std::string s = get_string(sector, key);
    if (s.length() == 0)
        return ret;

    std::string::size_type begin = 0, end = s.find_first_of(split_char);
    while (end != std::string::npos)
    {
        ret.push_back(s.substr(begin, end - begin));
        begin = end + 1;
        end = s.find_first_of(split_char, begin);
    }
    ret.push_back(s.substr(begin));
    return ret;
}

void IniDom::set_string(const std::string& sector, const std::string& key, const std::string& value) noexcept
{
    std::vector<rc_ptr<Line>> *lines = nullptr;
    if (sector.empty())
    {
        lines = &_anonymous_lines;
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
    if (nullptr == lines)
    {
        rc_ptr<Sector> sec = rc_new<Sector>();
        sec->_name = sector;
        lines = &(sec->_lines);
        _sectors.push_back(std::move(sec));
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
    lines->push_back(std::move(line));
    _dirty = true;
}

void IniDom::set_bool(const std::string& sector, const std::string& key, bool value) noexcept
{
    set_string(sector, key, (value ? "true" : "false"));
}

void IniDom::set_int(const std::string& sector, const std::string& key, long value) noexcept
{
    const int BUF_LEN = 30;
    char buf[BUF_LEN];
#if NUT_PLATFORM_OS_WINDOWS
    ::ltoa(value, buf, 10);
#else
    safe_snprintf(buf, BUF_LEN, "%ld", value);
#endif
    set_string(sector, key, buf);
}

void IniDom::set_decimal(const std::string& sector, const std::string& key, double value) noexcept
{
    const int BUF_LEN = 30;
    char buf[BUF_LEN];
    safe_snprintf(buf, BUF_LEN, "%lf", value);
    set_string(sector, key, buf);
}

void IniDom::set_list(const std::string& sector, const std::string& key,
                      const std::vector<std::string>& values, char split_char) noexcept
{
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
