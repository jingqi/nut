
#include <iostream>

#include <nut/util/console_util.h>

#include "console_log_handler.h"

namespace nut
{

ConsoleLogHandler::ConsoleLogHandler(bool colored)
    : _colored(colored)
{
    _in_a_tty = ConsoleUtil::isatty();
}

void ConsoleLogHandler::set_colored(bool colored)
{
    _colored = colored;
}

void ConsoleLogHandler::handle_log(const LogRecord& rec)
{
    std::cout << "[" << rec.get_time().get_clock_str() << "] ";
    if (_colored && _in_a_tty)
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

    if (_colored && _in_a_tty)
        ConsoleUtil::set_text_color();

    std::cout << " " << rec.get_message() << std::endl;

    if (0 != (_flush_mask & rec.get_level()))
        std::cout.flush();
}

}
