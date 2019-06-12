
#include "../platform/platform.h"
#include "cmd_exit.h"


namespace nut
{

CmdExit::CmdExit(IConsole *c) noexcept
    : _console(c)
{
    assert(nullptr != c);
}

const char* CmdExit::get_command_name() const noexcept
{
    return "exit";
}

const char* CmdExit::get_general_info() const noexcept
{
    return "exit current pconsole";
}

const char* CmdExit::get_detail_info() const noexcept
{
    return "exit\neg.:\n\texit";
}

void CmdExit::execute(const char* command_line) noexcept
{
    UNUSED(command_line);

    printf("exit\n");
    _console->exit(0);
}

}
