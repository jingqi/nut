/**
 * @file LogRecord.h
 * @author jingqi
 * @date 2010-8-18
 * @brief
 * last-edit : jingqi 2011-01-21 16:57:28
 */

#ifndef ___HEADFILE___8E93C94A_595D_4161_A718_E024606E84A8_
#define ___HEADFILE___8E93C94A_595D_4161_A718_E024606E84A8_

#include <string>
#include <nut/debugging/sourcelocation.hpp>
#include <nut/util/time.hpp>

#include "loglevel.hpp"

namespace nut
{

class LogRecord
{
private :
    Time m_time;
    LogLevel m_level;
    SourceLocation m_sourceLocation;
    std::string m_message;

public :
    LogRecord(LogLevel level, const SourceLocation &sl, const std::string &message)
        : m_level(level), m_sourceLocation(sl), m_message(message)
    {}

    LogRecord(const util::Time &time, LogLevel level, const debugging::SourceLocation &sl, const std::string &message)
        : m_time(time), m_level(level), m_sourceLocation(sl), m_message(message)
    {}

    const util::Time& getTime() const { return m_time; }
    LogLevel getLevel() const { return m_level; }
    const debugging::SourceLocation& getSourceLocation() const { return m_sourceLocation; }
    const std::string& getMessage() const { return m_message; }

    std::string toString() const
    {
        std::string ret = std::string("[") + m_time.toString() + "] " + logLevelToStr(m_level) + " " +
            m_sourceLocation.toString() + " " +
            m_message;
        return ret;
    }
};

}

#endif // head file guarder


