
#include "../../platform/platform.h"

#if NUT_PLATFORM_OS_MACOS || NUT_PLATFORM_OS_LINUX

#include <syslog.h>

#include "syslog_log_handler.h"


namespace nut
{

void SyslogLogHandler::handle_log(const LogRecord& rec) noexcept
{
    int level = 0;
    switch (rec.get_level())
    {
    case LL_DEBUG:
        level = LOG_DEBUG;
        break;

    case LL_INFO:
        level = LOG_INFO;
        break;

    case LL_WARN:
        level = LOG_WARNING;
        break;

    case LL_ERROR:
        level = LOG_ERR;
        break;

    case LL_FATAL:
        level = LOG_CRIT;
        break;

    default:
        assert(false);
    }

    // NOTE 为了安全(例如 msg 本身带有非预期的 % 字符), 需避免 msg 被再次格式化
    //      解析
    const std::string msg = rec.to_string();
    ::syslog(level, "%s", msg.c_str());
}

}

#endif
