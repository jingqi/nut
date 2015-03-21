/**
 * @file -
 * @author jingqi
 * @date 2011-12-25
 * @last-edit 2015-01-27 13:18:51 jingqi
 */

#ifndef ___HEADFILE_C9DB1FAD_B2DA_45F6_AE36_818B4BB68EC1_
#define ___HEADFILE_C9DB1FAD_B2DA_45F6_AE36_818B4BB68EC1_

#include <nut/platform/platform.hpp>
#include <nut/platform/path.hpp>

#include <assert.h>
#include <fstream>
#include <vector>
#include <string>

#include <nut/rc/rc_new.hpp>

#include "property_dom.hpp"


#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

/**
 * .ini 文件 DOM 结构
 */
class IniDom
{
    NUT_REF_COUNTABLE

    typedef PropertyDom::Line Line;

    /**
     * 每个块的行是这样构成的
     * space0 '[' space1 name space2 ']' space3 comment
     */
    struct Sector
    {
        NUT_REF_COUNTABLE

        std::string m_space0;
        std::string m_space1;
        std::string m_name;
        std::string m_space2;
        std::string m_space3;
        std::string m_comment;
        std::vector<rc_ptr<Line> > m_lines;

		static bool contains(const char *s, char c)
		{
			assert(NULL != s);
			for (size_t i = 0; '\0' != s[i]; ++i)
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
        static rc_ptr<Sector> parse_sector_name(const std::string& line, const char *line_comment_chars = ";#", const char *space_chars = " \t")
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

            rc_ptr<Sector> ret = RC_NEW(NULL, Sector);
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
		void serielize(std::string *out, const char *le = "\n")
        {
			assert(NULL != out);
			*out += m_space0;
			out->push_back('[');
			*out += m_space1;
			*out += m_name;
			*out += m_space2;
			out->push_back(']');
			*out += m_space3;
			*out += m_comment;
			for (size_t i = 0, sz = m_lines.size(); i < sz; ++i)
			{
				*out += le;
				m_lines.at(i)->serielize(out);
			}
        }
    };

    std::vector<rc_ptr<Line> > m_global_lines;
    std::vector<rc_ptr<Sector> > m_sectors;
    bool m_dirty;

public:
	IniDom()
		: m_dirty(false)
	{}

	/**
	 * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如 ';' 行注释和 '#' 行注释
	 * @param space_chars 空白字符，其中出现的字符将被视为空白
	 */
	void parse(const std::string& s, const char *line_comment_chars = ";#", const char *space_chars = " \t")
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

            rc_ptr<Line> line = RC_NEW(NULL, Line);
			line->parse(ln, line_comment_chars, space_chars);
			current_lines->push_back(line);
		}
	}

	/**
	 * @param le 换行符
	 */
	void serielize(std::string *out, const char *le = "\n") const
    {
        // 全局数据
		for (size_t i = 0, sz = m_global_lines.size(); i < sz; ++i)
		{
			if (0 != i)
				*out += le;
			m_global_lines.at(i)->serielize(out);
		}

        // 各个块
		for (size_t i = 0, sz = m_sectors.size(); i < sz; ++i)
        {
			if (0 != i || !m_global_lines.empty())
				*out += le;
			m_sectors.at(i)->serielize(out, le);
        }
    }

	bool is_dirty() const
	{
		return m_dirty;
	}

    void set_dirty(bool dirty = true)
    {
        m_dirty = dirty;
    }

    void clear()
    {
        m_global_lines.clear();
        m_sectors.clear();
        m_dirty = true;
    }

    void list_sectors(std::vector<std::string> *out) const
    {
		assert(NULL != out);
		for (size_t i = 0, sz = m_sectors.size(); i < sz; ++i)
			out->push_back(m_sectors.at(i)->m_name);
    }

    bool has_sector(const char *sector) const
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

    bool remove_sector(const char *sector)
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

    void list_keys(const char *sector, std::vector<std::string> *out) const
    {
		assert(NULL != out);
        if (NULL == sector)
        {
			for (size_t i = 0, sz = m_global_lines.size(); i < sz; ++i)
            {
                const rc_ptr<Line>& line = m_global_lines.at(i);
                if (!line->m_equal_sign)
                    continue;
                out->push_back(line->m_key);
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
                    out->push_back(line->m_key);
                }
                return;
            }
        }
    }

    bool has_key(const char *sector, const char *key) const
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

    bool remove_key(const char *sector, const char *key)
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

    const char* get_string(const char *sector, const char *key, const char *default_value = "") const
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

    bool get_bool(const char *sector, const char *key, bool default_value = false) const
    {
        assert(NULL != key);
        std::string s = get_string(sector, key);
        if (s == "0" || strieq(s,"false") || strieq(s,"no"))
            return false;
        if (s == "1" || strieq(s,"true") || strieq(s,"yes"))
            return true;
        return default_value;
    }

    long get_num(const char *sector, const char *key, long default_value = 0) const
    {
        assert(NULL != key);
        const char *s = get_string(sector, key);
        if (NULL == s || '\0' == s[0])
            return default_value;

        return atol(s);
    }

    double get_decimal(const char *sector, const char *key, double default_value = 0.0) const
    {
        assert(NULL != key);
        const char *s = get_string(sector, key);
        if (NULL == s || '\0' == s[0])
            return default_value;

        return atof(s);
    }

    void get_list(const char *sector, const char *key, std::vector<std::string> *out, char split_char = ',') const
    {
        assert(NULL != key && NULL != out);
        std::string s = get_string(sector, key);
        if (s.length() == 0)
            return;

        std::string::size_type begin = 0, end = s.find_first_of(split_char);
        while (end != std::string::npos)
        {
            out->push_back(s.substr(begin, end - begin));
            begin = end + 1;
            end = s.find_first_of(split_char, begin);
        }
        out->push_back(s.substr(begin));
    }

    void set_string(const char *sector, const char *key, const char *value)
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
            rc_ptr<Sector> sec = RC_NEW(NULL, Sector);
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
        rc_ptr<Line> line = RC_NEW(NULL, Line);
        line->m_key = key;
        line->m_equal_sign = true;
        line->m_value = value;
        lines->push_back(line);
        m_dirty = true;
    }

    void set_bool(const char *sector, const char *key, bool value)
    {
        assert(NULL != key);
        set_string(sector, key, (value ? "true" : "false"));
    }

    void set_num(const char *sector, const char *key, long value)
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

    void set_decimal(const char *sector, const char *key, double value)
    {
        assert(NULL != key);
        const int BUF_LEN = 30;
        char buf[BUF_LEN];
        ::memset(buf, 0, BUF_LEN);
        ::sprintf(buf, "%lf", value);
        set_string(sector, key, buf);
    }

    void set_list(const char *sector, const char *key, const std::vector<std::string>& values, char split_char = ',')
    {
        assert(NULL != key);
        std::string s;
        if (values.size() > 0)
            s = values.at(0);
        for (size_t i = 1, sz = values.size(); i < sz; ++i)
            s += std::string() + split_char + values.at(i);
        set_string(sector, key, s.c_str());
    }
};

}


#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif

#endif
