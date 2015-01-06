/**
 * @file LogRecord.h
 * @author jingqi
 * @date 2010-8-18
 * @brief
 * last-edit : 2015-01-06 22:04:06 jingqi
 */

#ifndef ___HEADFILE___8E93C94A_595D_4161_A718_E024606E84A8_
#define ___HEADFILE___8E93C94A_595D_4161_A718_E024606E84A8_

#include <string>
#include <nut/debugging/source_location.hpp>
#include <nut/util/time.hpp>

#include "log_level.hpp"

namespace nut
{

class LogRecord
{
private :
    Time m_time;
    LogLevel m_level;
    SourceLocation m_source_location;
    std::string m_message;

public :
    LogRecord(LogLevel level, const SourceLocation &sl, const std::string &message)
        : m_level(level), m_source_location(sl), m_message(message)
    {}

    LogRecord(const Time &time, LogLevel level, const SourceLocation &sl, const std::string &message)
        : m_time(time), m_level(level), m_source_location(sl), m_message(message)
    {}

    const Time& get_time() const
    {
        return m_time;
    }

    LogLevel get_level() const
    {
        return m_level;
    }

    const SourceLocation& get_source_location() const
    {
        return m_source_location;
    }

    const std::string& get_message() const
    {
        return m_message;
    }

    std::string to_string() const
    {
        std::string ret = std::string("[") + m_time.to_string() + "] " + log_level_to_str(m_level) + " " +
            m_source_location.to_string() + " " + m_message;
        return ret;
    }
};

}

#endif // head file guarder
