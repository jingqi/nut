/**
 * @file -
 * @author jingqi
 * @date 2010-8-17
 */

#ifndef ___HEADFILE___38990555_F0A8_4BA9_BE50_9A477A2A3F47_
#define ___HEADFILE___38990555_F0A8_4BA9_BE50_9A477A2A3F47_

#include <nut/platform/platform.hpp>

#include <string>
#include <vector>
#ifdef NUT_PLATFORM_OS_WINDOWS
#   include <memory>
#else
#   include <tr1/memory>
#endif

#include "loglevel.hpp"
#include "logrecord.hpp"

namespace nut
{

class LogFilter
{
public :
    virtual ~LogFilter() {}

    virtual bool isLogable (const std::string &loggerpath, const LogRecord &log) const = 0;
};

class DefaultLogFilter : public LogFilter
{
    bool m_allowMask[COUNT_OF_LOG_LEVEL];
    std::vector<std::string> m_excepts;

public :
    DefaultLogFilter (LogLevel minLevel, const std::vector<std::string> &excepts = std::vector<std::string>())
        : m_excepts(excepts)
    {
        int i = 0;
        for (; i < minLevel; ++i)
            m_allowMask[i] = false;
        for (; i < COUNT_OF_LOG_LEVEL; ++i)
            m_allowMask[i] = true;
    }

    DefaultLogFilter(bool allowDebug = true, bool allowInfo = true, bool allowWarn = true,
        bool allowError = true, bool allowFatal = true, const std::vector<std::string> &excepts = std::vector<std::string>())
        : m_excepts(excepts)
    {
        m_allowMask[LL_DEBUG] = allowDebug;
        m_allowMask[LL_INFO] = allowInfo;
        m_allowMask[LL_WARN] = allowWarn;
        m_allowMask[LL_ERROR] = allowError;
        m_allowMask[LL_FATAL] = allowFatal;
    }

    virtual bool isLogable(const std::string &loggerpath, const LogRecord &log) const
    {
        if (!m_allowMask[log.getLevel()])
            return false;
        for (std::vector<std::string>::const_iterator it = m_excepts.begin(), ite = m_excepts.end();
            it != ite; ++it)
        {
            if (it->length() <= loggerpath.length() &&
                *it == loggerpath.substr(0,it->length()))
                return false;
        }
        return true;
    }

    void addExceptPath(const std::string &path)
    {
        m_excepts.push_back(path);
    }
};

inline std::tr1::shared_ptr<LogFilter> createLogFilter(const std::string &arg)
{
    bool allows[5] = {true,true,true,true,true};
    for (size_t i = 0; i < arg.length() && i < 5; ++i)
        if (arg[i] == '0')
            allows[i] = false;
    std::vector<std::string> excepts;
    if (arg.length() > 6 && arg[5] == '|')
    {
        std::string::size_type begin = 6, end = arg.find_first_of(':',6);
        while (end != std::string::npos)
        {
            if (begin != end)
                excepts.push_back(arg.substr(begin, end - begin));
            begin = end + 1;
            end = arg.find_first_of(':',begin);
        }
        if (begin != arg.length())
            excepts.push_back(arg.substr(begin));
    }

    return std::tr1::shared_ptr<LogFilter>(new DefaultLogFilter(allows[0],allows[1],allows[2],allows[3],allows[4],excepts));
}

}


#endif // head file guarder

