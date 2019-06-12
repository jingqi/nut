
#ifndef ___HEADFILE_8FCF1438_56E0_43B3_AB98_BA0EEEB3F7C3_
#define ___HEADFILE_8FCF1438_56E0_43B3_AB98_BA0EEEB3F7C3_

#include "../nut_config.h"
#include "icommand.h"
#include "iconsole.h"


namespace nut
{

class NUT_API CmdHelp : public ICommand
{
public:
    explicit CmdHelp(IConsole *c) noexcept;

    virtual const char* get_command_name() const noexcept override;

    virtual const char** get_command_nick_names() const noexcept override;

    virtual const char* get_general_info() const noexcept override;

    virtual const char* get_detail_info() const noexcept override;

    virtual void execute(const char* command_line) noexcept override;

private:
    IConsole *_console = nullptr;
};

}

#endif
