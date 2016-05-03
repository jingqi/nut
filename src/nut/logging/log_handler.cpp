
#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_MAC
#   include <unistd.h> // for getpid()
#elif NUT_PLATFORM_OS_LINUX
#   include <unistd.h> // for getpid()
#   include <syslog.h>
#endif

#include <nut/util/console_util.h>
#include <nut/util/time/date_time.h>
#include <nut/util/string/to_string.h>

#include "log_handler.h"

namespace nut
{

LogHandler::LogHandler()
{}

void LogHandler::set_flush_mask(ll_mask_t mask)
{
    _flush_mask = mask;
}

LogFilter& LogHandler::get_filter()
{
    return _filter;
}

// -----------------------------------------------------------------------------

StreamLogHandler::StreamLogHandler(std::ostream &os)
    : _os(os)
{}

void StreamLogHandler::handle_log(const LogRecord &rec)
{
    _os << "[" << rec.get_time().get_clock_str() << "] " <<
         log_level_to_str(rec.get_level()) << " (" <<
            rec.get_file_name() << ":" << rec.get_line() << ") " <<
            rec.get_message() << std::endl;

    if (0 != (_flush_mask & rec.get_level()))
        _os.flush();
}

// -----------------------------------------------------------------------------

ConsoleLogHandler::ConsoleLogHandler(bool colored)
    : _colored(colored)
{}

void ConsoleLogHandler::set_colored(bool colored)
{
    _colored = colored;
}

void ConsoleLogHandler::handle_log(const LogRecord &rec)
{
    std::cout << "[" << rec.get_time().get_clock_str() << "] ";
    if (_colored)
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

    if (_colored)
        ConsoleUtil::set_text_color();

    std::cout << " (" << rec.get_file_name() << ":" << rec.get_line() <<
        ")  " << rec.get_message() << std::endl;

    if (0 != (_flush_mask & rec.get_level()))
        std::cout.flush();
}

// -----------------------------------------------------------------------------

FileLogHandler::FileLogHandler(const char *file, bool append)
    : _ofs(file, (append ? std::ios::app : std::ios::trunc))
{
    if (append)
        _ofs << "\n\n\n\n\n\n------------- ---------------- ---------------\n";
}

void FileLogHandler::handle_log(const LogRecord & rec)
{
    _ofs << rec.to_string() << std::endl;

    if (0 != (_flush_mask & rec.get_level()))
        _ofs.flush();
}

// -----------------------------------------------------------------------------

#if NUT_PLATFORM_OS_LINUX

SyslogLogHandler::SyslogLogHandler(bool close_syslog_on_exit)
    : _close_syslog_on_exit(close_syslog_on_exit)
{}

SyslogLogHandler::~SyslogLogHandler()
{
    if (_close_syslog_on_exit)
        ::closelog();  // the oposite way is openlog()
}

void SyslogLogHandler::set_close_syslog_on_exit(bool close_on_exit)
{
    _close_syslog_on_exit = close_on_exit;
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
    std::string msg = rec.to_string();
    ::syslog(level, msg.c_str());
}

#endif

}
