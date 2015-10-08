
#include <assert.h>
#include <stdlib.h>

#include <nut/util/string/to_string.h>

#include "log_record.h"

namespace nut
{

LogRecord::LogRecord(LogLevel level, const char *tag, const char *file_path, int line,
	const char *func, char *message)
    : _level(level), _tag(tag), _file_path(file_path), _line(line),
    _func(func), _message(message)
{
	assert(NULL != file_path && line >= 0 && NULL != message);
}

LogRecord::~LogRecord()
{
    if (NULL != _message)
        ::free(_message);
    _message = NULL;
}

const DateTime& LogRecord::get_time() const
{
    return _time;
}

LogLevel LogRecord::get_level() const
{
    return _level;
}

const char* LogRecord::get_tag() const
{
    return _tag;
}

const char* LogRecord::get_file_path() const
{
    return _file_path;
}

const char* LogRecord::get_file_name() const
{
    assert(NULL != _file_path);

    const char *ret = _file_path;
    for (const char *tmp = _file_path; 0 != *tmp; ++tmp)
	{
		if ('\\' == *tmp || '/' == *tmp)
			ret = tmp + 1;
	}
	return ret;
}

int LogRecord::get_line() const
{
    return _line;
}

const char* LogRecord::get_message() const
{
    return _message;
}

void LogRecord::to_string(std::string *appended) const
{
    assert(NULL != appended);

    *appended += "[";
    *appended += _time.to_string();
    *appended += "] ";
    *appended += log_level_to_str(_level);
    if (NULL != _tag)
	{
        *appended += " ";
        *appended += _tag;
	}
    *appended += " (";
    *appended += get_file_name();
    *appended += ":";
    *appended += int_to_str(_line);
    *appended += ")";
    if (NULL != _func)
	{
		*appended += " ";
        *appended += _func;
		*appended += "()";
	}
	*appended += " ";
    *appended += _message;
}

}
