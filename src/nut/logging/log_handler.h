
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
    LogFilter m_filter;     // 过滤器

protected:
    ll_mask_t m_flush_mask; // 控制那些日志需要立即刷新到磁盘

public:
    NUT_REF_COUNTABLE

    LogHandler()
        : m_flush_mask(LL_FATAL)
    {}

    void set_flush_mask(ll_mask_t mask)
    {
        m_flush_mask = mask;
    }

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
    StreamLogHandler(std::ostream &os)
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

    void set_colored(bool colored)
    {
        m_colored = colored;
    }

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

    void set_close_syslog_on_exit(bool close_on_exit)
    {
        m_close_syslog_on_exit = close_on_exit;
    }

    virtual void handle_log(const LogRecord &rec) override;
};
#endif

}

#endif  // head file guarder
