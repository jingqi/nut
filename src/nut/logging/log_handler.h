
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
    LogFilter _filter;     // 过滤器

protected:
    ll_mask_t _flush_mask = LL_FATAL; // 控制那些日志需要立即刷新到磁盘

public:
    NUT_REF_COUNTABLE

    LogHandler()
    {}

    void set_flush_mask(ll_mask_t mask)
    {
        _flush_mask = mask;
    }

    LogFilter& get_filter()
    {
        return _filter;
    }

    virtual void handle_log(const LogRecord &rec) = 0;
};

class StreamLogHandler : public LogHandler
{
    std::ostream &_os;

public:
    StreamLogHandler(std::ostream &os)
        : _os(os)
    {}

    virtual void handle_log(const LogRecord &rec) override;
};

class ConsoleLogHandler : public LogHandler
{
    bool _colored = true;

public:
    ConsoleLogHandler(bool colored = true)
        : _colored(colored)
    {}

    void set_colored(bool colored)
    {
        _colored = colored;
    }

    virtual void handle_log(const LogRecord &l) override;
};

class FileLogHandler : public LogHandler
{
    std::ofstream _ofs;

public:
    FileLogHandler(const char *file, bool append = false);

    virtual void handle_log(const LogRecord & rec) override;
};

#if defined(NUT_PLATFORM_OS_LINUX)
class SyslogLogHandler : public LogHandler
{
    bool _close_syslog_on_exit = false;

public:
    SyslogLogHandler(bool close_syslog_on_exit = false)
        : _close_syslog_on_exit(close_syslog_on_exit)
    {}

    ~SyslogLogHandler();

    void set_close_syslog_on_exit(bool close_on_exit)
    {
        _close_syslog_on_exit = close_on_exit;
    }

    virtual void handle_log(const LogRecord &rec) override;
};
#endif

}

#endif  // head file guarder
