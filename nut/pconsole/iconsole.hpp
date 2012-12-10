/**
 * @file -
 * @author jingqi
 * @date 2012-12-10
 * @last-edit 2012-12-10 21:30:47 jingqi
 * @brief
 */

#ifndef ___HEADFILE_9BD7EC68_5972_4758_AFA7_18EC2DF93A8B_
#define ___HEADFILE_9BD7EC68_5972_4758_AFA7_18EC2DF93A8B_

#include <assert.h>
#include <string>
#include <vector>

#include <nut/gc/ref.hpp>
#include <nut/util/string/stringutil.hpp>

#include "icommand.hpp"

namespace nut
{

class IConsole
{
    NUT_GC_REFERABLE

public:
    /** 打印伪命令行的一般信息 */
    virtual void printGeneralInfo() const = 0;

    /** 获取所有的命令 */
    virtual const std::vector<ref<ICommand> >& getCommands() const = 0;

    /** 退出当前伪命令行 */
    virtual void exit(int e) = 0;

public:
    /** 从指定的命令集中挑选出匹配名称的子集 */
    static std::vector<ref<ICommand> > matchCommands(const std::vector<ref<ICommand> >& commands, const std::string& toMatch)
    {   
        // 匹配命令
        std::vector<ref<ICommand> > matched_cmds;
        for (size_t i = 0, size = commands.size(); i < size; ++i)
        {
            ref<ICommand> cmd = commands.at(i);
            assert(!cmd.isNull());

            // 强匹配
            if (NULL != cmd->getCommandName() && toMatch == cmd->getCommandName())
            {
                matched_cmds.clear();
                matched_cmds.push_back(cmd);
                return matched_cmds;
            }

            // 弱匹配
            if (NULL != cmd->getCommandName() && starts_with(cmd->getCommandName(), toMatch))
            {
                matched_cmds.push_back(cmd);
            }
            else if (NULL != cmd->getCommandNickNames())
            {
                const char** nn = cmd->getCommandNickNames();
                for (size_t j = 0; NULL != nn[j]; ++j)
                {
                    if (toMatch == nn[j])
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


