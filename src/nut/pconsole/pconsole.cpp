﻿
#include <assert.h>
#include <string>
#include <vector>

#include <nut/util/string/string_utils.h>

#include "pconsole.h"
#include "cmd_exit.h"
#include "cmd_help.h"

namespace nut
{

PConsole::PConsole(const std::string& name)
    : _prompt_string("PConsole"), _console_name(name)
{
    // 内建命令
    add_a_command(rc_new<CmdExit>(this));
    add_a_command(rc_new<CmdHelp>(this));
}

char PConsole::get_prompt_char() const
{
    return _prompt_char;
}

void PConsole::set_prompt_char(char c)
{
    _prompt_char = c;
}

const std::string& PConsole::get_prompt_string() const
{
    return _prompt_string;
}

void PConsole::set_prompt_string(const std::string& s)
{
    _prompt_string = s;
}

void PConsole::add_a_command(rc_ptr<ICommand> cmd)
{
    _commands.push_back(cmd);
}

// 读取并执行一次
void PConsole::read_and_execute()
{
    // 打印命令提示符
    printf("%s%c ", _prompt_string.c_str(), _prompt_char);

    // 从命令行读取命令并执行
    std::string l;
    while (true)
    {
        const char c = getchar();
        if ('\n' == c)
            break;
        else
            l += c;
    }
    execute_line(l.c_str());
}

// 执行直到用户输入exit命令
int PConsole::execute()
{
    _exit = false;
    _exit_value = 0;
    while (!_exit)
        read_and_execute();
    return _exit_value;
}

// 打印一般信息
void PConsole::print_general_info() const
{
    printf("%s\n", _console_name.c_str());
    for (size_t i = 0, size = _commands.size(); i < size; ++i)
    {
        rc_ptr<ICommand> cmd = _commands.at(i);
        assert(!cmd.is_null());
        const char* cmd_name = cmd->get_command_name();
        const char* cmd_info = cmd->get_general_info();
        printf("\t%s %s\n", (nullptr == cmd_name ? "(null)" : cmd_name),
               (nullptr == cmd_info ? "" : cmd_info));
    }
}

// 获取命令列表
const std::vector<rc_ptr<ICommand> >& PConsole::get_commands() const
{
    return _commands;
}

// 设置退出标记
void PConsole::exit(int e)
{
    _exit_value = e;
    _exit = true;
}

void PConsole::execute_line(const char* l)
{
    assert(nullptr != l);
    // 首先取出命令名
    size_t start = 0;
    while (l[start] != '\0' && (l[start] == ' ' || l[start] == '\t'))
        ++start;
    size_t end = start;
    while (l[end] != '\0' && l[end] != ' ')
        ++end;
    if (start == end)
        return;
    const std::string cmd_name(l + start, l + end);

    // 匹配命令
    std::vector<rc_ptr<ICommand> > matched_cmds = match_commands(_commands, cmd_name);

    // 无匹配
    if (matched_cmds.size() == 0)
    {
        printf("\nERROR: command not found, \"%s\"\n", cmd_name.c_str());
        return;
    }

    // 匹配数多于1
    if (matched_cmds.size() > 1)
    {
        printf("\nMore than one command matched:\n\t");
        for (size_t i = 0, size = matched_cmds.size(); i < size; ++i)
        {
            const char *n = matched_cmds.at(i)->get_command_name();
            printf(" %s", (nullptr == n ? "(null)" : n));
        }
        return;
    }

    // 执行
    matched_cmds.at(0)->execute(l);
}

}
