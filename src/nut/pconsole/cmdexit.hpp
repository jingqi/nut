/**
 * @file -
 * @author jingqi
 * @date 2012-12-10
 * @last-edit 2012-12-10 21:12:21 jingqi
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
        assert(!c.isNull());
    }

    virtual const char* getCommandName() const
    {
        return "exit";
    }

    virtual const char* getGeneralInfo() const
    {
        return "exit current pconsole";
    }

    virtual const char* getDetailInfo() const
    {
        return "exit\neg.:\n\texit";
    }

    virtual void execute(const char* commandLine)
    {
        printf("exit\n");
        m_console->exit(0);
    }
};

}

#endif

