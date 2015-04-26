
#ifndef ___HEADFILE___8E93C94A_595D_4161_A718_E024606E84A8_
#define ___HEADFILE___8E93C94A_595D_4161_A718_E024606E84A8_

#include <string>

#include <nut/util/time/date_time.h>

#include "log_level.h"

namespace nut
{

class LogRecord
{
private:
    Time m_time;
    LogLevel m_level;
	const char *m_tag; 			// can be NULL, which indicated a root tag
    const char *m_file_path;
    int m_line;
	const char *m_func;			// can be NULL, when the source location is out of any function
	char *m_message;

private:
	LogRecord(const LogRecord&);

public:
    LogRecord(LogLevel level, const char *tag, const char *file_path, int line,
		const char *func, char *message);
	~LogRecord();

    const Time& get_time() const
    {
        return m_time;
    }

    LogLevel get_level() const
    {
        return m_level;
    }
	
	const char* get_tag() const
	{
		return m_tag;
	}

    const char* get_file_path() const
    {
        return m_file_path;
    }
	
	const char* get_file_name() const;

    int get_line() const
    {
        return m_line;
    }

    const char* get_message() const
    {
        return m_message;
    }

    void to_string(std::string *appended) const;
};

}

#endif
