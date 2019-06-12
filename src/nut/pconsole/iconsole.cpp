﻿
#include <assert.h>

#include "../util/string/string_utils.h"
#include "iconsole.h"


namespace nut
{

/** 从指定的命令集中挑选出匹配名称的子集 */
std::vector<rc_ptr<ICommand> > IConsole::match_commands(
    const std::vector<rc_ptr<ICommand> >& commands, const std::string& to_match) noexcept
{
    // 匹配命令
    std::vector<rc_ptr<ICommand> > matched_cmds;
    for (size_t i = 0, size = commands.size(); i < size; ++i)
    {
        rc_ptr<ICommand> cmd = commands.at(i);
        assert(!cmd.is_null());

        // 强匹配
        if (nullptr != cmd->get_command_name() && to_match == cmd->get_command_name())
        {
            matched_cmds.clear();
            matched_cmds.push_back(std::move(cmd));
            return matched_cmds;
        }

        // 弱匹配
        if (nullptr != cmd->get_command_name() && starts_with(cmd->get_command_name(), to_match.c_str()))
        {
            matched_cmds.push_back(std::move(cmd));
        }
        else if (nullptr != cmd->get_command_nick_names())
        {
            const char** nn = cmd->get_command_nick_names();
            for (size_t j = 0; nullptr != nn[j]; ++j)
            {
                if (to_match == nn[j])
                {
                    matched_cmds.push_back(std::move(cmd));
                    break;
                }
            }
        }
    }
    return matched_cmds;
}

}
