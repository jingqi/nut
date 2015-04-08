
#include <stdio.h>
#include <stdarg.h>

#include <nut/threading/sync/guard.hpp>

#include "logger.h"
#include "log_path.h"
#include "log_handler.h"

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif

namespace nut
{

Logger::Logger(Logger *parent, const std::string &path)
    : m_parent(parent), m_logger_path(path)
{}

void Logger::log(const std::string &log_path, const LogRecord &rec) const
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    if (!LogFilter::is_logable(log_path, rec, m_filters))
        return;

    for (std::vector<rc_ptr<LogHandler> >::const_iterator iter = m_handlers.begin(),
        end = m_handlers.end(); iter != end; ++iter)
            (*iter)->handle_log(log_path, rec, true);

    if (NULL != m_parent)
        m_parent->log(log_path, rec);
}

void Logger::add_handler(rc_ptr<LogHandler> handler)
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    m_handlers.push_back(handler);
}

void Logger::add_filter(rc_ptr<LogFilter> filter)
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    m_filters.push_back(filter);
}

void Logger::log(const LogRecord &record) const
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    log(m_logger_path, record);
}

void Logger::log(LogLevel level, const SourceLocation &sl, const std::string &msg) const
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    log(m_logger_path, LogRecord(level, sl, msg));
}

void Logger::log(LogLevel level, const SourceLocation &sl, const char *format, ...) const
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    assert(NULL != format);
    size_t size = 100;
    char *buf = (char*) ::malloc(size);
    assert(NULL != buf);

    va_list ap;
    while (NULL != buf)
    {
        va_start(ap, format);
        int n = ::vsnprintf(buf, size, format, ap);
        va_end(ap);
        if (n > -1 && n < (int)size)
            break;

        if (n > -1)
            size = n + 1; /* glibc 2.1 */
        else
            size *= 2; /* glibc 2.0 */

        if (NULL != buf)
            ::free(buf);
        buf = (char*) ::malloc(size);
        assert(NULL != buf);
    }
    std::string msg = (NULL == buf ? "" : buf);
    if (NULL != buf)
        ::free(buf);

    log(m_logger_path, LogRecord(level, sl, msg));
}

Logger* Logger::get_logger(const std::string &relative_path)
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    Guard<Mutex> g(&m_mutex);

    if (relative_path.length() == 0)
        return this;

    std::vector<rc_ptr<Logger> >::const_iterator iter = m_subloggers.begin(),
        end = m_subloggers.end();
    const std::string current = LogPath::get_first_parent(relative_path);
    while (iter != end && current != (*iter)->get_logger_name()) ++iter;
    if (iter == end)
    {
        m_subloggers.push_back(RC_NEW(NULL, Logger, this,
            (m_logger_path.length() == 0 ? current : m_logger_path + "." + current)));
        iter = m_subloggers.end() - 1;
    }
    return (*iter)->get_logger(LogPath::sub_log_path(relative_path));
}

std::string Logger::get_logger_path()
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    return m_logger_path;
}

std::string Logger::get_logger_name()
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    return LogPath::get_name(m_logger_path);
}

}


#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif
