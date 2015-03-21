/**
 * @file -
 * @author jingqi
 * @date 2012-12-10
 * @last-edit 2015-01-06 22:58:50 jingqi
 * @brief
 */

#ifndef ___HEADFILE_8FCF1438_56E0_43B3_AB98_BA0EEEB3F7C3_
#define ___HEADFILE_8FCF1438_56E0_43B3_AB98_BA0EEEB3F7C3_

#include <assert.h>
#include <stdio.h>

#include "icommand.hpp"
#include "iconsole.hpp"

namespace nut
{

class CmdHelp : public ICommand
{
    IConsole *m_console;

public:
    CmdHelp(IConsole *c)
        : m_console(c)
    {
        assert(NULL != c);
    }

    virtual const char* get_command_name() const
    {
        return "help";
    }

    virtual const char** get_command_nick_names() const
    {
        static const char* ret[] = {
            "hlp",
            NULL
        };
        return ret;
    }

    virtual const char* get_general_info() const
    {
        return "print some helpfull information";
    }

    virtual const char* get_detail_info() const
    {
        return "help [command_name]\neg.:\n\thelp\n\thelp exit";
    }

    virtual void execute(const char* command_line)
    {
        assert(NULL != command_line);
        const std::vector<std::string> params = parse_comand_line(command_line);

        // 空参数
        if (params.size() <= 1)
        {
            m_console->print_general_info();
            return;
        }
        const std::string cmd_name = params.at(1);

        // 匹配命令
        std::vector<rc_ptr<ICommand> > matched_cmds = IConsole::match_commands(m_console->get_commands(), cmd_name);

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
                printf(" %s", (NULL == n ? "(null)" : n));
            }
            return;
        }

        // 打印帮助信息
        const char *i = matched_cmds.at(0)->get_detail_info();
        printf("%s\n", (NULL == i ? "(No information found)" : i));
    }
};

}

#endif
