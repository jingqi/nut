
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include <nut/platform/platform.h>
#include <nut/platform/path.h>
#include <nut/platform/os.h>
#include <nut/util/txtcfg/xml/xml_parser.h>
#include <nut/util/string/string_util.h>
#include <nut/util/string/to_string.h>

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#else
#   include <unistd.h>
#endif

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

void Logger::log(LogLevel level, const char *tag, const char *file, int line, const char *func, const char *format, ...) const
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(NULL != format);

    if (m_handlers.empty())
        return;

    if (m_filter.is_forbidden(tag, level))
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
        if (n > -1 && n < (int) size)
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
    for (size_t i = 0, sz = m_handlers.size(); i < sz; ++i)
    {
        LogHandler *handler = m_handlers.at(i);
        assert(NULL != handler);
        if (handler->get_filter().is_forbidden(tag, level))
            continue;
        handler->handle_log(record);
    }
}

void Logger::load_config(const std::string& config)
{
    class TagHandler : public XmlElementHandler
    {
        LogFilter *m_filter;
        std::string m_tag_name;
        ll_mask_t m_forbid_mask;

    public:
        TagHandler(LogFilter *filter)
            : m_filter(filter), m_forbid_mask(0)
        {
            assert(NULL != filter);
        }

        virtual void handle_attribute(const std::string& name, const std::string& value) override
        {
            if (name == "name")
            {
                m_tag_name = value;
            }
            else if (name == "forbids")
            {
                std::vector<std::string> forbids;
                chr_split(value, ',', &forbids, true);

                m_forbid_mask = 0;
                for (size_t i = 0, sz = forbids.size(); i < sz; ++i)
                    m_forbid_mask |= str_to_log_level(forbids.at(i).c_str());
            }
        }

        virtual void handle_finish() override
        {
            m_filter->forbid(m_tag_name.c_str(), m_forbid_mask);
        }
    };

    class FilterHandler : public XmlElementHandler
    {
        LogFilter *m_filter;

    public:
        FilterHandler(LogFilter *filter)
            : XmlElementHandler("filter"), m_filter(filter)
        {
            assert(NULL != filter);
        }

        virtual XmlElementHandler* handle_child(const std::string& name) override
        {
            if (name == "Tag")
            {
                TagHandler *handler = (TagHandler*) ::malloc(sizeof(TagHandler));
                new (handler) TagHandler(m_filter);
                return handler;
            }
            return NULL;
        }

        virtual void handle_child_finish(XmlElementHandler *child) override
        {
            if (NULL == child)
                return;
            TagHandler *handler = dynamic_cast<TagHandler*>(child);
            if (NULL == handler)
                return;
            handler->~TagHandler();
            ::free(handler);
        }
    };

    class HandlerHandler : public XmlElementHandler
    {
        std::string m_type;
        std::string m_file_path;
        bool m_append, m_colored, m_close_syslog_on_exit;
        int m_circle;
        ll_mask_t m_flush_mask;
        LogFilter m_filter;

    public:
        HandlerHandler()
            : XmlElementHandler("handler"), m_append(false), m_colored(true),
              m_close_syslog_on_exit(false), m_circle(-1), m_flush_mask(LL_FATAL)
        {}

        virtual XmlElementHandler* handle_child(const std::string& name) override
        {
            if (name == "Filter")
            {
                FilterHandler *handler = (FilterHandler*) ::malloc(sizeof(FilterHandler));
                new (handler) FilterHandler(&m_filter);
                return handler;
            }
            return NULL;
        }

        virtual void handle_attribute(const std::string& name, const std::string& value) override
        {
            if (name == "type")
            {
                m_type = value;
            }
            else if (name == "flushs")
            {
                std::vector<std::string> flushs;
                chr_split(value, ',', &flushs, true);

                m_flush_mask = 0;
                for (size_t i = 0, sz = flushs.size(); i < sz; ++i)
                    m_flush_mask |= str_to_log_level(flushs.at(i).c_str());
            }
            else if (name == "path")
            {
                m_file_path = value;
            }
            else if (name == "append")
            {
                m_append = (value == "true" || value == "1");
            }
            else if (name == "circle")
            {
                m_circle = ::atoi(value.c_str());
            }
            else if (name == "colored")
            {
                m_colored = (value == "true" || value == "1");
            }
            else if (name == "close_syslog_on_exit")
            {
                m_close_syslog_on_exit = (value == "true" || value == "1");
            }
        }

        virtual void handle_child_finish(XmlElementHandler *child) override
        {
            if (NULL == child)
                return;
            FilterHandler *handler = dynamic_cast<FilterHandler*>(child);
            if (NULL != handler)
            {
                handler->~FilterHandler();
                ::free(handler);
            }
        }

        virtual void handle_finish() override
        {
            if (m_type == "stdout")
            {
                rc_ptr<StreamLogHandler> handler = rc_new<StreamLogHandler>(std::cout);
                handler->set_flush_mask(m_flush_mask);
                handler->get_filter().swap(&m_filter);
                Logger::get_instance()->add_handler(handler.pointer());
            }
            else if (m_type == "stderr")
            {
                rc_ptr<StreamLogHandler> handler = rc_new<StreamLogHandler>(std::cerr);
                handler->set_flush_mask(m_flush_mask);
                handler->get_filter().swap(&m_filter);
                Logger::get_instance()->add_handler(handler.pointer());
            }
            else if (m_type == "console")
            {
                rc_ptr<ConsoleLogHandler> handler = rc_new<ConsoleLogHandler>();
                handler->set_flush_mask(m_flush_mask);
                handler->get_filter().swap(&m_filter);
                handler->set_colored(m_colored);
                Logger::get_instance()->add_handler(handler.pointer());
            }
            else if (m_type == "file")
            {
                if (m_file_path.empty())
                    return;

                if (m_circle >= 0)
                {
                    // 找到相同目录下所有到日志文件
                    std::string dir_path;
                    Path::split(m_file_path, &dir_path, NULL);
                    std::vector<std::string> files;
                    OS::listdir(dir_path.c_str(), &files, false, true, true);
                    const std::string log_ext(".log");
                    for (size_t i = 0; i < files.size(); ++i)
                    {
                        if (!ends_with(files.at(i), log_ext))
                        {
                            files.erase(files.begin() + i);
                            --i;
                        }
                    }

                    // 删除多余的日志文件
                    if (files.size() > (size_t) m_circle)
                    {
                        std::sort(files.begin(), files.end());
                        for (size_t i = 0, count = files.size() - m_circle; i < count; ++i)
                        {
                            OS::removefile(Path::join(dir_path, files.at(i)).c_str());
                            files.pop_back();
                        }
                    }

                    // 给文件名添加附加标志符
                    m_file_path += Time().format_time("%Y-%m-%d %H-%M-%S ");
        #if defined(NUT_PLATFORM_OS_WINDOWS)
                    long pid = ::GetCurrentProcessId();
        #else
                    pid_t pid = ::getpid();
        #endif
                    m_file_path += ll_to_str(pid);
                    m_file_path += log_ext;
                }

                rc_ptr<FileLogHandler> handler = rc_new<FileLogHandler>(m_file_path.c_str(), m_append);
                handler->set_flush_mask(m_flush_mask);
                handler->get_filter().swap(&m_filter);
                Logger::get_instance()->add_handler(handler.pointer());
            }
#if defined(NUT_PLATFORM_OS_LINUX)
            else if (m_type == "syslog")
            {
                rc_ptr<SyslogLogHandler> handler = rc_new<SyslogLogHandler>(m_close_syslog_on_exit);
                handler->set_flush_mask(m_flush_mask);
                handler->get_filter().swap(&m_filter);
                Logger::get_instance()->add_handler(handler.pointer());
            }
#endif
        }
    };

    class LoggerHandler : public XmlElementHandler
    {
        virtual XmlElementHandler* handle_child(const std::string& name) override
        {
            if (name == "Filter")
            {
                FilterHandler *handler = (FilterHandler*) ::malloc(sizeof(FilterHandler));
                new (handler) FilterHandler(&Logger::get_instance()->get_filter());
                return handler;
            }
            else if (name == "Handler")
            {
                HandlerHandler *handler = (HandlerHandler*) ::malloc(sizeof(HandlerHandler));
                new (handler) HandlerHandler;
                return handler;
            }
            return NULL;
        }

        virtual void handle_child_finish(XmlElementHandler *child) override
        {
            if (NULL == child)
                return;
            if (0 == ::strcmp(child->name, "filter"))
            {
                FilterHandler *handler = dynamic_cast<FilterHandler*>(child);
                if (NULL != handler)
                {
                    handler->~FilterHandler();
                    ::free(handler);
                }
            }
            else
            {
                assert(0 == ::strcmp(child->name, "handler"));
                HandlerHandler *handler = dynamic_cast<HandlerHandler*>(child);
                if (NULL != handler)
                {
                    handler->~HandlerHandler();
                    ::free(handler);
                }
            }
        }
    };

    class RootHandler : public XmlElementHandler
    {
        virtual XmlElementHandler* handle_child(const std::string& name)
        {
            if (name == "Logger")
            {
                LoggerHandler *handler = (LoggerHandler*) ::malloc(sizeof(LoggerHandler));
                new (handler) LoggerHandler;
                return handler;
            }
            return NULL;
        }

        virtual void handle_child_finish(XmlElementHandler *child)
        {
            if (NULL == child)
                return;
            LoggerHandler *handler = dynamic_cast<LoggerHandler*>(child);
            if (NULL != handler)
            {
                handler->~LoggerHandler();
                ::free(handler);
            }
        }
    };

    m_filter.clear_forbids();
    clear_handlers();

    RootHandler root;
    XmlParser parser(&root);
    parser.input(config.c_str());
    parser.finish();
}

}
