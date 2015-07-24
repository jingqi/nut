
#ifndef ___HEADFILE_7584A409_3042_4BB3_AB87_B6C66D804476_
#define ___HEADFILE_7584A409_3042_4BB3_AB87_B6C66D804476_

#include <assert.h>
#include <stdio.h>

#include "icommand.h"
#include "iconsole.h"

namespace nut
{

class CmdExit : public ICommand
{
    IConsole *_console = NULL;

public:
    CmdExit(IConsole *c)
        : _console(c)
    {
        assert(NULL != c);
    }

    virtual const char* get_command_name() const override
    {
        return "exit";
    }

    virtual const char* get_general_info() const override
    {
        return "exit current pconsole";
    }

    virtual const char* get_detail_info() const override
    {
        return "exit\neg.:\n\texit";
    }

    virtual void execute(const char* command_line) override
    {
        (void) command_line; // unused
        printf("exit\n");
        _console->exit(0);
    }
};

}

#endif
