/**
 * @file -
 * @author jingqi
 * @date 2010-7-9
 */

#ifndef ___HEADFILE___C3889B7F_89FD_4968_A677_E85C8BB7EC22_
#define ___HEADFILE___C3889B7F_89FD_4968_A677_E85C8BB7EC22_

#include <fstream>
#include <string>
#include <vector>
#include <assert.h>
#include <stdlib.h>     // for atoi() and so on
#include <stdio.h>      // for sprintf()

namespace nut
{

class ConfigFile
{
    /**
     * 每一行是这样构成的
     * [space0]key[space1]=[space2]value[space3]#comment
     */
    struct Line
    {
        int m_space0;           /* space0 的空格个数 */
        std::string m_key;      /* key */
        int m_space1;           /* space1 的空格个数 */
        bool m_equalSign;       /* 等号 */
        int m_space2;           /* space2 的空格个数 */
        std::string m_value;    /* value */
        int m_space3;           /* space3 的空格个数 */
        std::string m_comment;  /* 注释 */

        Line ()
            : m_space0(0),m_space1(0),m_equalSign(false),m_space2(0),m_space3(0)
        {}
    };

    std::string m_filePath;
    std::vector<Line> m_document;
    bool m_needToSave;

    /// test if two string are equal (ignore cases)
    static bool strieq(std::string str0, const char * str1)
    {
        assert(NULL != str1);
        size_t i = 0;
        while (i < str0.length() && (str0[i] | 0x20) == (str1[i] | 0x20))
            ++i;
        if (i == str0.length() && str1[i] == '\0')
            return true;
        return false;
    }

public :
    ConfigFile (const char *filePath)
        : m_filePath(filePath), m_needToSave(false)
    {
        assert(NULL != filePath);
        std::ifstream ifs(filePath);
        std::string strLine;

        while (getline(ifs, strLine))
        {
            Line line;

            // 兼容将windows下的换行拿到linux下使用导致的问题
            if (strLine.length() > 0 && '\r' == *strLine.rbegin())
                strLine.erase(strLine.length() - 1);

            // 注释
            std::string::size_type index = strLine.find_first_of('#');
            if (index != std::string::npos)
            {
                line.m_comment = strLine.substr(index);
                strLine.erase(index);
            }

            // space0
            index = strLine.find_first_not_of(" \t");
            if (index != std::string::npos)
            {
                line.m_space0 = index;
                strLine.erase(0, index);
            }
            else
            {
                line.m_space0 = strLine.length();
                strLine.erase();
            }

            // space3
            index = strLine.find_last_not_of(" \t");
            if (index != std::string::npos)
            {
                line.m_space3 = strLine.length() - index - 1;
                strLine.erase(index + 1);
            }
            else
            {
                line.m_space3 = strLine.length();
                strLine.erase();
            }

            // '='
            index = strLine.find_first_of('=');
            std::string strKey, strValue;
            if (index != std::string::npos)
            {
                line.m_equalSign = true;
                strKey = strLine.substr(0, index);
                strValue = strLine.substr(index + 1);
            }
            else
            {
                line.m_equalSign = false;
                strKey = strLine;
                strValue.erase();
            }

            // space1, key
            index = strKey.find_last_not_of(" \t");
            if (index != std::string::npos)
            {
                line.m_space1 = strKey.length() - index - 1;
                line.m_key = strKey.substr(0, index + 1);
            }
            else
            {
                line.m_space1 = strKey.length();
            }

            // space2, value
            index = strValue.find_first_not_of(" \t");
            if (index != std::string::npos)
            {
                line.m_space2 = index;
                line.m_value = strValue.substr(index);
            }

            m_document.push_back(line);
        }
    }

    ~ConfigFile ()
    {
        flush();
    }

    void flush()
    {
        if (m_needToSave)
        {
            std::ofstream ofs(m_filePath.c_str());
            for (std::vector<Line>::const_iterator it = m_document.begin(); it != m_document.end(); ++it)
            {
                for (int j = 0; j < it->m_space0; ++j)
                    ofs << ' ';
                ofs << it->m_key;
                for (int j = 0; j < it->m_space1; ++j)
                    ofs << ' ';
                if (it->m_equalSign)
                    ofs << '=';
                for (int j = 0; j < it->m_space2; ++j)
                    ofs << ' ';
                ofs << it->m_value;
                for (int j = 0; j < it->m_space3; ++j)
                    ofs << ' ';
                ofs << it->m_comment << std::endl;
            }
            m_needToSave = false;
        }
    }

    std::string readString(const char *cfgName, const char *defaultValue = "") const
    {
        for (std::vector<Line>::const_iterator it = m_document.begin(); it != m_document.end(); ++it)
        {
            if (it->m_key == cfgName)
                return it->m_value;
        }
        return defaultValue;
    }

    bool readBool (const char *cfgName, bool defaultValue = false) const
    {
        std::string s = readString(cfgName);
        if (s == "0" || strieq(s,"false") || strieq(s,"no"))
            return false;
        if (s == "1" || strieq(s,"true") || strieq(s,"yes"))
            return true;
        return defaultValue;
    }

    long readNum (const char *cfgName, long defaultValue = 0) const
    {
        std::string s = readString(cfgName);
        if (s.length() == 0)
            return defaultValue;

        long ret = atol(s.c_str());
        return ret;
    }

    double readDecimal (const char *cfgName, double defaultValue = 0.0) const
    {
        std::string s = readString(cfgName);
        if (s.length() == 0)
            return defaultValue;

        double ret = atof(s.c_str());
        return ret;
    }

    std::vector<std::string> readList(const char *cfgName, char splitChar = ',') const
    {
        std::vector<std::string> ret;
        std::string s = readString(cfgName);
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

    void writeString(const char * cfgName, const char * value)
    {
        for (std::vector<Line>::iterator it = m_document.begin(); it != m_document.end(); ++it)
        {
            if (it->m_key == cfgName)
            {
                it->m_value = value;
                m_needToSave = true;  // tag the need of saving
                return;
            }
        }

        // if not found, then add a new record
        Line line;
        line.m_key = cfgName;
        line.m_equalSign = true;
        line.m_value = value;
        m_document.push_back(line);
        m_needToSave = true;   // tag the need of saving
    }

    void writeBool(const char * cfgName, bool value)
    {
        writeString(cfgName, (value ? "true" : "false"));
    }

    void writeNum(const char * cfgName, long value)
    {
        char buf[30];
        sprintf(buf,"%ld",value);
        writeString(cfgName, buf);
    }

    void writeDecimal(const char * cfgName, double value)
    {
        char buf[30];
        sprintf(buf,"%lf",value);
        writeString(cfgName, buf);
    }

    void writeList(const char *cfgName, std::vector<std::string> value, char splitChar = ',')
    {
        std::string s;
        if (value.size() > 0)
            s = value[0];
        for (size_t i = 1; i < value.size(); ++i)
            s += std::string()+ splitChar + value[i];
        writeString(cfgName, s.c_str());
    }
};

}


#endif /* head file guarder */

