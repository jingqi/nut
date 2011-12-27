/**
 * @file -
 * @author jingqi
 * @date 2011-12-25
 * @last-edit 2011-12-25 13:36:51 jingqi
 */

#ifndef ___HEADFILE_C9DB1FAD_B2DA_45F6_AE36_818B4BB68EC1_
#define ___HEADFILE_C9DB1FAD_B2DA_45F6_AE36_818B4BB68EC1_

#include <assert.h>
#include <fstream>
#include <vector>
#include <string>

#include <nut/gc/gc.hpp>

#include "propertyfile.hpp"

namespace nut
{

class ConfigFile
{
    NUT_GC_REFERABLE

    typedef PropertyFile::Line Line;

    /**
     * 每个块的行是这样构成的
     * space0 '[' space1 name space2 ']' space3 comment
     */
    struct Sector
    {
        NUT_GC_REFERABLE

        std::string m_space0;
        std::string m_space1;
        std::string m_name;
        std::string m_space2;
        std::string m_space3;
        std::string m_comment;
        std::vector<ref<Line> > m_lines;

        static ref<Sector> parseSectorName(const std::string& line)
        {
            const std::string::size_type index1 = line.find_first_of('[');
            const std::string::size_type index2 = line.find_first_of(']');
            if (std::string::npos == index1 || std::string::npos == index2 || index1 > index2)
                return NULL;
            const std::string::size_type index3 = line.find_first_of("#;", index2);

            for (register size_t i = 0; i < index1; ++i)
            {
                const char c = line.at(i);
                if (c != ' ' && c != '\t')
                    return NULL;
            }

            const size_t linelen = line.length();
            for (register size_t i = index2 + 1; i < linelen; ++i)
            {
                const char c = line.at(i);
                if (c == '#' || c == ';')
                    break;
                else if (c != ' ' && c != '\t')
                    return NULL;
            }

            ref<Sector> ret = gc_new<Sector>();
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

            std::string::size_type index = ret->m_name.find_first_not_of(" \t");
            if (std::string::npos != index)
            {
                ret->m_space1 = ret->m_name.substr(0, index - 0);
                ret->m_name.erase(0, index - 0);
                index = ret->m_name.find_last_not_of(" \t");
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

        void writeBlock(std::ostream& os)
        {
            os << m_space0 << '[' << m_space1 << m_name << m_space2 << ']' << m_space3 << m_comment << std::endl;
            for (std::vector<ref<Line> >::const_iterator iter = m_lines.begin(), end = m_lines.end();
                iter != end; ++iter)
            {
                (*iter)->writeLine(os);
            }
        }
    };

    std::string m_filePath;
    std::vector<ref<Line> > m_global_lines;
    std::vector<ref<Sector> > m_sectors;
    bool m_dirty;

public:
    ConfigFile(const char *filePath)
        : m_filePath(filePath), m_dirty(false)
    {
        assert(NULL != filePath);
        std::ifstream ifs(filePath);
        std::string strLine;

        std::vector<ref<Line> > *currentLines = &m_global_lines;

        while (getline(ifs, strLine))
        {
            // 兼容将windows下的换行拿到linux下使用导致的问题
            if (strLine.length() > 0 && '\r' == *strLine.rbegin())
                strLine.erase(strLine.length() - 1);

            ref<Sector> sector = Sector::parseSectorName(strLine);
            if (!sector.isNull())
            {
                currentLines = &(sector->m_lines);
                m_sectors.push_back(sector);
                continue;
            }

            ref<Line> line = PropertyFile::Line::parsePropertyLine(strLine);
            assert(!line.isNull());
            currentLines->push_back(line);
        }
    }

    ~ConfigFile()
    {
        flush();
    }

    void flush()
    {
        if (m_dirty)
        {
            std::ofstream ofs(m_filePath.c_str());
            // 全局数据
            for (std::vector<ref<Line> >::const_iterator iter = m_global_lines.begin(), end = m_global_lines.end();
                iter != end; ++iter)
            {
                (*iter)->writeLine(ofs);
            }
            // 各个块
            for (std::vector<ref<Sector> >::const_iterator iter = m_sectors.begin(), end = m_sectors.end();
                iter != end; ++iter)
            {
                (*iter)->writeBlock(ofs);
            }
            m_dirty = false;
        }
    }

    void setDirty(bool dirty = true)
    {
        m_dirty = dirty;
    }

    std::vector<std::string> listSectors() const
    {
        std::vector<std::string> ret;
        for (std::vector<ref<Sector> >::const_iterator iter = m_sectors.begin(), end = m_sectors.end();
            iter != end; ++iter)
        {
            ret.push_back((*iter)->m_name);
        }
        return ret;
    }

    bool hasSector(const char *sector) const
    {
        if (NULL == sector)
            return true;

        for (std::vector<ref<Sector> >::const_iterator iter = m_sectors.begin(), end = m_sectors.end();
            iter != end; ++iter)
        {
            if ((*iter)->m_name == sector)
                return true;
        }
        return false;
    }

    bool deleteSector(const char *sector)
    {
        if (NULL == sector)
            return false;

        for (std::vector<ref<Sector> >::iterator iter = m_sectors.begin(), end = m_sectors.end();
            iter != end; ++iter)
        {
            if ((*iter)->m_name == sector)
            {
                m_sectors.erase(iter);
                m_dirty = true;
                return true;
            }
        }
        return false;
    }

    std::vector<std::string> listKeys(const char *sector) const
    {
        std::vector<std::string> ret;
        if (NULL == sector)
        {
            for (std::vector<ref<Line> >::const_iterator iter = m_global_lines.begin(), end = m_global_lines.end();
                iter != end; ++iter)
            {
                if (!(*iter)->m_equalSign)
                    continue;
                ret.push_back((*iter)->m_key);
            }
            return ret;
        }

        for (std::vector<ref<Sector> >::const_iterator iter = m_sectors.begin(), end = m_sectors.end();
            iter != end; ++iter)
        {
            if ((*iter)->m_name == sector)
            {
                const std::vector<ref<Line> > lines = (*iter)->m_lines;
                for (std::vector<ref<Line> >::const_iterator iter2 = lines.begin(), end2 = lines.end();
                    iter2 != end2; ++iter2)
                {
                    if (!(*iter)->m_equalSign)
                        continue;
                    ret.push_back((*iter2)->m_key);
                }
                return ret;
            }
        }
        return ret;
    }

    bool hasKey(const char *sector, const char *key) const
    {
        assert(NULL != key);
        const std::vector<ref<Line> > *lines = NULL;
        if (NULL == sector)
        {
            lines = &m_global_lines;
        }
        else
        {
            for (std::vector<ref<Sector> >::const_iterator iter = m_sectors.begin(), end = m_sectors.end();
                iter != end; ++iter)
            {
                if ((*iter)->m_name == sector)
                {
                    lines = &((*iter)->m_lines);
                    break;
                }
            }
        }
        if (NULL == lines)
            return false;

        for (std::vector<ref<Line> >::const_iterator iter = lines->begin(), end = lines->end();
            iter != end; ++iter)
        {
            if (!(*iter)->m_equalSign)
                continue;
            if ((*iter)->m_key == key)
                return true;
        }
        return false;
    }

    bool deleteKey(const char *sector, const char *key)
    {
        assert(NULL != key);
        std::vector<ref<Line> > *lines = NULL;
        if (NULL == sector)
        {
            lines = &m_global_lines;
        }
        else
        {
            for (std::vector<ref<Sector> >::const_iterator iter = m_sectors.begin(), end = m_sectors.end();
                iter != end; ++iter)
            {
                if ((*iter)->m_name == sector)
                {
                    lines = &((*iter)->m_lines);
                    break;
                }
            }
        }
        if (NULL == lines)
            return false;

        for (std::vector<ref<Line> >::iterator iter = lines->begin(), end = lines->end();
            iter != end; ++iter)
        {
            if ((*iter)->m_key == key)
            {
                lines->erase(iter);
                m_dirty = true;
                return true;
            }
        }
        return false;
    }

    std::string getString(const char *sector, const char *key, const char *defaultValue = "") const
    {
        assert(NULL != key && NULL != defaultValue);
        const std::vector<ref<Line> > *lines = NULL;
        if (NULL == sector)
        {
            lines = &m_global_lines;
        }
        else
        {
            for (std::vector<ref<Sector> >::const_iterator iter = m_sectors.begin(), end = m_sectors.end();
                iter != end; ++iter)
            {
                if ((*iter)->m_name == sector)
                {
                    lines = &((*iter)->m_lines);
                    break;
                }
            }
        }
        if (NULL == lines)
            return defaultValue;

        for (std::vector<ref<Line> >::const_iterator iter = lines->begin(), end = lines->end();
            iter != end; ++iter)
        {
            if ((*iter)->m_key == key)
                return (*iter)->m_value;
        }
        return defaultValue;
    }

    bool getBool(const char *sector, const char *key, bool defaultValue = false) const
    {
        assert(NULL != key);
        std::string s = getString(sector, key);
        if (s == "0" || strieq(s,"false") || strieq(s,"no"))
            return false;
        if (s == "1" || strieq(s,"true") || strieq(s,"yes"))
            return true;
        return defaultValue;
    }

    long getNum(const char *sector, const char *key, long defaultValue = 0) const
    {
        assert(NULL != key);
        std::string s = getString(sector, key);
        if (s.length() == 0)
            return defaultValue;

        long ret = atol(s.c_str());
        return ret;
    }

    double getDecimal(const char *sector, const char *key, double defaultValue = 0.0) const
    {
        assert(NULL != key);
        std::string s = getString(sector, key);
        if (s.length() == 0)
            return defaultValue;

        double ret = atof(s.c_str());
        return ret;
    }

    std::vector<std::string> getList(const char *sector, const char *key, char splitChar = ',') const
    {
        assert(NULL != key);
        std::vector<std::string> ret;
        std::string s = getString(sector, key);
        if (s.length() == 0)
            return ret;

        std::string::size_type begin = 0, end = s.find_first_of(splitChar);
        while (end != std::string::npos)
        {
            ret.push_back(s.substr(begin, end - begin));
            begin = end + 1;
            end = s.find_first_of(splitChar, begin);
        }
        ret.push_back(s.substr(begin));
        return ret;
    }

    void setString(const char *sector, const char *key, const char *value)
    {
        assert(NULL != key && NULL != value);
        std::vector<ref<Line> > *lines = NULL;
        if (NULL == sector)
        {
            lines = &m_global_lines;
        }
        else
        {
            for (std::vector<ref<Sector> >::const_iterator iter = m_sectors.begin(), end = m_sectors.end();
                iter != end; ++iter)
            {
                if ((*iter)->m_name == sector)
                {
                    lines = &((*iter)->m_lines);
                    break;
                }
            }
        }
        if (NULL == lines)
        {
            ref<Sector> sec = gc_new<Sector>();
            sec->m_name = sector;
            lines = &(sec->m_lines);
            m_sectors.push_back(sec);
        }

        for (std::vector<ref<Line> >::iterator iter = lines->begin(), end = lines->end();
            iter != end; ++iter)
        {
            if ((*iter)->m_key == key)
            {
                (*iter)->m_value = value;
                m_dirty = true;
                return;
            }
        }
        ref<Line> line = gc_new<Line>();
        line->m_key = key;
        line->m_equalSign = true;
        line->m_value = value;
        lines->push_back(line);
        m_dirty = true;
    }

    void setBool(const char *sector, const char *key, bool value)
    {
        assert(NULL != key);
        setString(sector, key, (value ? "true" : "false"));
    }

    void setNum(const char *sector, const char *key, long value)
    {
        assert(NULL != key);
        const int BUF_LEN = 30;
        char buf[BUF_LEN];
        ::memset(buf, 0, BUF_LEN);
        ::ltoa(value, buf, 10);
        setString(sector, key, buf);
    }

    void setDecimal(const char *sector, const char *key, double value)
    {
        assert(NULL != key);
        const int BUF_LEN = 30;
        char buf[BUF_LEN];
        ::memset(buf, 0, BUF_LEN);
        ::sprintf(buf, "%lf", value);
        setString(sector, key, buf);
    }

    void setList(const char *sector, const char *key, const std::vector<std::string>& value, char splitChar = ',')
    {
        assert(NULL != key);
        std::string s;
        if (value.size() > 0)
            s = value[0];
        const size_t list_count = value.size();
        for (register size_t i = 1; i < list_count; ++i)
            s += std::string() + splitChar + value.at(i);
        setString(sector, key, s.c_str());
    }
};

}

#endif

