
#include <string.h> /* for strcmp() */

#include "../util/string/to_string.h"
#include "source_location.h"


namespace nut
{

SourceLocation::SourceLocation(const char *file, int line, const char *func)
    : _path(file), _line(line), _func(func)
{
    assert(nullptr != file && line >= 0);
}

bool SourceLocation::operator==(const SourceLocation& x) const
{
    if (_line != x._line)
        return false;

    if (_path != x._path)
    {
        assert(nullptr != _path && nullptr != x._path);
        if (0 != ::strcmp(_path, x._path))
            return false;
    }

    if (_func != x._func)
    {
        if (nullptr == _func || nullptr == x._func)
            return false;
        if (0 != ::strcmp(_func, x._func))
            return false;
    }

    return true;
}

bool SourceLocation::operator!=(const SourceLocation& x) const
{
    return !(*this == x);
}

const char* SourceLocation::get_file_path() const
{
    return _path;
}

const char* SourceLocation::get_file_name() const
{
    assert(nullptr != _path);

    const char *ret = _path;
    for (const char *tmp = _path; 0 != *tmp; ++tmp)
    {
        if ('\\' == *tmp || '/' == *tmp)
            ret = tmp + 1;
    }
    return ret;
}

int SourceLocation::get_line_number() const
{
    return _line;
}

const char* SourceLocation::get_function_name() const
{
    return _func;
}

std::string SourceLocation::to_string() const
{
    std::string s = get_file_name();
    s.push_back(':');
    s += nut::int_to_str(_line);
    if (_func != nullptr)
    {
        s.push_back(' ');
        s += _func;
        s += "()";
    }
    return s;
}

}
