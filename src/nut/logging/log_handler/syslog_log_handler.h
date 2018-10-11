
#ifndef ___HEADFILE_AF6CB8AD_3974_4914_A35E_B86138464E05_
#define ___HEADFILE_AF6CB8AD_3974_4914_A35E_B86138464E05_

#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_MAC || NUT_PLATFORM_OS_LINUX

#include "../../nut_config.h"
#include "log_handler.h"


namespace nut
{

class NUT_API SyslogLogHandler : public LogHandler
{
public:
    explicit SyslogLogHandler(bool close_syslog_on_exit = false);

    ~SyslogLogHandler();

    void set_close_syslog_on_exit(bool close_on_exit);

    virtual void handle_log(const LogRecord& rec) override;

private:
    SyslogLogHandler(const SyslogLogHandler&) = delete;
    SyslogLogHandler& operator=(const SyslogLogHandler&) = delete;

private:
    bool _close_syslog_on_exit = false;
};

}

#endif

#endif // File head guarder
