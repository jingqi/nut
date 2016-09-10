
#ifndef ___HEADFILE_AF6CB8AD_3974_4914_A35E_B86138464E05_
#define ___HEADFILE_AF6CB8AD_3974_4914_A35E_B86138464E05_

#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_MAC || NUT_PLATFORM_OS_LINUX

#include <nut/nut_config.h>

#include "log_handler.h"

namespace nut
{

class NUT_API SyslogLogHandler : public LogHandler
{
    bool _close_syslog_on_exit = false;

public:
    SyslogLogHandler(bool close_syslog_on_exit = false);

    ~SyslogLogHandler();

    void set_close_syslog_on_exit(bool close_on_exit);

    virtual void handle_log(const LogRecord& rec) override;
};

}

#endif

#endif // File head guarder
