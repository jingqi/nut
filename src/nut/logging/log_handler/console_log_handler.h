
#ifndef ___HEADFILE_78125394_FD78_4023_AE0B_E382527514CB_
#define ___HEADFILE_78125394_FD78_4023_AE0B_E382527514CB_

#include "../../nut_config.h"
#include "log_handler.h"


namespace nut
{

class NUT_API ConsoleLogHandler : public LogHandler
{
    bool _in_a_tty = false;
    bool _abbr_mode = true;

public:
    ConsoleLogHandler(bool abbr_mode = true);

    virtual void handle_log(const LogRecord& l) override;
};

}

#endif
