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

#include "loghandler.hpp"

namespace nut
{

class Logger
{
    DECLARE_GC_ENABLE

    std::vector<ref<LogHandler> > m_handlers;
    std::vector<ref<Logger> > m_subloggers;
    weak_ref<Logger> m_parent;
    std::string m_loggerPath;
    int m_destructTag;
    Mutex m_mutex;

#ifndef NDEBUG
    DestroyChecker m_cheker;
#endif

private :
    Logger(const Logger&);
    Logger& operator=(const Logger&);

private :
    friend class LogManager;

    Logger(weak_ref<Logger> parent, const std::string &path)
        : m_parent(parent), m_loggerPath(path)
    {}

    static std::string getFirstParent(const std::string &loggerpath)
    {
        std::string::size_type i = loggerpath.find_first_of('.');
        return loggerpath.substr(0, i);
    }

    static std::string getLoggerName(const std::string &loggerpath)
    {
        std::string::size_type i = loggerpath.find_last_of('.');
        if (i == std::string::npos)
            i = 0;
        else
            ++i;
        return loggerpath.substr(i);
    }

    static std::string subLoggerPath(const std::string &loggerpath)
    {
        std::string::size_type i = loggerpath.find_first_of('.');
        if (i == std::string::npos)
            i = loggerpath.length();
        else
            ++i;
        return loggerpath.substr(i);
    }

    void log(const std::string &loggerpath, const LogRecord &record) const
    {
#ifndef NDEBUG
        m_cheker.checkDestroy();
#endif

        for (std::vector<std::tr1::shared_ptr<LogHandler> >::const_iterator it = m_handlers.begin(),
            ite = m_handlers.end(); it != ite; ++it)
        {
            LogFilter* filter = (*it)->getFilter();
            if (filter == NULL  || filter->isLogable(loggerpath,record))
                (*it)->handleLog(loggerpath, record);
        }
        if (m_parent != NULL)
            m_parent->log(loggerpath, record);
    }

public :
    void addHandler(ref<LogHandler> handler)
    {
#ifndef NDEBUG
        m_cheker.checkDestroy();
#endif

        m_handlers.push_back(handler);
    }

    void log(const LogRecord &record) const
    {
#ifndef NDEBUG
        m_cheker.checkDestroy();
#endif

        log(m_loggerPath, record);
    }

    void log(LogLevel level, const SourceLocation &sl, const std::string &msg) const
    {
#ifndef NDEBUG
        m_cheker.checkDestroy();
#endif

        log(m_loggerPath, LogRecord(level,sl,msg));
    }

    void log(LogLevel level, const SourceLocation &sl, const char *format, ...) const
    {
#ifndef NDEBUG
        m_cheker.checkDestroy();
#endif

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
            if (n > -1 && n < size)
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

        log(m_loggerPath, LogRecord(level,sl,msg));
    }

    Logger& getLogger(const std::string &relativepath)
    {
#ifndef NDEBUG
        m_cheker.checkDestroy();
#endif

        Guard<Mutex> g(&m_mutex);

        if (relativepath.length() == 0)
            return *this;

        std::vector<ref<Logger> >::const_iterator it = m_subloggers.begin(),
            ite = m_subloggers.end();
        std::string current = getFirstParent(relativepath);
        for (; it != ite && current != (*it)->getLoggerName(); ++it) {}
        if (it == ite)
        {
            m_subloggers.push_back(std::tr1::shared_ptr<Logger>(new Logger(this,
                (m_loggerPath.length() == 0 ? current : m_loggerPath + "." + current))));
            it = m_subloggers.end() - 1;
        }
        return (*it)->getLogger(subLoggerPath(relativepath));
    }

    std::string getLoggerPath()
    {
#ifndef NDEBUG
        m_cheker.checkDestroy();
#endif

        return m_loggerPath;
    }

    std::string getLoggerName()
    {
#ifndef NDEBUG
        m_cheker.checkDestroy();
#endif

        return getLoggerName(m_loggerPath);
    }

};

}

#endif // head file guarder

