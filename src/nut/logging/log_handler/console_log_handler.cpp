
#include <iostream>
#include <mutex> // for std::lock_guard

#include "../../util/console_util.h"
#include "../../util/string/to_string.h"
#include "console_log_handler.h"


namespace nut
{

ConsoleLogHandler::ConsoleLogHandler(bool verbose) noexcept
    : _in_a_tty(ConsoleUtil::isatty()), _verbose(verbose)
{}

static ConsoleColor get_level_color(enum LogLevel level) noexcept
{
    switch (level)
    {
    case LL_DEBUG:
        return ConsoleColor::Blue;

    case LL_INFO:
        return ConsoleColor::Green;

    case LL_WARN:
        return ConsoleColor::Yellow;

    case LL_ERROR:
        return ConsoleColor::Pink;

    case LL_FATAL:
        return ConsoleColor::Red;

    default:
        return ConsoleColor::White;
    }
}

void ConsoleLogHandler::handle_log(const LogRecord& rec) noexcept
{
    std::string first_seg = "[";
    first_seg += rec.get_time().format_time(
        _verbose ? "%m-%d %H:%M:%S.%6f" : "%H:%M:%S.%3f");
    first_seg += "] ";

    std::string level;
    if (_verbose)
        level = log_level_to_str(rec.get_level());
    else
        level.push_back(log_level_to_char(rec.get_level()));

    std::string last_seg = " ";
    if (_verbose)
    {
        last_seg += rec.get_tag();
        last_seg += " (";
        last_seg += rec.get_file_name();
        last_seg.push_back(':');
        last_seg += int_to_str(rec.get_line());
        last_seg += ") ";
        last_seg += rec.get_message();
    }
    else
    {
        last_seg += rec.get_message();
    }

    std::lock_guard<SpinLock> guard(_lock);

    if (_in_a_tty)
    {
        std::cout << first_seg;

        const ConsoleColor color = get_level_color(rec.get_level());
        ConsoleUtil::set_text_color(color, ConsoleColor::Default);
        std::cout << level;
        ConsoleUtil::set_text_color();

        std::cout << last_seg << std::endl;
    }
    else
    {
        std::cout << first_seg << level << last_seg << std::endl;
    }

    if (0 != (_flush_mask & rec.get_level()))
        std::cout.flush();
}

}
