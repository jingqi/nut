/**
 * @file -
 * @author jingqi
 * @date 2010-8-17
 */

#ifndef ___HEADFILE___4CDF318F_AF06_4CEF_BAC8_DE26853A73AB_
#define ___HEADFILE___4CDF318F_AF06_4CEF_BAC8_DE26853A73AB_


#include <iostream>
#include <fstream>
#include <vector>

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_LINUX)
#   include <syslog.h>
#endif

#include <nut/util/consoleutil.hpp>
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
    NUT_GC_REFERABLE

    Mutex m_mutex;
    std::vector<ref<LogFilter> > m_filters;

public:
    virtual ~LogHandler() {}

    virtual void handleLog(const std::string &logPath, const LogRecord &rec) = 0;

    void addFilter(ref<LogFilter> filter)
    {
        Guard<Mutex> g(&m_mutex);
        m_filters.push_back(filter);
    }
    
    /**
     * @return
     *      true, 通过筛选
     *      false, 在筛选过程中被剔除
     */
    void handleLog(const std::string logPath, const LogRecord& rec, bool applyFilter)
    {
        Guard<Mutex> g(&m_mutex);
        if (applyFilter && !LogFilter::isLogable(logPath, rec, m_filters))
            return;
        handleLog(logPath, rec);
    }
};

class StreamLogHandler : public LogHandler
{
    std::ostream &m_os;

public:
    StreamLogHandler (std::ostream &os) : m_os(os) {}

    virtual void handleLog(const std::string &logPath, const LogRecord &rec)
    {
        (void) logPath; // unused

        m_os << rec.toString() << std::endl;
        m_os.flush();
    }
};

class ConsoleLogHandler : public LogHandler
{
    bool m_colored;
    
public :
    ConsoleLogHandler(bool colored = true) : m_colored(colored) {}

    virtual void handleLog(const std::string &loggerpath, const LogRecord &l)
    {
        (void) loggerpath; // unused

        std::cout << "[" << l.getTime().toString() << "] ";
        if (m_colored)
        {
            switch(l.getLevel())
            {
            case LL_DEBUG:
                ConsoleUtil::setTextColor(ConsoleUtil::WHITE, ConsoleUtil::BLUE);
                break;
            case LL_INFO:
                ConsoleUtil::setTextColor(ConsoleUtil::WHITE, ConsoleUtil::GREEN);
                break;
            case LL_WARN:
                ConsoleUtil::setTextColor(ConsoleUtil::WHITE, ConsoleUtil::YELLOW);
                break;
            case LL_ERROR:
                ConsoleUtil::setTextColor(ConsoleUtil::WHITE, ConsoleUtil::PINK);
                break;
            case LL_FATAL:
                ConsoleUtil::setTextColor(ConsoleUtil::WHITE, ConsoleUtil::RED);
                break;
            default:
                break;
            }
        }

        std::cout << logLevelToStr(l.getLevel());

        if (m_colored)
            ConsoleUtil::setTextColor();

        std::cout << " " << l.getSourceLocation().toString() << "  " << l.getMessage() << "\n";
        std::cout.flush();
    }
};

class FileLogHandler : public LogHandler
{
    std::ofstream m_ofs;

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

    virtual void handleLog(const std::string &logPath, const LogRecord & rec)
    {
        (void) logPath; // unused

        m_ofs << rec.toString() << std::endl;
        m_ofs.flush();
    }
};

#if defined(NUT_PLATFORM_OS_LINUX)
class SyslogLogHandler : public LogHandler
{
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

    virtual void handleLog(const std::string &logPath, const LogRecord &rec)
    {
        (void) logPath; // unused

        int level = 0;
        switch (rec.getLevel())
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
        syslog(level, (rec.getSourceLocation().toString() + "  " + rec.getMessage()).c_str());
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

