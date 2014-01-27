/**
 * @file -
 * @author jingqi
 * @date 2012-12-10
 * @last-edit 2012-12-10 21:37:25 jingqi
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
    weak_ref<IConsole> m_console;

public:
    CmdHelp(weak_ref<IConsole> c)
        : m_console(c)
    {
        assert(!c.isNull());
    }

    virtual const char* getCommandName() const
    {
        return "help";
    }

    virtual const char** getCommandNickNames() const
    {
        static const char* ret[] = {
            "hlp",
            NULL
        };
        return ret;
    }

    virtual const char* getGeneralInfo() const
    {
        return "print some helpfull information";
    }

    virtual const char* getDetailInfo() const
    {
        return "help [command_name]\neg.:\n\thelp\n\thelp exit";
    }

    virtual void execute(const char* commandLine)
    {
        assert(NULL != commandLine);
        const std::vector<std::string> params = parseComandLine(commandLine);
        
        // 空参数
        if (params.size() <= 1)
        {
            m_console->printGeneralInfo();
            return;
        }
        const std::string cmd_name = params.at(1);

        // 匹配命令
        std::vector<ref<ICommand> > matched_cmds = IConsole::matchCommands(m_console->getCommands(), cmd_name);

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
                const char *n = matched_cmds.at(i)->getCommandName();
                printf(" %s", (NULL == n ? "(null)" : n));
            }
            return;
        }

        // 打印帮助信息
        const char *i = matched_cmds.at(0)->getDetailInfo();
        printf("%s\n", (NULL == i ? "(No information found)" : i));
    }
};

}

#endif


