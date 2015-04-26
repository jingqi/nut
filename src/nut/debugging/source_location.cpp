
#include <string.h> /* for strcmp() */

#include <nut/util/string/to_string.h>

#include "source_location.h"

namespace nut
{

bool SourceLocation::operator==(const SourceLocation& x) const
{
	if (m_line != x.m_line)
		return false;

	if (m_path != x.m_path)
	{
		assert(NULL != m_path && NULL != x.m_path);
		if (0 != ::strcmp(m_path, x.m_path))
			return false;
	}
	
	if (m_func != x.m_func)
	{
		if (NULL == m_func || NULL == x.m_func)
			return false;
		if (0 != ::strcmp(m_func, x.m_func))
			return false;
	}

	return true;
}

const char* SourceLocation::get_file_name() const
{
	assert(NULL != m_path);

	const char *ret = m_path;
	for (const char *tmp = m_path; 0 != *tmp; ++tmp)
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
	nut::i_to_str(m_line, appended);
	if (m_func != NULL)
	{
		appended->push_back(' ');
		*appended += m_func;
		*appended += "()";
	}
}

}
