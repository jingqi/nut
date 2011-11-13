/**
 * @file -
 * @author jingqi
 * @date 2010-8-17
 */

#ifndef ___HEADFILE___4CDF318F_AF06_4CEF_BAC8_DE26853A73AB_
#define ___HEADFILE___4CDF318F_AF06_4CEF_BAC8_DE26853A73AB_


#include <iostream>
#include <fstream>
#if defined(WIN32)
#   include <memory>  // for std::tr1::shared_ptr
#else
#   include <tr1/memory>  // for std::tr1::shared_ptr
#   include <syslog.h>
#endif
#include <util/ConsoleHelper.h>
#include <util/Time.h>
#include <threading/Mutex.h>
#include <threading/Guard.h>

#include "LogRecord.h"
#include "LogFilter.h"

namespace nut
{

class LogHandler
{
    std::tr1::shared_ptr<LogFilter> m_filter;

protected :
    threading::Mutex m_mutex;

public :
    virtual ~LogHandler() {}

    virtual void handleLog(const std::string &loggerpath, const LogRecord &l) = 0;

    void setFilter(std::tr1::shared_ptr<LogFilter> filter)
    {
        threading::Guard<threading::Mutex> g(&m_mutex);
        m_filter = filter;
    }

    LogFilter* getFilter()
    {
        threading::Guard<threading::Mutex> g(&m_mutex);
        return m_filter.get();
    }
};

class StreamLogHandler : public LogHandler
{
    std::ostream &m_os;
    using LogHandler::m_mutex;

public :
    StreamLogHandler (std::ostream &os) : m_os(os) {}

    virtual void handleLog(const std::string &loggerpath, const LogRecord &l)
    {
        threading::Guard<threading::Mutex> g(&m_mutex);
        m_os << l.toString() << std::endl;
        m_os.flush();
    }
};

class ConsoleLogHandler : public LogHandler
{
    bool mColored;
    using LogHandler::m_mutex;

public :
    ConsoleLogHandler(bool colored = true)
        : mColored(colored)
    {}

    virtual void handleLog(const std::string &loggerpath, const LogRecord &l)
    {
        threading::Guard<threading::Mutex> g(&m_mutex);
        std::cout << "[" << l.getTime().toString() << "] ";
        if (mColored)
        {
            switch(l.getLevel())
            {
            case LL_DEBUG:
                util::ConsoleHelper::setTextColor(util::ConsoleHelper::WHITE, util::ConsoleHelper::BLUE);
                break;
            case LL_INFO:
                util::ConsoleHelper::setTextColor(util::ConsoleHelper::WHITE, util::ConsoleHelper::GREEN);
                break;
            case LL_WARN:
                util::ConsoleHelper::setTextColor(util::ConsoleHelper::WHITE, util::ConsoleHelper::YELLOW);
                break;
            case LL_ERROR:
                util::ConsoleHelper::setTextColor(util::ConsoleHelper::WHITE, util::ConsoleHelper::PINK);
                break;
            case LL_FATAL:
                util::ConsoleHelper::setTextColor(util::ConsoleHelper::WHITE, util::ConsoleHelper::RED);
                break;
            default:
                break;
            }
        }

        std::cout << logLevelToStr(l.getLevel());

        if (mColored)
            util::ConsoleHelper::setTextColor();

        std::cout << " " << l.getSourceLocation().toString() << "  " << l.getMessage() << "\n";
        std::cout.flush();
    }
};

class FileLogHandler : public LogHandler
{
    std::ofstream m_ofs;
    using LogHandler::m_mutex;

public :
    FileLogHandler (const char *file, bool append = false)
        : m_ofs(file, (append ? std::ios::app : std::ios::trunc))
    {
        if (append)
        {
            m_ofs << "\n\n\n\n\n\n------------- ---------------- ---------------\n";
            m_ofs.flush();
        }
    }

    virtual void handleLog(const std::string &loggerpath, const LogRecord & l)
    {
        threading::Guard<threading::Mutex> g(&m_mutex);
        m_ofs << l.toString() << std::endl;
        m_ofs.flush();
    }
};

#if !defined(WIN32)
class SyslogLogHandler : public LogHandler
{
    using LogHandler::m_mutex;
    bool m_closesyslog;

public :
    SyslogLogHandler(bool closesyslog = false)
        : m_closesyslog(closesyslog)
    {}

    ~SyslogLogHandler()
    {
        if (m_closesyslog)
            closelog();  // the oposite way is openlog()
    }

    virtual void handleLog(const std::string &loggerpath, const LogRecord &l)
    {
        threading::Guard<threading::Mutex> g(&m_mutex);
        int level = 0;
        switch (l.getLevel())
        {
        case LL_DEBUG:
            level = LOG_DEBUG;
            break;
        case LL_INFO:
            level = LOG_INFO;
            break;
        case LL_WARN:
            level = LOG_WARNING;
            break;
        case LL_ERROR:
            level = LOG_ERR;
            break;
        case LL_FATAL:
            level = LOG_CRIT;
            break;
        default:
            level = LOG_ERR;
        }
        syslog(level,(l.getSourceLocation().toString() + "  " + l.getMessage()).c_str());
    }
};
#endif


/**
 * the following is allowed:
 * stdout
 * stderr
 * console
 * console|nocolor
 * file|append|./logfile.log
 * file|circle|./dir/prefix
 * file|./dir/logfile.log
 * syslog
 */
std::tr1::shared_ptr<LogHandler> createLogHandler(const std::string &type)
{
    if (type == "stdout")
    {
        return std::tr1::shared_ptr<LogHandler>(new StreamLogHandler(std::cout));
    }
    else if (type == "stderr")
    {
        return std::tr1::shared_ptr<LogHandler>(new StreamLogHandler(std::cerr));
    }
    else if (type.size() >= 7 && type.substr(0,7) == "console")
    {
        if (type.size() == 15 && type.substr(7,8) == "|nocolor")
            return std::tr1::shared_ptr<LogHandler>(new ConsoleLogHandler(false));
        else
            return std::tr1::shared_ptr<LogHandler>(new ConsoleLogHandler(true));
    }
    else if (type.size() > 5 && type.substr(0,5) == "file|")
    {
        std::string::size_type pos = type.find_last_of('|');
        std::string last = type.substr(pos + 1);
        if (type.size() > 12 && type.substr(5,7) == "append|")
        {
            return std::tr1::shared_ptr<LogHandler>(new FileLogHandler(last.c_str(), true));
        }
        else if (type.size() > 12 && type.substr(5,7) == "circle|")
        {
            char buf[30] = {0};
#if defined(WIN32)
            long pid = ::GetCurrentProcessId();
#else
            pid_t pid = getpid();
#endif
            sprintf(buf, "%ld", (long)pid);
            last += util::Time().formatTime("%Y-%m-%d %H-%M-%S ");
            last += buf;
            last += ".log";
            return std::tr1::shared_ptr<LogHandler>(new FileLogHandler(last.c_str(), false));
        }
        else // trunc
        {
            return std::tr1::shared_ptr<LogHandler>(new FileLogHandler(last.c_str(), false));
        }
    }
#if !defined(WIN32)
    else if (type == "syslog")
    {
        return std::tr1::shared_ptr<LogHandler>(new SyslogLogHandler(false));
    }
#endif

    /* default */
    return std::tr1::shared_ptr<LogHandler>(new StreamLogHandler(std::cout));
}

}


#endif  // head file guarder

