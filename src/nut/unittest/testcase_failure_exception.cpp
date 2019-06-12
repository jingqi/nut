
#include "testcase_failure_exception.h"


namespace nut
{

TestCaseFailureException::TestCaseFailureException(std::string&& msg, const char *file, int line) noexcept
    : _message(std::forward<std::string>(msg)), _source_path(file), _source_line(line)
{}

TestCaseFailureException::TestCaseFailureException(const std::string& msg, const char *file, int line) noexcept
    : _message(msg), _source_path(file), _source_line(line)
{}

const char* TestCaseFailureException::what() const noexcept
{
    return _message.c_str();
}

const std::string& TestCaseFailureException::get_message() const noexcept
{
    return _message;
}

const char* TestCaseFailureException::get_source_file() const noexcept
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

const char* TestCaseFailureException::get_source_path() const noexcept
{
    return _source_path;
}

int TestCaseFailureException::get_source_line() const noexcept
{
    return _source_line;
}

}
