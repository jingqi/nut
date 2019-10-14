
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include "../platform/platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#else
#   include <unistd.h>
#endif

#include "../platform/path.h"
#include "../platform/os.h"
#include "../rc/rc_new.h"
#include "../util/txtcfg/xml/xml_parser.h"
#include "../util/string/string_utils.h"
#include "../util/string/to_string.h"
#include "logger.h"
#include "log_handler/stream_log_handler.h"
#include "log_handler/console_log_handler.h"
#include "log_handler/syslog_log_handler.h"
#include "log_handler/file_log_handler.h"
#include "log_handler/circle_files_by_size_log_handler.h"
#include "log_handler/circle_files_by_time_log_handler.h"


namespace nut
{

Logger::~Logger() noexcept
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    clear_handlers();
}

Logger* Logger::get_instance() noexcept
{
    static Logger instance;
    return &instance;
}

LogFilter& Logger::get_filter() noexcept
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    return _filter;
}

void Logger::add_handler(LogHandler *handler) noexcept
{
    assert(nullptr != handler);
    NUT_DEBUGGING_ASSERT_ALIVE;

    _handlers.push_back(handler);
}

void Logger::remove_handler(LogHandler *handler) noexcept
{
    assert(nullptr != handler);
    NUT_DEBUGGING_ASSERT_ALIVE;

    for (size_t i = 0, sz = _handlers.size(); i < sz; ++i)
    {
        if (_handlers.at(i).pointer() == handler)
        {
            _handlers.erase(_handlers.begin() + i);
            return;
        }
    }
}

void Logger::clear_handlers() noexcept
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    _handlers.clear();
}

void Logger::log(enum LogLevel level, const char *tag, const char *file, int line,
                 const char *func, const char *format, ...) const noexcept
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(nullptr != format);

    if (_handlers.empty())
        return;

    if (!_filter.is_allowed(tag, level))
        return;

    LogRecord record(level, tag, file, line, func);
    for (size_t i = 0, sz = _handlers.size(); i < sz; ++i)
    {
        LogHandler *handler = _handlers.at(i);
        assert(nullptr != handler);
        if (!handler->get_filter().is_allowed(tag, level))
            continue;

        // delay init log record
        if (nullptr == record.get_message())
        {
            // format log message
            size_t size = ::strlen(format) * 3 / 2 + 8;
            char *buf = (char*) ::malloc(size);
            va_list ap;
            while (nullptr != buf)
            {
                va_start(ap, format);
                int n = ::vsnprintf(buf, size, format, ap);
                va_end(ap);
                if (0 <= n && n < (int) size)
                    break;

                if (n < 0)
                    size *= 2; /* glibc 2.0 */
                else
                    size = n + 1; /* glibc 2.1 */
                buf = (char*) ::realloc(buf, size);
            }
            if (nullptr == buf)
                return; // some error happend

            // delay init log record
            record.delay_init(buf); // NOTE 'buf' will be freed by LogRecord
        }

        handler->handle_log(record);
    }
}

void Logger::load_xml_config(const std::string& config) noexcept
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    class TagHandler : public XmlElementHandler
    {
    public:
        TagHandler()
            : XmlElementHandler(HANDLE_ATTRIBUTE)
        {}

        void reset(LogFilter *filter)
        {
            assert(nullptr != filter);
            _filter = filter;
            _tag_name.clear();
            _allowed_levels = 0;
            _forbidden_levels = 0;
        }

        virtual void handle_attribute(const std::string& name, const std::string& value) noexcept override
        {
            if (name == "name")
            {
                _tag_name = value;
            }
            else if (name == "allow")
            {
                std::vector<std::string> allow = chr_split(value.c_str(), ',', true);

                _allowed_levels = 0;
                for (size_t i = 0, sz = allow.size(); i < sz; ++i)
                    _allowed_levels |= str_to_log_level(allow.at(i).c_str());
                _forbidden_levels &= ~_allowed_levels;
            }
            else if (name == "forbid")
            {
                std::vector<std::string> forbid = chr_split(value.c_str(), ',', true);

                _forbidden_levels = 0;
                for (size_t i = 0, sz = forbid.size(); i < sz; ++i)
                    _forbidden_levels |= str_to_log_level(forbid.at(i).c_str());
                _allowed_levels &= ~_forbidden_levels;
            }
        }

        virtual void handle_finish() noexcept override
        {
            if (0 != _allowed_levels)
                _filter->allow(_tag_name.c_str(), _allowed_levels);
            if (0 != _forbidden_levels)
                _filter->forbid(_tag_name.c_str(), _forbidden_levels);
        }

    private:
        LogFilter *_filter = nullptr;
        std::string _tag_name;
        loglevel_mask_type _forbidden_levels = 0;
        loglevel_mask_type _allowed_levels = 0;
    } tag_xml_handler;

    class FilterHandler : public XmlElementHandler
    {
    public:
        FilterHandler(TagHandler *tag_xml_handler)
            : XmlElementHandler(HANDLE_CHILD), _tag_xml_handler(tag_xml_handler)
        {
            assert(nullptr != tag_xml_handler);
        }

        void reset(LogFilter *filter)
        {
            assert(nullptr != filter);
            _filter = filter;
        }

        virtual XmlElementHandler* handle_child(const std::string& name) noexcept override
        {
            if (name == "Tag")
            {
                _tag_xml_handler->reset(_filter);
                return _tag_xml_handler;
            }
            return nullptr;
        }

    private:
        TagHandler *_tag_xml_handler = nullptr;
        LogFilter *_filter = nullptr;
    } filter_xml_handler(&tag_xml_handler);

    class HandlerHandler : public XmlElementHandler
    {
    public:
        HandlerHandler(FilterHandler *filter_xml_handler)
            : XmlElementHandler(HANDLE_ATTRIBUTE | HANDLE_CHILD),
              _filter_xml_handler(filter_xml_handler)
        {
            assert(nullptr != filter_xml_handler);
        }

        void reset()
        {
            _type.clear();
            _path.clear();
            _file_prefix.clear();
            _time_format = "%Y%m%d-%H%M%S.%3f";
            _trunc = false;
            _cross_file = false;
            _circle = 10;
            _max_file_size = 1 * 1024 * 1024;
            _flush_mask = LL_FATAL;
            _filter.reset();
        }

        virtual XmlElementHandler* handle_child(const std::string& name) noexcept override
        {
            if (name == "Filter")
            {
                _filter_xml_handler->reset(&_filter);
                return _filter_xml_handler;
            }
            return nullptr;
        }

        virtual void handle_attribute(const std::string& name, const std::string& value) noexcept override
        {
            if (name == "type")
            {
                _type = value;
            }
            else if (name == "flush")
            {
                std::vector<std::string> flush = chr_split(value.c_str(), ',', true);

                _flush_mask = 0;
                for (size_t i = 0, sz = flush.size(); i < sz; ++i)
                    _flush_mask |= str_to_log_level(flush.at(i).c_str());
            }
            else if (name == "path")
            {
                _path = value;
            }
            else if (name == "trunc")
            {
                _trunc = (value == "true" || value == "1");
            }
            else if (name == "circle")
            {
                _circle = str_to_long(value);
            }
            else if (name == "file_prefix")
            {
                _file_prefix = value;
            }
            else if (name == "time_format")
            {
                _time_format = value;
            }
            else if (name == "max_file_size")
            {
                _max_file_size = str_to_long(value);
            }
            else if (name == "cross_file")
            {
                _cross_file = (value == "true" || value == "1");
            }
        }

        virtual void handle_finish() noexcept override
        {
            if (_type == "stdout")
            {
                rc_ptr<StreamLogHandler> handler = rc_new<StreamLogHandler>(std::cout);
                handler->set_flush_mask(_flush_mask);
                handler->get_filter().swap(&_filter);
                Logger::get_instance()->add_handler(handler);
            }
            else if (_type == "stderr")
            {
                rc_ptr<StreamLogHandler> handler = rc_new<StreamLogHandler>(std::cerr);
                handler->set_flush_mask(_flush_mask);
                handler->get_filter().swap(&_filter);
                Logger::get_instance()->add_handler(handler);
            }
            else if (_type == "console")
            {
                rc_ptr<ConsoleLogHandler> handler = rc_new<ConsoleLogHandler>();
                handler->set_flush_mask(_flush_mask);
                handler->get_filter().swap(&_filter);
                Logger::get_instance()->add_handler(handler);
            }
            else if (_type == "file")
            {
                if (_path.empty())
                    return;
                rc_ptr<FileLogHandler> handler = rc_new<FileLogHandler>(_path.c_str(), _trunc);
                handler->set_flush_mask(_flush_mask);
                handler->get_filter().swap(&_filter);
                Logger::get_instance()->add_handler(handler);
            }
            else if (_type == "cicle_file_by_size")
            {
                if (_path.empty())
                    return;
                rc_ptr<CircleFilesBySizeLogHandler> handler = rc_new<CircleFilesBySizeLogHandler>(
                    _path, _file_prefix, _circle, _max_file_size, _cross_file);
                handler->set_flush_mask(_flush_mask);
                handler->get_filter().swap(&_filter);
                Logger::get_instance()->add_handler(handler);
            }
            else if (_type == "file_cicle_by_time")
            {
                if (_path.empty())
                    return;
                rc_ptr<CircleFilesByTimeLogHandler> handler = rc_new<CircleFilesByTimeLogHandler>(
                    _path, _file_prefix, _time_format.c_str(), _circle);
                handler->set_flush_mask(_flush_mask);
                handler->get_filter().swap(&_filter);
                Logger::get_instance()->add_handler(handler);
            }
#if NUT_PLATFORM_OS_MACOS || NUT_PLATFORM_OS_LINUX
            else if (_type == "syslog")
            {
                rc_ptr<SyslogLogHandler> handler = rc_new<SyslogLogHandler>();
                handler->set_flush_mask(_flush_mask);
                handler->get_filter().swap(&_filter);
                Logger::get_instance()->add_handler(handler);
            }
#endif
        }

    private:
        FilterHandler *_filter_xml_handler = nullptr;

        std::string _type;
        std::string _path;
        std::string _file_prefix;
        std::string _time_format;
        bool _trunc = false;
        bool _cross_file = true;
        size_t _circle = 10;
        long _max_file_size = 1 * 1024 * 1024;
        loglevel_mask_type _flush_mask = LL_FATAL;
        LogFilter _filter;
    } handler_xml_handler(&filter_xml_handler);

    class LoggerHandler : public XmlElementHandler
    {
    public:
        LoggerHandler(FilterHandler *filter_xml_handler, HandlerHandler *handler_xml_handler)
            : XmlElementHandler(HANDLE_CHILD), _filter_xml_handler(filter_xml_handler),
              _handler_xml_handler(handler_xml_handler)
        {
            assert(nullptr != filter_xml_handler && nullptr != handler_xml_handler);
        }

        virtual XmlElementHandler* handle_child(const std::string& name) noexcept override
        {
            if (name == "Filter")
            {
                _filter_xml_handler->reset(&Logger::get_instance()->get_filter());
                return _filter_xml_handler;
            }
            else if (name == "Handler")
            {
                _handler_xml_handler->reset();
                return _handler_xml_handler;
            }
            return nullptr;
        }

    private:
        FilterHandler *_filter_xml_handler = nullptr;
        HandlerHandler *_handler_xml_handler = nullptr;
    } logger_xml_handler(&filter_xml_handler, &handler_xml_handler);

    class RootHandler : public XmlElementHandler
    {
    public:
        RootHandler(LoggerHandler *logger_xml_handler)
            : XmlElementHandler(HANDLE_CHILD), _logger_xml_handler(logger_xml_handler)
        {
            assert(nullptr != logger_xml_handler);
        }

        virtual XmlElementHandler* handle_child(const std::string& name) noexcept
        {
            if (name == "Logger")
                return _logger_xml_handler;
            return nullptr;
        }

    private:
        LoggerHandler *_logger_xml_handler = nullptr;
    } root_handler(&logger_xml_handler);

    _filter.reset();
    clear_handlers();

    XmlParser parser(&root_handler);
    parser.input(config.c_str());
    parser.finish();
}

}
