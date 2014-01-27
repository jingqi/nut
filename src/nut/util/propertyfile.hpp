/**
 * @file -
 * @author jingqi
 * @date 2010-7-9
 */

#ifndef ___HEADFILE___C3889B7F_89FD_4968_A677_E85C8BB7EC22_
#define ___HEADFILE___C3889B7F_89FD_4968_A677_E85C8BB7EC22_

#include <nut/platform/platform.hpp>

#include <fstream>
#include <string>
#include <vector>
#include <assert.h>
#include <stdlib.h>     // for atoi() and so on
#include <stdio.h>      // for sprintf()

#include <nut/gc/gc.hpp>
#include "string/stringutil.hpp"


#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

class PropertyFile
{
    NUT_GC_REFERABLE

    /**
     * 每一行是这样构成的
     * space0 key space1 '=' space2 value space3 comment
     */
    struct Line
    {
        NUT_GC_REFERABLE

        std::string m_space0;
        std::string m_key;
        std::string m_space1;
        bool m_equalSign;
        std::string m_space2;
        std::string m_value;
        std::string m_space3;
        std::string m_comment;

        Line() : m_equalSign(false) {}

        static ref<Line> parsePropertyLine(const std::string& line)
        {
            std::string s = line;
            ref<Line> ret = gc_new<Line>();

            // 注释
            std::string::size_type index = s.find_first_of("#;");
            if (std::string::npos != index)
            {
                ret->m_comment = s.substr(index);
                s.erase(index);
            }
            else
            {
                ret->m_comment.clear();
            }

            // space0
            index = s.find_first_not_of(" \t");
            if (std::string::npos != index)
            {
                ret->m_space0 = s.substr(0, index - 0);
                s.erase(0, index - 0);
            }
            else
            {
                ret->m_space0 = s;
                s.clear();
            }

            // space3
            index = s.find_last_not_of(" \t");
            if (std::string::npos != index)
            {
                ret->m_space3 = s.substr(index + 1);
                s.erase(index + 1);
            }
            else
            {
                ret->m_space3 = s;
                s.clear();
            }

            // '='
            index = s.find_first_of('=');
            std::string strKey, strValue;
            if (std::string::npos != index)
            {
                ret->m_equalSign = true;
                strKey = s.substr(0, index - 0);
                strValue = s.substr(index + 1);
            }
            else
            {
                ret->m_equalSign = false;
                strKey = s;
                strValue.clear();
            }

            // space1, key
            index = strKey.find_last_not_of(" \t");
            if (std::string::npos != index)
            {
                ret->m_space1 = strKey.substr(index + 1);
                ret->m_key = strKey.substr(0, index + 1 - 0);
            }
            else
            {
                ret->m_space1 = strKey;
                ret->m_key.clear();
            }

            // space2, value
            index = strValue.find_first_not_of(" \t");
            if (std::string::npos != index)
            {
                ret->m_space2 = strValue.substr(0, index - 0);
                ret->m_value = strValue.substr(index);
            }
            else
            {
                ret->m_space2 = strValue;
                ret->m_value.clear();
            }
            return ret;
        }

        void writeLine(std::ostream& os)
        {
            os << m_space0 << m_key << m_space1;
            if (m_equalSign)
                os << '=';
            os << m_space2 << m_value << m_space3 << m_comment << std::endl;
        }
    };
    friend class ConfigFile;

    std::string m_filePath;
    std::vector<ref<Line> > m_lines;
    bool m_dirty;

public:
    PropertyFile(const char *filePath)
        : m_filePath(filePath), m_dirty(false)
    {
        assert(NULL != filePath);
        std::ifstream ifs(filePath);
        std::string strLine;

        while (getline(ifs, strLine))
        {
            // 兼容将windows下的换行拿到linux下使用导致的问题
            if (strLine.length() > 0 && '\r' == *strLine.rbegin())
                strLine.erase(strLine.length() - 1);

            ref<Line> line = Line::parsePropertyLine(strLine);
            assert(!line.isNull());
            m_lines.push_back(line);
        }
    }

    ~PropertyFile()
    {
        flush();
    }

    void flush()
    {
        if (m_dirty)
        {
            std::ofstream ofs(m_filePath.c_str());
            for (std::vector<ref<Line> >::const_iterator iter = m_lines.begin(), end = m_lines.end();
                iter != end; ++iter)
            {
                (*iter)->writeLine(ofs);
            }
            m_dirty = false;
        }
    }

    /** set dirty flag */
    void setDirty(bool dirty = true)
    {
        m_dirty = dirty;
    }

    std::vector<std::string> listKeys() const
    {
        std::vector<std::string> ret;
        for (std::vector<ref<Line> >::const_iterator iter = m_lines.begin(), end = m_lines.end();
            iter != end; ++iter)
        {
            if (!(*iter)->m_equalSign)
                continue;
            ret.push_back((*iter)->m_key);
        }
        return ret;
    }

    bool hasKey(const char *key) const
    {
        assert(NULL != key);
        for (std::vector<ref<Line> >::const_iterator iter = m_lines.begin(), end = m_lines.end();
            iter != end; ++iter)
        {
            if ((*iter)->m_key == key)
                return true;
        }
        return false;
    }

    bool removeKey(const char *key)
    {
        for (std::vector<ref<Line> >::iterator iter = m_lines.begin(), end = m_lines.end();
            iter != end; ++iter)
        {
            if ((*iter)->m_key == key)
            {
                m_lines.erase(iter);
                m_dirty = true;
                return true;
            }
        }
        return false;
    }

    std::string getString(const char *key, const char *defaultValue = "") const
    {
        assert(NULL != key && NULL != defaultValue);
        for (std::vector<ref<Line> >::const_iterator iter = m_lines.begin(), end = m_lines.end();
            iter != end; ++iter)
        {
            if ((*iter)->m_key == key)
                return (*iter)->m_value;
        }
        return defaultValue;
    }

    bool getBool(const char *key, bool defaultValue = false) const
    {
        assert(NULL != key);
        std::string s = getString(key);
        if (s == "0" || strieq(s,"false") || strieq(s,"no"))
            return false;
        if (s == "1" || strieq(s,"true") || strieq(s,"yes"))
            return true;
        return defaultValue;
    }

    long getNum(const char *key, long defaultValue = 0) const
    {
        assert(NULL != key);
        std::string s = getString(key);
        if (s.length() == 0)
            return defaultValue;

        long ret = atol(s.c_str());
        return ret;
    }

    double getDecimal(const char *key, double defaultValue = 0.0) const
    {
        assert(NULL != key);
        std::string s = getString(key);
        if (s.length() == 0)
            return defaultValue;

        double ret = atof(s.c_str());
        return ret;
    }

    std::vector<std::string> getList(const char *key, char splitChar = ',') const
    {
        assert(NULL != key);
        std::vector<std::string> ret;
        std::string s = getString(key);
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

    void setString(const char *key, const char *value)
    {
        assert(NULL != key && NULL != value);
        for (std::vector<ref<Line> >::iterator iter = m_lines.begin(), end = m_lines.end();
            iter != end; ++iter)
        {
            if ((*iter)->m_key == key)
            {
                (*iter)->m_value = value;
                m_dirty = true;  // tag the need of saving
                return;
            }
        }

        // if not found, then add a new record
        ref<Line> line = gc_new<Line>();
        line->m_key = key;
        line->m_equalSign = true;
        line->m_value = value;
        m_lines.push_back(line);
        m_dirty = true;   // tag the need of saving
    }

    void setBool(const char *key, bool value)
    {
        assert(NULL != key);
        setString(key, (value ? "true" : "false"));
    }

    void setNum(const char *key, long value)
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
        setString(key, buf);
    }

    void setDecimal(const char *key, double value)
    {
        assert(NULL != key);
        const int BUF_LEN = 30;
        char buf[BUF_LEN];
        ::memset(buf, 0, BUF_LEN);
        ::sprintf(buf, "%lf", value);
        setString(key, buf);
    }

    void setList(const char *key, const std::vector<std::string>& value, char splitChar = ',')
    {
        assert(NULL != key);
        std::string s;
        if (value.size() > 0)
            s = value[0];
        const size_t list_count = value.size();
        for (register size_t i = 1; i < list_count; ++i)
            s += std::string() + splitChar + value.at(i);
        setString(key, s.c_str());
    }
};

}


#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif

#endif /* head file guarder */

