/**
 * @file -
 * @author jingqi
 * @date 2012-12-10
 * @last-edit 2015-01-06 22:59:58 jingqi
 * @brief
 */

#ifndef ___HEADFILE_7584A409_3042_4BB3_AB87_B6C66D804476_
#define ___HEADFILE_7584A409_3042_4BB3_AB87_B6C66D804476_

#include <assert.h>
#include <stdio.h>

#include "icommand.hpp"
#include "iconsole.hpp"

namespace nut
{

class CmdExit : public ICommand
{
    weak_ref<IConsole> m_console;

public:
    CmdExit(weak_ref<IConsole> c)
        : m_console(c)
    {
        assert(!c.is_null());
    }

    virtual const char* get_command_name() const
    {
        return "exit";
    }

    virtual const char* get_general_info() const
    {
        return "exit current pconsole";
    }

    virtual const char* get_detail_info() const
    {
        return "exit\neg.:\n\texit";
    }

    virtual void execute(const char* command_line)
    {
        (void) command_line; // unused
        printf("exit\n");
        m_console->exit(0);
    }
};

}

#endif
