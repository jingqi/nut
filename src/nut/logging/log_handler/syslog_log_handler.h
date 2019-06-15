/**
 * syslog 接口
 *
 * ::openlog()
 * ::syslog()
 * ::vsyslog()
 * ::closelog()
 *
 * - 如未调用过 openlog(), 将在首次调用 syslog() / vsyslog() 时自动初始化
 * - closelog() 关闭与 syslog 守护进程通信的文件描述符, 也是可选的
 */

#ifndef ___HEADFILE_AF6CB8AD_3974_4914_A35E_B86138464E05_
#define ___HEADFILE_AF6CB8AD_3974_4914_A35E_B86138464E05_

#include "../../platform/platform.h"

#if NUT_PLATFORM_OS_MACOS || NUT_PLATFORM_OS_LINUX

#include "../../nut_config.h"
#include "log_handler.h"


namespace nut
{

/**
 * syslog 接口适配
 */
class NUT_API SyslogLogHandler : public LogHandler
{
public:
    SyslogLogHandler() = default;

    virtual void handle_log(const LogRecord& rec) noexcept override;

private:
    SyslogLogHandler(const SyslogLogHandler&) = delete;
    SyslogLogHandler& operator=(const SyslogLogHandler&) = delete;
};

}

#endif

#endif // File head guarder
