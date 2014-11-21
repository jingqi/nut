/**
 * @file -
 * @author jingqi
 * @date 2010-8-17
 */

#ifndef ___HEADFILE___38990555_F0A8_4BA9_BE50_9A477A2A3F47_
#define ___HEADFILE___38990555_F0A8_4BA9_BE50_9A477A2A3F47_

#include <string>
#include <vector>

#include <nut/gc/gc.hpp>

#include "loglevel.hpp"
#include "logrecord.hpp"

namespace nut
{

class LogFilter
{
    NUT_GC_REFERABLE

public:
    virtual bool isLogable(const std::string &loggerpath, const LogRecord &log) const = 0;

public:
    static bool isLogable(const std::string& logPath, const LogRecord& rec, const std::vector<ref<LogFilter> >& filters)
    {
        for (std::vector<ref<LogFilter> >::const_iterator iter = filters.begin(), end = filters.end();
            iter != end; ++iter)
        {
            if (!(*iter)->isLogable(logPath, rec))
                return false;
        }
        return true;
    }
};

class DefaultLogFilter : public LogFilter
{
    bool m_levelMask[COUNT_OF_LOG_LEVEL];
    std::vector<std::string> m_denyPaths;

public :
    DefaultLogFilter (LogLevel minLevel, const std::vector<std::string> &denyPaths = std::vector<std::string>())
        : m_denyPaths(denyPaths)
    {
        for (int i = 0; i < COUNT_OF_LOG_LEVEL; ++i)
            m_levelMask[i] = (i < minLevel ? true : false);
    }

    DefaultLogFilter(bool allowDebug = true, bool allowInfo = true, bool allowWarn = true,
        bool allowError = true, bool allowFatal = true, const std::vector<std::string> &excepts = std::vector<std::string>())
        : m_denyPaths(excepts)
    {
        m_levelMask[LL_DEBUG] = allowDebug;
        m_levelMask[LL_INFO] = allowInfo;
        m_levelMask[LL_WARN] = allowWarn;
        m_levelMask[LL_ERROR] = allowError;
        m_levelMask[LL_FATAL] = allowFatal;
    }

    virtual bool isLogable(const std::string &logPath, const LogRecord &rec) const
    {
        if (!m_levelMask[rec.getLevel()])
            return false;

        for (std::vector<std::string>::const_iterator iter = m_denyPaths.begin(), end = m_denyPaths.end();
            iter != end; ++iter)
        {
            if (iter->length() <= logPath.length() &&
                *iter == logPath.substr(0, iter->length()))
                return false;
        }
        return true;
    }

    void addDenyPath(const std::string &path)
    {
        m_denyPaths.push_back(path);
    }
};

class LogFilterFactory
{
    LogFilterFactory();

public:
    static ref<LogFilter> createLogFilter(const std::string &arg)
    {
        bool mask[5] = {true, true, true, true, true};
        for (size_t i = 0; i < arg.length() && i < 5; ++i)
            if (arg[i] == '0')
                mask[i] = false;
        std::vector<std::string> denyPaths;
        if (arg.length() > 6 && arg[5] == '|')
        {
            std::string::size_type begin = 6, end = arg.find_first_of(':',6);
            while (end != std::string::npos)
            {
                if (begin != end)
                    denyPaths.push_back(arg.substr(begin, end - begin));
                begin = end + 1;
                end = arg.find_first_of(':',begin);
            }
            if (begin != arg.length())
                denyPaths.push_back(arg.substr(begin));
        }

        return gc_new<DefaultLogFilter>(mask[0], mask[1], mask[2], mask[3], mask[4], denyPaths);
    }
};

}


#endif // head file guarder
