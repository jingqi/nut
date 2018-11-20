
#include <iostream>

#include <nut/util/console_util.h>

#include "console_log_handler.h"

namespace nut
{

ConsoleLogHandler::ConsoleLogHandler(bool abbr_mode)
    : _abbr_mode(abbr_mode)
{
    _in_a_tty = ConsoleUtil::isatty();
}

void ConsoleLogHandler::handle_log(const LogRecord& rec)
{
    if (_abbr_mode)
        std::cout << "[" << rec.get_time().format_time("%H:%M:%S.%3f") << "] ";
    else
        std::cout << "[" << rec.get_time().to_string() << "] ";

    if (_in_a_tty)
    {
        switch (rec.get_level())
        {
        case LogLevel::Debug:
            ConsoleUtil::set_text_color(ConsoleColor::Blue, ConsoleColor::Default);
            break;

        case LogLevel::Info:
            ConsoleUtil::set_text_color(ConsoleColor::Green, ConsoleColor::Default);
            break;

        case LogLevel::Warn:
            ConsoleUtil::set_text_color(ConsoleColor::Yellow, ConsoleColor::Default);
            break;

        case LogLevel::Error:
            ConsoleUtil::set_text_color(ConsoleColor::Pink, ConsoleColor::Default);
            break;

        case LogLevel::Fatal:
            ConsoleUtil::set_text_color(ConsoleColor::Red, ConsoleColor::Default);
            break;

        default:
            break;
        }
    }

    if (_abbr_mode)
        std::cout << log_level_to_char(rec.get_level());
    else
        std::cout << log_level_to_str(rec.get_level());

    if (_in_a_tty)
        ConsoleUtil::set_text_color();

    if (_abbr_mode)
    {
        std::cout << " " << rec.get_message() << std::endl;
    }
    else
    {
        std::cout << " (" << rec.get_file_name() << ":" << rec.get_line() << ") " <<
            rec.get_message() << std::endl;
    }

    if (0 != (_flush_mask & static_cast<loglevel_mask_type>(rec.get_level())))
        std::cout.flush();
}

}
