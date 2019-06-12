
#include <string.h> /* for strcmp() */

#include "../util/string/to_string.h"
#include "source_location.h"


namespace nut
{

bool SourceLocation::operator==(const SourceLocation& x) const noexcept
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

bool SourceLocation::operator!=(const SourceLocation& x) const noexcept
{
    return !(*this == x);
}

const char* SourceLocation::get_file_name() const noexcept
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

std::string SourceLocation::to_string() const noexcept
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
