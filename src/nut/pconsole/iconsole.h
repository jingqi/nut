
#ifndef ___HEADFILE_9BD7EC68_5972_4758_AFA7_18EC2DF93A8B_
#define ___HEADFILE_9BD7EC68_5972_4758_AFA7_18EC2DF93A8B_

#include <string>
#include <vector>

#include <nut/rc/rc_ptr.h>

#include "../nut_config.h"
#include "icommand.h"


namespace nut
{

class NUT_API IConsole
{
    NUT_REF_COUNTABLE

public:
    /** 打印伪命令行的一般信息 */
    virtual void print_general_info() const = 0;

    /** 获取所有的命令 */
    virtual const std::vector<rc_ptr<ICommand> >& get_commands() const = 0;

    /** 退出当前伪命令行 */
    virtual void exit(int e) = 0;

    /** 从指定的命令集中挑选出匹配名称的子集 */
    static std::vector<rc_ptr<ICommand> > match_commands(const std::vector<rc_ptr<ICommand> >& commands, const std::string& to_match);
};

}

#endif
