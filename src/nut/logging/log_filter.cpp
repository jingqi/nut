
#include <nut/rc/rc_new.h>

#include "log_filter.h"

namespace nut
{

bool LogFilter::is_logable(const std::string& log_path, const LogRecord& rec,
    const std::vector<rc_ptr<LogFilter> >& filters)
{
    for (size_t i = 0, sz = filters.size(); i < sz; ++i)
    {
        if (!filters.at(i)->is_logable(log_path, rec))
            return false;
    }
    return true;
}

DefaultLogFilter::DefaultLogFilter(LogLevel min_level, const std::vector<std::string> &deny_paths)
    : m_deny_paths(deny_paths)
{
    for (int i = 0; i < COUNT_OF_LOG_LEVEL; ++i)
        m_level_mask[i] = (i < min_level);
}

DefaultLogFilter::DefaultLogFilter(bool allow_debug, bool allow_info, bool allow_warn,
    bool allow_error, bool allow_fatal, const std::vector<std::string> &excepts)
    : m_deny_paths(excepts)
{
    m_level_mask[LL_DEBUG] = allow_debug;
    m_level_mask[LL_INFO] = allow_info;
    m_level_mask[LL_WARN] = allow_warn;
    m_level_mask[LL_ERROR] = allow_error;
    m_level_mask[LL_FATAL] = allow_fatal;
}

bool DefaultLogFilter::is_logable(const std::string &log_path, const LogRecord &rec) const
{
    if (!m_level_mask[rec.get_level()])
        return false;

    for (size_t i = 0, sz = m_deny_paths.size(); i < sz; ++i)
    {
        const std::string& deny_path = m_deny_paths.at(i);
        if (deny_path.length() <= log_path.length() &&
            deny_path == log_path.substr(0, deny_path.length()))
            return false;
    }
    return true;
}

void DefaultLogFilter::add_deny_path(const std::string &path)
{
    m_deny_paths.push_back(path);
}

rc_ptr<LogFilter> LogFilterFactory::create_log_filter(const std::string &arg)
{
    bool mask[5] = { true, true, true, true, true };
    for (size_t i = 0, sz = arg.length(); i < sz && i < 5; ++i)
    {
        if (arg[i] == '0')
            mask[i] = false;
    }
    std::vector<std::string> deny_paths;
    if (arg.length() > 6 && arg[5] == '|')
    {
        std::string::size_type begin = 6, end = arg.find_first_of(':',6);
        while (end != std::string::npos)
        {
            if (begin != end)
                deny_paths.push_back(arg.substr(begin, end - begin));
            begin = end + 1;
            end = arg.find_first_of(':',begin);
        }
        if (begin != arg.length())
            deny_paths.push_back(arg.substr(begin));
    }

    return rc_new<DefaultLogFilter>(mask[0], mask[1], mask[2], mask[3], mask[4], deny_paths);
}

}
