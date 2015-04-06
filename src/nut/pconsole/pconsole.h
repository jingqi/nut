
#ifndef ___HEADFILE_823DB076_2993_4B27_A90B_0072994B4D37_
#define ___HEADFILE_823DB076_2993_4B27_A90B_0072994B4D37_

#include <assert.h>
#include <string>
#include <vector>

#include <nut/rc/rc_new.hpp>
#include <nut/util/string/string_util.h>

#include "icommand.h"
#include "iconsole.h"

namespace nut
{

/**
 * Pseudo console
 */
class PConsole : public IConsole
{
    char m_prompt_char;
    std::string m_prompt_string;
    std::string m_console_name;
    std::vector<rc_ptr<ICommand> > m_commands;
    bool m_exit;
    int m_exit_value;

public:
    PConsole(const std::string& name = "PConsole");

    char get_prompt_char() const
    {
        return m_prompt_char;
    }

    void set_prompt_char(char c)
    {
        m_prompt_char = c;
    }

    const std::string& get_prompt_string() const
    {
        return m_prompt_string;
    }

    void set_prompt_string(const std::string& s)
    {
        m_prompt_string = s;
    }

    /** 添加一个命令 */
    void add_a_command(rc_ptr<ICommand> cmd)
    {
        m_commands.push_back(cmd);
    }

    /** 读取并执行一次 */
    void read_and_execute();

    /** 执行直到用户输入exit命令 */
    int execute();

    /** 打印一般信息 */
    virtual void print_general_info() const override;

    /** 获取命令列表 */
    virtual const std::vector<rc_ptr<ICommand> >& get_commands() const override
    {
        return m_commands;
    }

    /** 设置退出标记 */
    virtual void exit(int e) override;

private:
    void execute_line(const char* l);
};

}

#endif
