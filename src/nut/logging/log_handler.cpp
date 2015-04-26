
#include <nut/platform/platform.h>

#if defined(NUT_PLATFORM_OS_MAC)
#   include <unistd.h> // for getpid()
#elif defined(NUT_PLATFORM_OS_LINUX)
#   include <unistd.h> // for getpid()
#   include <syslog.h>
#endif

#include <nut/util/console_util.h>
#include <nut/util/time/date_time.h>
#include <nut/util/string/to_string.h>

#include "log_handler.h"

namespace nut
{

void StreamLogHandler::handle_log(const LogRecord &rec)
{
    m_os << "[" << rec.get_time().get_clock_str() << "] " <<
         log_level_to_str(rec.get_level()) << " (" <<
            rec.get_file_name() << ":" << rec.get_line() << ") " <<
            rec.get_message() << std::endl;

    if (0 != (m_flush_mask & rec.get_level()))
        m_os.flush();
}

void ConsoleLogHandler::handle_log(const LogRecord &rec)
{
    std::cout << "[" << rec.get_time().get_clock_str() << "] ";
    if (m_colored)
    {
        switch (rec.get_level())
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

    std::cout << log_level_to_str(rec.get_level());

    if (m_colored)
        ConsoleUtil::set_text_color();

    std::cout << " (" << rec.get_file_name() << ":" << rec.get_line() <<
        ")  " << rec.get_message() << std::endl;

    if (0 != (m_flush_mask & rec.get_level()))
        std::cout.flush();
}

FileLogHandler::FileLogHandler(const char *file, bool append)
    : m_ofs(file, (append ? std::ios::app : std::ios::trunc))
{
    if (append)
        m_ofs << "\n\n\n\n\n\n------------- ---------------- ---------------\n";
}

void FileLogHandler::handle_log(const LogRecord & rec)
{
    std::string msg;
    rec.to_string(&msg);
    m_ofs << msg << std::endl;

    if (0 != (m_flush_mask & rec.get_level()))
        m_ofs.flush();
}

#if defined(NUT_PLATFORM_OS_LINUX)

SyslogLogHandler::~SyslogLogHandler()
{
    if (m_close_syslog_on_exit)
        closelog();  // the oposite way is openlog()
}

void SyslogLogHandler::handle_log(const LogRecord &rec)
{
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
    std::string msg;
    rec.to_string(&msg);
    syslog(level, msg.c_str());
}

#endif

}
