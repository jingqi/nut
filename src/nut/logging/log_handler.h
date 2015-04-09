
#ifndef ___HEADFILE___4CDF318F_AF06_4CEF_BAC8_DE26853A73AB_
#define ___HEADFILE___4CDF318F_AF06_4CEF_BAC8_DE26853A73AB_


#include <iostream>
#include <fstream>
#include <vector>

#include <nut/platform/platform.h>

#include <nut/threading/sync/mutex.h>
#include <nut/rc/rc_new.h>

#include "log_record.h"
#include "log_filter.h"

namespace nut
{

class LogHandler
{
    NUT_REF_COUNTABLE

    Mutex m_mutex;
    std::vector<rc_ptr<LogFilter> > m_filters;

public:
    virtual ~LogHandler() {}

    virtual void handle_log(const std::string &log_path, const LogRecord &rec) = 0;

    void add_filter(rc_ptr<LogFilter> filter);

    /**
     * @return
     *      true, 通过筛选
     *      false, 在筛选过程中被剔除
     */
    void handle_log(const std::string log_path, const LogRecord& rec, bool apply_filter);
};

class StreamLogHandler : public LogHandler
{
    std::ostream &m_os;

public:
    StreamLogHandler (std::ostream &os)
        : m_os(os)
    {}

    virtual void handle_log(const std::string &log_path, const LogRecord &rec) override;
};

class ConsoleLogHandler : public LogHandler
{
    bool m_colored;

public :
    ConsoleLogHandler(bool colored = true)
        : m_colored(colored)
    {}

    virtual void handle_log(const std::string &logger_path, const LogRecord &l) override;
};

class FileLogHandler : public LogHandler
{
    std::ofstream m_ofs;

public :
    FileLogHandler(const char *file, bool append = false);

    virtual void handle_log(const std::string &log_path, const LogRecord & rec) override;
};

#if defined(NUT_PLATFORM_OS_LINUX)
class SyslogLogHandler : public LogHandler
{
    bool m_close_syslog_on_exit;

public :
    SyslogLogHandler(bool close_syslog_on_exit = false)
        : m_close_syslog_on_exit(close_syslog_on_exit)
    {}

    ~SyslogLogHandler();

    virtual void handle_log(const std::string &log_path, const LogRecord &rec) override;
};
#endif


class LogHandlerFactory
{
    LogHandlerFactory();

public:
    /**
     * the following is allowed:
     * stdout
     * stderr
     * console
     * console|nocolor
     * file|append|./logfile.log
     * file|circle|./dir/prefix
     * file|./dir/logfile.log
     * syslog
     */
    static rc_ptr<LogHandler> create_log_handler(const std::string &type);
};

}

#endif  // head file guarder
