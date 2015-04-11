
#include <nut/platform/platform.h>

#if defined(NUT_PLATFORM_OS_MAC)
#   include <unistd.h> // for getpid()
#elif defined(NUT_PLATFORM_OS_LINUX)
#   include <unistd.h> // for getpid()
#   include <syslog.h>
#endif

#include <nut/util/console_util.h>
#include <nut/util/time/date_time.h>
#include <nut/threading/sync/mutex.h>
#include <nut/threading/sync/guard.h>
#include <nut/rc/rc_new.h>

#include "log_handler.h"

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

void LogHandler::add_filter(rc_ptr<LogFilter> filter)
{
    Guard<Mutex> g(&m_mutex);
    m_filters.push_back(filter);
}

/**
 * @return
 *      true, 通过筛选
 *      false, 在筛选过程中被剔除
 */
void LogHandler::handle_log(const std::string log_path, const LogRecord& rec, bool apply_filter)
{
    Guard<Mutex> g(&m_mutex);
    if (apply_filter && !LogFilter::is_logable(log_path, rec, m_filters))
        return;
    handle_log(log_path, rec);
}

void StreamLogHandler::handle_log(const std::string &log_path, const LogRecord &rec)
{
    (void) log_path; // unused

    m_os << rec.to_string() << std::endl;
    m_os.flush();
}

void ConsoleLogHandler::handle_log(const std::string &logger_path, const LogRecord &l)
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

FileLogHandler::FileLogHandler(const char *file, bool append)
    : m_ofs(file, (append ? std::ios::app : std::ios::trunc))
{
    if (append)
    {
        m_ofs << "\n\n\n\n\n\n------------- ---------------- ---------------\n";
        m_ofs.flush();
    }
}

void FileLogHandler::handle_log(const std::string &log_path, const LogRecord & rec)
{
    (void) log_path; // unused

    m_ofs << rec.to_string() << std::endl;
    m_ofs.flush();
}

#if defined(NUT_PLATFORM_OS_LINUX)

SyslogLogHandler::~SyslogLogHandler()
{
    if (m_close_syslog_on_exit)
        closelog();  // the oposite way is openlog()
}

void SyslogLogHandler::handle_log(const std::string &log_path, const LogRecord &rec)
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

#endif


rc_ptr<LogHandler> LogHandlerFactory::create_log_handler(const std::string &type)
{
    if (type == "stdout")
    {
        return rc_new<StreamLogHandler>(std::cout);
    }
    else if (type == "stderr")
    {
        return rc_new<StreamLogHandler>(std::cerr);
    }
    else if (type.size() >= 7 && type.substr(0,7) == "console")
    {
        if (type.size() == 15 && type.substr(7,8) == "|nocolor")
            return rc_new<ConsoleLogHandler>(false);
        else
            return rc_new<ConsoleLogHandler>(true);
    }
    else if (type.size() > 5 && type.substr(0,5) == "file|")
    {
        std::string::size_type pos = type.find_last_of('|');
        std::string last = type.substr(pos + 1);
        if (type.size() > 12 && type.substr(5,7) == "append|")
        {
            return rc_new<FileLogHandler>(last.c_str(), true);
        }
        else if (type.size() > 12 && type.substr(5,7) == "circle|")
        {
            char buf[30] = {0};
#if defined(NUT_PLATFORM_OS_WINDOWS)
            long pid = ::GetCurrentProcessId();
#else
            pid_t pid = ::getpid();
#endif
            sprintf(buf, "%ld", (long)pid);
            last += Time().format_time("%Y-%m-%d %H-%M-%S ");
            last += buf;
            last += ".log";
            return rc_new<FileLogHandler>(last.c_str(), false);
        }
        else // trunc
        {
            return rc_new<FileLogHandler>(last.c_str(), false);
        }
    }
#if defined(NUT_PLATFORM_OS_LINUX)
    else if (type == "syslog")
    {
        return rc_new<SyslogLogHandler>(false);
    }
#endif

    /* default */
    return rc_new<StreamLogHandler>(std::cout);
}

}

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif
