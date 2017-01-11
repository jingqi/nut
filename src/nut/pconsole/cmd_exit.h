
#ifndef ___HEADFILE_7584A409_3042_4BB3_AB87_B6C66D804476_
#define ___HEADFILE_7584A409_3042_4BB3_AB87_B6C66D804476_

#include <assert.h>
#include <stdio.h>

#include "../nut_config.h"
#include "icommand.h"
#include "iconsole.h"


namespace nut
{

class NUT_API CmdExit : public ICommand
{
    IConsole *_console = nullptr;

public:
    CmdExit(IConsole *c);

    virtual const char* get_command_name() const override;

    virtual const char* get_general_info() const override;

    virtual const char* get_detail_info() const override;

    virtual void execute(const char* command_line) override;
};

}

#endif
