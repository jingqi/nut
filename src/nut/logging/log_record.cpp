
#include <assert.h>
#include <stdlib.h>

#include <nut/util/string/to_string.h>

#include "log_record.h"

namespace nut
{

LogRecord::LogRecord(LogLevel level, const char *tag, const char *file_path, int line,
	const char *func, char *message)
	: m_level(level), m_tag(tag), m_file_path(file_path), m_line(line),
	m_func(func), m_message(message)
{
	assert(NULL != file_path && line >= 0 && NULL != message);
}

LogRecord::~LogRecord()
{
	if (NULL != m_message)
		::free(m_message);
	m_message = NULL;
}

const char* LogRecord::get_file_name() const
{
	assert(NULL != m_file_path);

	const char *ret = m_file_path;
	for (const char *tmp = m_file_path; 0 != *tmp; ++tmp)
	{
		if ('\\' == *tmp || '/' == *tmp)
			ret = tmp + 1;
	}
	return ret;
}

void LogRecord::to_string(std::string *appended) const
{
    assert(NULL != appended);

    *appended += "[";
    *appended += m_time.to_string();
    *appended += "] ";
    *appended += log_level_to_str(m_level);
	if (NULL != m_tag)
	{
        *appended += " ";
        *appended += m_tag;
	}
    *appended += " (";
    *appended += get_file_name();
    *appended += ":";
    *appended += i_to_str(m_line);
    *appended += ")";
	if (NULL != m_func)
	{
		*appended += " ";
		*appended += m_func;
		*appended += "()";
	}
	*appended += " ";
    *appended += m_message;
}

}
