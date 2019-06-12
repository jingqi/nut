
#include <assert.h>
#include <stdlib.h>

#include "../util/string/to_string.h"
#include "log_record.h"


namespace nut
{

LogRecord::LogRecord(enum LogLevel level, const char *tag, const char *file_path,
                     int line, const char *func) noexcept
    : _level(level), _tag(tag), _file_path(file_path), _line(line), _func(func)
{
    assert(nullptr != file_path && line >= 0);
}

LogRecord::~LogRecord() noexcept
{
    if (nullptr != _message)
        ::free(_message);
    _message = nullptr;
}

void LogRecord::delay_init(char *message) noexcept
{
    assert(nullptr != message);
    _message = message;
    _time.set_to_now();
}

const DateTime& LogRecord::get_time() const noexcept
{
    return _time;
}

enum LogLevel LogRecord::get_level() const noexcept
{
    return _level;
}

const char* LogRecord::get_tag() const noexcept
{
    return _tag;
}

const char* LogRecord::get_file_path() const noexcept
{
    return _file_path;
}

const char* LogRecord::get_file_name() const noexcept
{
    assert(nullptr != _file_path);

    const char *ret = _file_path;
    for (const char *tmp = _file_path; 0 != *tmp; ++tmp)
    {
        if ('\\' == *tmp || '/' == *tmp)
            ret = tmp + 1;
    }
    return ret;
}

int LogRecord::get_line() const noexcept
{
    return _line;
}

const char* LogRecord::get_message() const noexcept
{
    return _message;
}

std::string LogRecord::to_string() const noexcept
{
    std::string s = "[";
    s += _time.to_string();
    s += "] ";
    s += log_level_to_str(_level);
    if (nullptr != _tag)
    {
        s.push_back(' ');
        s += _tag;
    }
    s += " (";
    s += get_file_name();
    s.push_back(':');
    s += int_to_str(_line);
    s.push_back(')');
    if (nullptr != _func)
    {
        s.push_back(' ');
        s += _func;
        s += "()";
    }
    s.push_back(' ');
    s += _message;
    return s;
}

}
