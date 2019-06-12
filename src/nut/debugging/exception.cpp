
#include "exception.h"


namespace nut
{

Exception::Exception(long code, std::string&& msg, const char *file,
                     int line, const char *func) noexcept
    : _code(code), _message(std::forward<std::string>(msg)), _source_path(file),
      _source_line(line), _function(func)
{}

Exception::Exception(long code, const std::string& msg, const char *file,
                     int line, const char *func) noexcept
    : _code(code), _message(msg), _source_path(file), _source_line(line),
      _function(func)
{}

const char* Exception::what() const noexcept
{
    return _message.c_str();
}

long Exception::get_code() const noexcept
{
    return _code;
}

const std::string& Exception::get_message() const noexcept
{
    return _message;
}

const char* Exception::get_source_file() const noexcept
{
    if (nullptr == _source_path)
        return nullptr;

    const char *ret = _source_path;
    for (const char *i = _source_path; '\0' != *i; ++i)
    {
        if ('\\' == *i || '/' == *i)
            ret = i + 1;
    }
    return ret;
}

const char* Exception::get_source_path() const noexcept
{
    return _source_path;
}

int Exception::get_source_line() const noexcept
{
    return _source_line;
}

const char* Exception::get_source_func() const noexcept
{
    return _function;
}

}
