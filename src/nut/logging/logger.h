
#ifndef ___HEADFILE___C2472118_B9F0_49CE_BE12_59F3AE4991CA_
#define ___HEADFILE___C2472118_B9F0_49CE_BE12_59F3AE4991CA_

#include <string>
#include <vector>

#include <nut/rc/rc_new.h>
#include <nut/threading/sync/mutex.h>
#include <nut/debugging/destroy_checker.h>

#include "log_path.h"
#include "log_handler.h"

namespace nut
{

class Logger
{
    NUT_REF_COUNTABLE
    NUT_PRIVATE_RCNEW

    std::vector<rc_ptr<LogHandler> > m_handlers;
    std::vector<rc_ptr<Logger> > m_subloggers;
    std::vector<rc_ptr<LogFilter> > m_filters;
    Logger *m_parent;
    std::string m_logger_path;
    Mutex m_mutex;

    /** 析构检查器 */
    NUT_DEBUGGING_DESTROY_CHECKER

private:
    Logger(const Logger&);
    Logger& operator=(const Logger&);

private:
    Logger(Logger *parent, const std::string &path);

    void log(const std::string &log_path, const LogRecord &rec) const;

public :
    void add_handler(rc_ptr<LogHandler> handler);

    void add_filter(rc_ptr<LogFilter> filter);

    void log(const LogRecord &record) const;
    void log(LogLevel level, const SourceLocation &sl, const std::string &msg) const;
    void log(LogLevel level, const SourceLocation &sl, const char *format, ...) const;

    Logger* get_logger(const std::string &relative_path);

    std::string get_logger_path();

    std::string get_logger_name();
};

}

#endif // head file guarder
