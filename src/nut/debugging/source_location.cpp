
#include <string.h> /* for strcmp() */

#include <nut/util/string/to_string.h>

#include "source_location.h"

namespace nut
{

SourceLocation::SourceLocation(const char *file, int line, const char *func)
    : _path(file), _line(line), _func(func)
{
    assert(NULL != file && line >= 0);
}

bool SourceLocation::operator==(const SourceLocation& x) const
{
	if (_line != x._line)
		return false;

	if (_path != x._path)
	{
		assert(NULL != _path && NULL != x._path);
		if (0 != ::strcmp(_path, x._path))
			return false;
	}

	if (_func != x._func)
	{
		if (NULL == _func || NULL == x._func)
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
	assert(NULL != _path);

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

void SourceLocation::to_string(std::string *appended) const
{
	assert(NULL != appended);

	*appended += get_file_name();
	appended->push_back(':');
    nut::int_to_str(_line, appended);
	if (_func != NULL)
	{
		appended->push_back(' ');
		*appended += _func;
		*appended += "()";
	}
}

}
