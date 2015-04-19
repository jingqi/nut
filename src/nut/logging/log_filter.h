
#ifndef ___HEADFILE___38990555_F0A8_4BA9_BE50_9A477A2A3F47_
#define ___HEADFILE___38990555_F0A8_4BA9_BE50_9A477A2A3F47_

#include <string>
#include <vector>

#include <nut/rc/rc_new.h>

#include "log_level.h"
#include "log_record.h"

namespace nut
{

class LogFilter
{
    NUT_REF_COUNTABLE

public:
    virtual bool is_logable(const std::string &logger_path, const LogRecord &log) const = 0;

public:
    static bool is_logable(const std::string& log_path, const LogRecord& rec,
        const std::vector<rc_ptr<LogFilter> >& filters);
};

class DefaultLogFilter : public LogFilter
{
    bool m_level_mask[COUNT_OF_LOG_LEVEL];
    std::vector<std::string> m_deny_paths;

public:
    DefaultLogFilter (LogLevel min_level, const std::vector<std::string> &deny_paths =
        std::vector<std::string>());

    DefaultLogFilter(bool allow_debug = true, bool allow_info = true, bool allow_warn = true,
        bool allow_error = true, bool allow_fatal = true,
        const std::vector<std::string> &excepts = std::vector<std::string>());

    virtual bool is_logable(const std::string &log_path, const LogRecord &rec) const override;

    void add_deny_path(const std::string &path);
};

class LogFilterFactory
{
    LogFilterFactory();

public:
    static rc_ptr<LogFilter> create_log_filter(const std::string &arg);
};

}


#endif // head file guarder
