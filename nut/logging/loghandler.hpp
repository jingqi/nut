/**
 * @file -
 * @author jingqi
 * @date 2010-8-17
 */

#ifndef ___HEADFILE___4CDF318F_AF06_4CEF_BAC8_DE26853A73AB_
#define ___HEADFILE___4CDF318F_AF06_4CEF_BAC8_DE26853A73AB_


#include <iostream>
#include <fstream>

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_LINUX)
#   include <syslog.h>
#endif

#include <nut/util/consolehelper.hpp>
#include <nut/util/time.hpp>
#include <nut/threading/mutex.hpp>
#include <nut/threading/guard.hpp>
#include <nut/gc/gc.hpp>

#include "logrecord.hpp"
#include "logfilter.hpp"

namespace nut
{

class LogHandler
{
    DECLARE_GC_ENABLE

    ref<LogFilter> m_filter;

protected :
    Mutex m_mutex;

public :
    virtual ~LogHandler() {}

    virtual void handleLog(const std::string &loggerpath, const LogRecord &l) = 0;

    void setFilter(ref<LogFilter> filter)
    {
        Guard<Mutex> g(&m_mutex);
        m_filter = filter;
    }

    LogFilter* getFilter()
    {
        Guard<Mutex> g(&m_mutex);
        return m_filter.pointer();
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
        Guard<Mutex> g(&m_mutex);
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
        Guard<Mutex> g(&m_mutex);
        std::cout << "[" << l.getTime().toString() << "] ";
        if (mColored)
        {
            switch(l.getLevel())
            {
            case LL_DEBUG:
                ConsoleHelper::setTextColor(ConsoleHelper::WHITE, ConsoleHelper::BLUE);
                break;
            case LL_INFO:
                ConsoleHelper::setTextColor(ConsoleHelper::WHITE, ConsoleHelper::GREEN);
                break;
            case LL_WARN:
                ConsoleHelper::setTextColor(ConsoleHelper::WHITE, ConsoleHelper::YELLOW);
                break;
            case LL_ERROR:
                ConsoleHelper::setTextColor(ConsoleHelper::WHITE, ConsoleHelper::PINK);
                break;
            case LL_FATAL:
                ConsoleHelper::setTextColor(ConsoleHelper::WHITE, ConsoleHelper::RED);
                break;
            default:
                break;
            }
        }

        std::cout << logLevelToStr(l.getLevel());

        if (mColored)
            ConsoleHelper::setTextColor();

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
        Guard<Mutex> g(&m_mutex);
        m_ofs << l.toString() << std::endl;
        m_ofs.flush();
    }
};

#if defined(NUT_PLATFORM_OS_LINUX)
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
        Guard<threading::Mutex> g(&m_mutex);
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


class LogHandlerFactory
{
    LogHandlerFactory();

public:
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
    static ref<LogHandler> createLogHandler(const std::string &type)
    {
        if (type == "stdout")
        {
            return gc_new<StreamLogHandler>(ref_arg<std::ostream>(std::cout));
        }
        else if (type == "stderr")
        {
            return gc_new<StreamLogHandler>(ref_arg<std::ostream>(std::cerr));
        }
        else if (type.size() >= 7 && type.substr(0,7) == "console")
        {
            if (type.size() == 15 && type.substr(7,8) == "|nocolor")
                return gc_new<ConsoleLogHandler>(false);
            else
                return gc_new<ConsoleLogHandler>(true);
        }
        else if (type.size() > 5 && type.substr(0,5) == "file|")
        {
            std::string::size_type pos = type.find_last_of('|');
            std::string last = type.substr(pos + 1);
            if (type.size() > 12 && type.substr(5,7) == "append|")
            {
                return gc_new<FileLogHandler>(last.c_str(), true);
            }
            else if (type.size() > 12 && type.substr(5,7) == "circle|")
            {
                char buf[30] = {0};
#if defined(NUT_PLATFORM_OS_WINDOWS)
                long pid = ::GetCurrentProcessId();
#else
                pid_t pid = getpid();
#endif
                sprintf(buf, "%ld", (long)pid);
                last += Time().formatTime("%Y-%m-%d %H-%M-%S ");
                last += buf;
                last += ".log";
                return gc_new<FileLogHandler>(last.c_str(), false);
            }
            else // trunc
            {
                return gc_new<FileLogHandler>(last.c_str(), false);
            }
        }
#if defined(NUT_PLATFORM_OS_LINUX)
        else if (type == "syslog")
        {
            return gc_new<SyslogLogHandler>(false);
        }
#endif

        /* default */
        return gc_new<StreamLogHandler>(ref_arg<std::ostream>(std::cout));
    }
};

}


#endif  // head file guarder

