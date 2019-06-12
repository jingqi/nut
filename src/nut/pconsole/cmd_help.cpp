
#include <assert.h>
#include <stdio.h>

#include "cmd_help.h"


namespace nut
{

CmdHelp::CmdHelp(IConsole *c) noexcept
    : _console(c)
{
    assert(nullptr != c);
}

const char* CmdHelp::get_command_name() const noexcept
{
    return "help";
}

const char** CmdHelp::get_command_nick_names() const noexcept
{
    static const char* ret[] = {
        "hlp",
        nullptr
    };
    return ret;
}

const char* CmdHelp::get_general_info() const noexcept
{
    return "print some helpfull information";
}

const char* CmdHelp::get_detail_info() const noexcept
{
    return "help [command_name]\neg.:\n\thelp\n\thelp exit";
}

void CmdHelp::execute(const char* command_line) noexcept
{
    assert(nullptr != command_line);
    const std::vector<std::string> params = parse_comand_line(command_line);

    // 空参数
    if (params.size() <= 1)
    {
        _console->print_general_info();
        return;
    }
    const std::string cmd_name = params.at(1);

    // 匹配命令
    std::vector<rc_ptr<ICommand> > matched_cmds = IConsole::match_commands(_console->get_commands(), cmd_name);

    // 无匹配
    if (matched_cmds.size() == 0)
    {
        printf("ERROR: command not found, \"%s\"\n", cmd_name.c_str());
        return;
    }

    // 多于一个的匹配
    if (matched_cmds.size() > 1)
    {
        printf("More than one command matched:\n\t");
        for (size_t i = 0, size = matched_cmds.size(); i < size; ++i)
        {
            const char *n = matched_cmds.at(i)->get_command_name();
            printf(" %s", (nullptr == n ? "(null)" : n));
        }
        return;
    }

    // 打印帮助信息
    const char *i = matched_cmds.at(0)->get_detail_info();
    printf("%s\n", (nullptr == i ? "(No information found)" : i));
}

}
