﻿
#include <nut/platform/platform.h>

#if NUT_PLATFORM_OS_MAC || NUT_PLATFORM_OS_LINUX

#include <syslog.h>

#include "syslog_log_handler.h"

namespace nut
{

SyslogLogHandler::SyslogLogHandler(bool close_syslog_on_exit)
    : _close_syslog_on_exit(close_syslog_on_exit)
{}

SyslogLogHandler::~SyslogLogHandler()
{
    if (_close_syslog_on_exit)
        ::closelog();  // The oposite way is openlog()
}

void SyslogLogHandler::set_close_syslog_on_exit(bool close_on_exit)
{
    _close_syslog_on_exit = close_on_exit;
}

void SyslogLogHandler::handle_log(const LogRecord& rec)
{
    int level = 0;
    switch (rec.get_level())
    {
    case LogLevel::Debug:
        level = LOG_DEBUG;
        break;

    case LogLevel::Info:
        level = LOG_INFO;
        break;

    case LogLevel::Warn:
        level = LOG_WARNING;
        break;

    case LogLevel::Error:
        level = LOG_ERR;
        break;

    case LogLevel::Fatal:
        level = LOG_CRIT;
        break;

    default:
        assert(false);
    }
    // NOTE None-constant string should be treated as an argument of format string
    //      to avoid of security issue
    std::string msg = rec.to_string();
    ::syslog(level, "%s", msg.c_str());
}

}

#endif
