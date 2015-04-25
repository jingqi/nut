
#ifndef ___HEADFILE___4CDF318F_AF06_4CEF_BAC8_DE26853A73AB_
#define ___HEADFILE___4CDF318F_AF06_4CEF_BAC8_DE26853A73AB_

#include <iostream>
#include <fstream>
#include <vector>

#include <nut/platform/platform.h>

#include <nut/rc/rc_new.h>

#include "log_record.h"
#include "log_filter.h"

namespace nut
{

class LogHandler
{
    LogFilter m_filter;

public:
    NUT_REF_COUNTABLE

    LogFilter& get_filter()
    {
        return m_filter;
    }

    virtual void handle_log(const LogRecord &rec) = 0;
};

class StreamLogHandler : public LogHandler
{
    std::ostream &m_os;

public:
    StreamLogHandler (std::ostream &os)
        : m_os(os)
    {}

    virtual void handle_log(const LogRecord &rec) override;
};

class ConsoleLogHandler : public LogHandler
{
    bool m_colored;

public:
    ConsoleLogHandler(bool colored = true)
        : m_colored(colored)
    {}

    virtual void handle_log(const LogRecord &l) override;
};

class FileLogHandler : public LogHandler
{
    std::ofstream m_ofs;

public:
    FileLogHandler(const char *file, bool append = false);

    virtual void handle_log(const LogRecord & rec) override;
};

#if defined(NUT_PLATFORM_OS_LINUX)
class SyslogLogHandler : public LogHandler
{
    bool m_close_syslog_on_exit;

public:
    SyslogLogHandler(bool close_syslog_on_exit = false)
        : m_close_syslog_on_exit(close_syslog_on_exit)
    {}

    ~SyslogLogHandler();

    virtual void handle_log(const LogRecord &rec) override;
};
#endif

}

#endif  // head file guarder
