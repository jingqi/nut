
#include <assert.h>
#include <string>
#include <vector>

#include <nut/util/string/string_util.h>

#include "pconsole.h"
#include "cmd_exit.h"
#include "cmd_help.h"

namespace nut
{

PConsole::PConsole(const std::string& name)
    : m_prompt_char('>'), m_prompt_string("PConsole"), m_console_name(name),
      m_exit(false), m_exit_value(0)
{
    // 内建命令
    add_a_command(rc_new<CmdExit>(this));
    add_a_command(rc_new<CmdHelp>(this));
}

/** 读取并执行一次 */
void PConsole::read_and_execute()
{
    // 打印命令提示符
    printf("%s%c ", m_prompt_string.c_str(), m_prompt_char);

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

/** 执行直到用户输入exit命令 */
int PConsole::execute()
{
    m_exit = false;
    m_exit_value = 0;
    while (!m_exit)
        read_and_execute();
    return m_exit_value;
}

/** 打印一般信息 */
void PConsole::print_general_info() const
{
    printf("%s\n", m_console_name.c_str());
    for (size_t i = 0, size = m_commands.size(); i < size; ++i)
    {
        rc_ptr<ICommand> cmd = m_commands.at(i);
        assert(!cmd.is_null());
        const char* cmd_name = cmd->get_command_name();
        const char* cmd_info = cmd->get_general_info();
        printf("\t%s %s\n", (NULL == cmd_name ? "(null)" : cmd_name), (NULL == cmd_info ? "" : cmd_info));
    }
}

/** 设置退出标记 */
void PConsole::exit(int e)
{
    m_exit_value = e;
    m_exit = true;
}

void PConsole::execute_line(const char* l)
{
    assert(NULL != l);
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
    std::vector<rc_ptr<ICommand> > matched_cmds = match_commands(m_commands, cmd_name);

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
            printf(" %s", (NULL == n ? "(null)" : n));
        }
        return;
    }

    // 执行
    matched_cmds.at(0)->execute(l);
}

}
