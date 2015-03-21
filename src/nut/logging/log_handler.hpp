/**
 * @file -
 * @author jingqi
 * @date 2010-8-17
 * @last-edit 2015-01-06 22:23:30 jingqi
 */

#ifndef ___HEADFILE___4CDF318F_AF06_4CEF_BAC8_DE26853A73AB_
#define ___HEADFILE___4CDF318F_AF06_4CEF_BAC8_DE26853A73AB_


#include <iostream>
#include <fstream>
#include <vector>

#include <nut/platform/platform.hpp>

#if defined(NUT_PLATFORM_OS_LINUX)
#   include <unistd.h> // for getpid()
#   include <syslog.h>
#endif

#include <nut/util/console_util.hpp>
#include <nut/util/time.hpp>
#include <nut/threading/sync/mutex.hpp>
#include <nut/threading/sync/guard.hpp>
#include <nut/rc/rc_new.hpp>

#include "log_record.hpp"
#include "log_filter.hpp"


#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

class LogHandler
{
    NUT_REF_COUNTABLE

    Mutex m_mutex;
    std::vector<rc_ptr<LogFilter> > m_filters;

public:
    virtual ~LogHandler() {}

    virtual void handle_log(const std::string &log_path, const LogRecord &rec) = 0;

    void add_filter(rc_ptr<LogFilter> filter)
    {
        Guard<Mutex> g(&m_mutex);
        m_filters.push_back(filter);
    }

    /**
     * @return
     *      true, 通过筛选
     *      false, 在筛选过程中被剔除
     */
    void handle_log(const std::string log_path, const LogRecord& rec, bool apply_filter)
    {
        Guard<Mutex> g(&m_mutex);
        if (apply_filter && !LogFilter::is_logable(log_path, rec, m_filters))
            return;
        handle_log(log_path, rec);
    }
};

class StreamLogHandler : public LogHandler
{
    std::ostream &m_os;

public:
    StreamLogHandler (std::ostream &os)
        : m_os(os)
    {}

    virtual void handle_log(const std::string &log_path, const LogRecord &rec)
    {
        (void) log_path; // unused

        m_os << rec.to_string() << std::endl;
        m_os.flush();
    }
};

class ConsoleLogHandler : public LogHandler
{
    bool m_colored;

public :
    ConsoleLogHandler(bool colored = true)
        : m_colored(colored)
    {}

    virtual void handle_log(const std::string &logger_path, const LogRecord &l)
    {
        (void) logger_path; // unused

        std::cout << "[" << l.get_time().to_string() << "] ";
        if (m_colored)
        {
            switch(l.get_level())
            {
            case LL_DEBUG:
                ConsoleUtil::set_text_color(ConsoleUtil::WHITE, ConsoleUtil::BLUE);
                break;

            case LL_INFO:
                ConsoleUtil::set_text_color(ConsoleUtil::WHITE, ConsoleUtil::GREEN);
                break;

            case LL_WARN:
                ConsoleUtil::set_text_color(ConsoleUtil::WHITE, ConsoleUtil::YELLOW);
                break;

            case LL_ERROR:
                ConsoleUtil::set_text_color(ConsoleUtil::WHITE, ConsoleUtil::PINK);
                break;

            case LL_FATAL:
                ConsoleUtil::set_text_color(ConsoleUtil::WHITE, ConsoleUtil::RED);
                break;

            default:
                break;
            }
        }

        std::cout << log_level_to_str(l.get_level());

        if (m_colored)
            ConsoleUtil::set_text_color();

        std::cout << " " << l.get_source_location().to_string() << "  " << l.get_message() << "\n";
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

    virtual void handle_log(const std::string &log_path, const LogRecord & rec)
    {
        (void) log_path; // unused

        m_ofs << rec.to_string() << std::endl;
        m_ofs.flush();
    }
};

#if defined(NUT_PLATFORM_OS_LINUX)
class SyslogLogHandler : public LogHandler
{
    bool m_close_syslog_on_exit;

public :
    SyslogLogHandler(bool close_syslog_on_exit = false)
        : m_close_syslog_on_exit(close_syslog_on_exit)
    {}

    ~SyslogLogHandler()
    {
        if (m_close_syslog_on_exit)
            closelog();  // the oposite way is openlog()
    }

    virtual void handle_log(const std::string &log_path, const LogRecord &rec)
    {
        (void) log_path; // unused

        int level = 0;
        switch (rec.get_level())
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
        std::string msg = rec.get_source_location().to_string() + "  " + rec.get_message();
        syslog(level, msg.c_str());
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
    static rc_ptr<LogHandler> create_log_handler(const std::string &type)
    {
        if (type == "stdout")
        {
            return RC_NEW(NULL, StreamLogHandler, ref_arg<std::ostream>(std::cout));
        }
        else if (type == "stderr")
        {
            return RC_NEW(NULL, StreamLogHandler, ref_arg<std::ostream>(std::cerr));
        }
        else if (type.size() >= 7 && type.substr(0,7) == "console")
        {
            if (type.size() == 15 && type.substr(7,8) == "|nocolor")
                return RC_NEW(NULL, ConsoleLogHandler, false);
            else
                return RC_NEW(NULL, ConsoleLogHandler, true);
        }
        else if (type.size() > 5 && type.substr(0,5) == "file|")
        {
            std::string::size_type pos = type.find_last_of('|');
            std::string last = type.substr(pos + 1);
            if (type.size() > 12 && type.substr(5,7) == "append|")
            {
                return RC_NEW(NULL, FileLogHandler, last.c_str(), true);
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
                last += Time().format_time("%Y-%m-%d %H-%M-%S ");
                last += buf;
                last += ".log";
                return RC_NEW(NULL, FileLogHandler, last.c_str(), false);
            }
            else // trunc
            {
                return RC_NEW(NULL, FileLogHandler, last.c_str(), false);
            }
        }
#if defined(NUT_PLATFORM_OS_LINUX)
        else if (type == "syslog")
        {
            return RC_NEW<SyslogLogHandler>(false);
        }
#endif

        /* default */
        return RC_NEW(NULL, StreamLogHandler, ref_arg<std::ostream>(std::cout));
    }
};

}


#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif

#endif  // head file guarder
