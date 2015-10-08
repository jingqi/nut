
#include "testcase_failure_exception.h"

namespace nut
{

TestCaseFailureException::TestCaseFailureException(const char *des, const char *file, int line)
    : _description(des), _file(file), _line(line)
{}

const char* TestCaseFailureException::get_description() const
{
    return _description;
}

const char* TestCaseFailureException::get_file() const
{
    return _file;
}

int TestCaseFailureException::get_line() const
{
    return _line;
}

}
