/**
 * @file -
 * @author jingqi
 * @date 2010-8-18
 */

#ifndef ___HEADFILE___C2472118_B9F0_49CE_BE12_59F3AE4991CA_
#define ___HEADFILE___C2472118_B9F0_49CE_BE12_59F3AE4991CA_

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <vector>

#include <nut/gc/gc.hpp>
#include <nut/threading/mutex.hpp>
#include <nut/threading/guard.hpp>
#include <nut/debugging/destroychecker.hpp>

#include "logpath.hpp"
#include "loghandler.hpp"

namespace nut
{

class Logger
{
    NUT_GC_REFERABLE
    NUT_GC_PRIVATE_GCNEW

    std::vector<ref<LogHandler> > m_handlers;
    std::vector<ref<Logger> > m_subloggers;
    std::vector<ref<LogFilter> > m_filters;
    weak_ref<Logger> m_parent;
    std::string m_loggerPath;
    Mutex m_mutex;

    /** 析构检查器 */
    NUT_DEBUGGING_DESTROY_CHECKER

private:
    Logger(const Logger&);
    Logger& operator=(const Logger&);

private:
    Logger(weak_ref<Logger> parent, const std::string &path)
        : m_parent(parent), m_loggerPath(path)
    {}

    void log(const std::string &logPath, const LogRecord &rec) const
    {
        NUT_DEBUGGING_ASSERT_ALIVE;

        if (!LogFilter::isLogable(logPath, rec, m_filters))
            return;

        for (std::vector<ref<LogHandler> >::const_iterator iter = m_handlers.begin(),
            end = m_handlers.end(); iter != end; ++iter)
                (*iter)->handleLog(logPath, rec, true);

        if (!m_parent.isNull())
            m_parent->log(logPath, rec);
    }

public :
    void addHandler(ref<LogHandler> handler)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;

        m_handlers.push_back(handler);
    }

    void addFilter(ref<LogFilter> filter)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;

        m_filters.push_back(filter);
    }

    void log(const LogRecord &record) const
    {
        NUT_DEBUGGING_ASSERT_ALIVE;

        log(m_loggerPath, record);
    }

    void log(LogLevel level, const SourceLocation &sl, const std::string &msg) const
    {
        NUT_DEBUGGING_ASSERT_ALIVE;

        log(m_loggerPath, LogRecord(level, sl, msg));
    }

    void log(LogLevel level, const SourceLocation &sl, const char *format, ...) const
    {
        NUT_DEBUGGING_ASSERT_ALIVE;

        assert(NULL != format);
        size_t size = 100;
        char *buf = (char*)malloc(size);
        assert(NULL != buf);

        va_list ap;
        while (NULL != buf)
        {
            va_start(ap, format);
            int n = vsnprintf(buf, size, format, ap);
            va_end(ap);
            if (n > -1 && n < (int)size)
                break;

            if (n > -1)
                size = n + 1; /* glibc 2.1 */
            else
                size *= 2; /* glibc 2.0 */

            if (NULL != buf)
                free(buf);
            buf = (char*)malloc(size);
            assert(NULL != buf);
        }
        std::string msg = (NULL == buf ? "" : buf);
        if (NULL != buf)
            free(buf);

        log(m_loggerPath, LogRecord(level, sl, msg));
    }

    weak_ref<Logger> getLogger(const std::string &relativepath)
    {
        NUT_DEBUGGING_ASSERT_ALIVE;

        Guard<Mutex> g(&m_mutex);

        if (relativepath.length() == 0)
            return this;

        std::vector<ref<Logger> >::const_iterator iter = m_subloggers.begin(),
            end = m_subloggers.end();
        const std::string current = LogPath::getFirstParent(relativepath);
        while (iter != end && current != (*iter)->getLoggerName()) ++iter;
        if (iter == end)
        {
            m_subloggers.push_back(ref<Logger>(gc_new<Logger>(this,
                (m_loggerPath.length() == 0 ? current : m_loggerPath + "." + current))));
            iter = m_subloggers.end() - 1;
        }
        return (*iter)->getLogger(LogPath::subLogPath(relativepath));
    }

    std::string getLoggerPath()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;

        return m_loggerPath;
    }

    std::string getLoggerName()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;

        return LogPath::getName(m_loggerPath);
    }

};

}

#endif // head file guarder

