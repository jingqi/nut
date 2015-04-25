
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#include "logger.h"

namespace nut
{

Logger::~Logger()
{
	NUT_DEBUGGING_ASSERT_ALIVE;

	clear_handlers();
}

Logger* Logger::get_instance()
{
	// 跨动态链接库的单例实现
	return (Logger*) nut_get_logger();
}

LogFilter& Logger::get_filter()
{
	NUT_DEBUGGING_ASSERT_ALIVE;

	return m_filter;
}

void Logger::add_handler(LogHandler *handler)
{
	assert(NULL != handler);
	NUT_DEBUGGING_ASSERT_ALIVE;

	handler->add_ref();
	m_handlers.push_back(handler);
}

void Logger::remove_handler(LogHandler *handler)
{
	assert(NULL != handler);
	NUT_DEBUGGING_ASSERT_ALIVE;

	for (size_t i = 0, sz = m_handlers.size(); i < sz; ++i)
	{
		if (m_handlers.at(i) == handler)
		{
			m_handlers.erase(m_handlers.begin() + i);
			handler->release_ref();
			return;
		}
	}
}

void Logger::clear_handlers()
{
	NUT_DEBUGGING_ASSERT_ALIVE;

	for (size_t i = 0, sz = m_handlers.size(); i < sz; ++i)
	{
		LogHandler *handler = m_handlers.at(i);
		assert(NULL != handler);
		handler->release_ref();
	}
	m_handlers.clear();
}

void Logger::log(const LogRecord &rec) const
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    if (m_handlers.empty())
        return;

    const char *tag = rec.get_tag();
    LogLevel level = rec.get_level();
    if (m_filter.is_forbidden(tag, level))
        return;

    for (size_t i = 0, sz = m_handlers.size(); i < sz; ++i)
    {
        LogHandler *handler = m_handlers.at(i);
        assert(NULL != handler);
        if (handler->get_filter().is_forbidden(tag, level))
            continue;
        handler->handle_log(rec);
    }
}

void Logger::log(LogLevel level, const char *tag, const char *file, int line, const char *func, const char *format, ...) const
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(NULL != format);

    if (m_handlers.empty())
        return;

    size_t size = 32;
    char *buf = (char*) ::malloc(size);
    assert(NULL != buf);

    va_list ap;
    while (NULL != buf)
    {
        va_start(ap, format);
        int n = ::vsnprintf(buf, size, format, ap);
        va_end(ap);
        if (n > -1 && n < (int)size)
            break;

        if (n > -1)
            size = n + 1; /* glibc 2.1 */
        else
            size *= 2; /* glibc 2.0 */

        buf = (char*) ::realloc(buf, size);
    }
	if (NULL == buf)
		return;

	LogRecord record(level, tag, file, line, func, buf); /* buf will be freed by LogRecord */
    log(record);
}

}
