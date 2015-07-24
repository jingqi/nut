
#include <string.h> /* for strcmp() */

#include <nut/util/string/to_string.h>

#include "source_location.h"

namespace nut
{

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

void SourceLocation::to_string(std::string *appended) const
{
	assert(NULL != appended);

	*appended += get_file_name();
	appended->push_back(':');
	nut::i_to_str(_line, appended);
	if (_func != NULL)
	{
		appended->push_back(' ');
		*appended += _func;
		*appended += "()";
	}
}

}
