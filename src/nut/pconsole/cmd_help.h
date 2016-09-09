
#ifndef ___HEADFILE_8FCF1438_56E0_43B3_AB98_BA0EEEB3F7C3_
#define ___HEADFILE_8FCF1438_56E0_43B3_AB98_BA0EEEB3F7C3_

#include <nut/nut_config.h>

#include "icommand.h"
#include "iconsole.h"

namespace nut
{

class NUT_API CmdHelp : public ICommand
{
    IConsole *_console = NULL;

public:
    CmdHelp(IConsole *c);

    virtual const char* get_command_name() const override;

    virtual const char** get_command_nick_names() const override;

    virtual const char* get_general_info() const override;

    virtual const char* get_detail_info() const override;

    virtual void execute(const char* command_line) override;
};

}

#endif
