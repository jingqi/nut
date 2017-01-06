
#include <iostream>

#include <nut/util/console_util.h>

#include "console_log_handler.h"

namespace nut
{

ConsoleLogHandler::ConsoleLogHandler()
{
    _in_a_tty = ConsoleUtil::isatty();
}

void ConsoleLogHandler::handle_log(const LogRecord& rec)
{
    std::cout << "[" << rec.get_time().get_clock_str() << "] ";
    if (_in_a_tty)
    {
        switch (rec.get_level())
        {
        case LL_DEBUG:
            ConsoleUtil::set_text_color(ConsoleUtil::BLUE, ConsoleUtil::DEFAULT);
            break;

        case LL_INFO:
            ConsoleUtil::set_text_color(ConsoleUtil::GREEN, ConsoleUtil::DEFAULT);
            break;

        case LL_WARN:
            ConsoleUtil::set_text_color(ConsoleUtil::YELLOW, ConsoleUtil::DEFAULT);
            break;

        case LL_ERROR:
            ConsoleUtil::set_text_color(ConsoleUtil::PINK, ConsoleUtil::DEFAULT);
            break;

        case LL_FATAL:
            ConsoleUtil::set_text_color(ConsoleUtil::RED, ConsoleUtil::DEFAULT);
            break;

        default:
            break;
        }
        std::cout << log_level_to_char(rec.get_level());
    }
    else
    {
        std::cout << log_level_to_str(rec.get_level());
    }

    if (_in_a_tty)
        ConsoleUtil::set_text_color();

    if (_in_a_tty)
    {
        std::cout << " " << rec.get_message() << std::endl;
    }
    else
    {
        std::cout << " (" << rec.get_file_name() << ":" << rec.get_line() << ") " <<
            rec.get_message() << std::endl;
    }

    if (0 != (_flush_mask & rec.get_level()))
        std::cout.flush();
}

}
