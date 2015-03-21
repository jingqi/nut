/**
 * @file -
 * @author jingqi
 * @date 2012-12-10
 * @last-edit 2015-01-06 22:49:52 jingqi
 * @brief
 */

#ifndef ___HEADFILE_9BD7EC68_5972_4758_AFA7_18EC2DF93A8B_
#define ___HEADFILE_9BD7EC68_5972_4758_AFA7_18EC2DF93A8B_

#include <assert.h>
#include <string>
#include <vector>

#include <nut/rc/rc_ptr.hpp>
#include <nut/util/string/string_util.hpp>

#include "icommand.hpp"

namespace nut
{

class IConsole
{
    NUT_REF_COUNTABLE

public:
    /** 打印伪命令行的一般信息 */
    virtual void print_general_info() const = 0;

    /** 获取所有的命令 */
    virtual const std::vector<rc_ptr<ICommand> >& get_commands() const = 0;

    /** 退出当前伪命令行 */
    virtual void exit(int e) = 0;

public:
    /** 从指定的命令集中挑选出匹配名称的子集 */
    static std::vector<rc_ptr<ICommand> > match_commands(const std::vector<rc_ptr<ICommand> >& commands, const std::string& to_match)
    {
        // 匹配命令
        std::vector<rc_ptr<ICommand> > matched_cmds;
        for (size_t i = 0, size = commands.size(); i < size; ++i)
        {
            rc_ptr<ICommand> cmd = commands.at(i);
            assert(!cmd.is_null());

            // 强匹配
            if (NULL != cmd->get_command_name() && to_match == cmd->get_command_name())
            {
                matched_cmds.clear();
                matched_cmds.push_back(cmd);
                return matched_cmds;
            }

            // 弱匹配
            if (NULL != cmd->get_command_name() && starts_with(cmd->get_command_name(), to_match))
            {
                matched_cmds.push_back(cmd);
            }
            else if (NULL != cmd->get_command_nick_names())
            {
                const char** nn = cmd->get_command_nick_names();
                for (size_t j = 0; NULL != nn[j]; ++j)
                {
                    if (to_match == nn[j])
                    {
                        matched_cmds.push_back(cmd);
                        break;
                    }
                }
            }
        }
        return matched_cmds;
    }
};

}

#endif
