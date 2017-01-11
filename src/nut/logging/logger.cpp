
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

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#else
#   include <unistd.h>
#endif

#include "logger.h"
#include "log_handler/stream_log_handler.h"
#include "log_handler/console_log_handler.h"
#include "log_handler/syslog_log_handler.h"
#include "log_handler/file_log_handler.h"
#include "log_handler/circle_file_by_size_log_handler.h"
#include "log_handler/circle_file_by_time_log_handler.h"

namespace nut
{

Logger::~Logger()
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    clear_handlers();
}

Logger* Logger::get_instance()
{
    static Logger instance;
    return &instance;
}

LogFilter& Logger::get_filter()
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    return _filter;
}

void Logger::add_handler(LogHandler *handler)
{
    assert(nullptr != handler);
    NUT_DEBUGGING_ASSERT_ALIVE;

    handler->add_ref();
    _handlers.push_back(handler);
}

void Logger::remove_handler(LogHandler *handler)
{
    assert(nullptr != handler);
    NUT_DEBUGGING_ASSERT_ALIVE;

    for (size_t i = 0, sz = _handlers.size(); i < sz; ++i)
    {
        if (_handlers.at(i) == handler)
        {
            _handlers.erase(_handlers.begin() + i);
            handler->release_ref();
            return;
        }
    }
}

void Logger::clear_handlers()
{
    NUT_DEBUGGING_ASSERT_ALIVE;

    for (size_t i = 0, sz = _handlers.size(); i < sz; ++i)
    {
        LogHandler *handler = _handlers.at(i);
        assert(nullptr != handler);
        handler->release_ref();
    }
    _handlers.clear();
}

void Logger::log(LogLevel level, const char *tag, const char *file, int line,
                 const char *func, const char *format, ...) const
{
    NUT_DEBUGGING_ASSERT_ALIVE;
    assert(nullptr != format);

    if (_handlers.empty())
        return;

    if (_filter.is_forbidden(tag, level))
        return;

    size_t size = 32;
    char *buf = (char*) ::malloc(size);
    assert(nullptr != buf);
    va_list ap;
    while (nullptr != buf)
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
    if (nullptr == buf)
        return;

    LogRecord record(level, tag, file, line, func, buf); // 'buf' will be freed by LogRecord
    for (size_t i = 0, sz = _handlers.size(); i < sz; ++i)
    {
        LogHandler *handler = _handlers.at(i);
        assert(nullptr != handler);
        if (handler->get_filter().is_forbidden(tag, level))
            continue;
        handler->handle_log(record);
    }
}

void Logger::load_xml_config(const std::string& config)
{
    class TagHandler : public XmlElementHandler
    {
        LogFilter *_filter = nullptr;
        std::string _tag_name;
        ll_mask_type _forbid_mask = 0;

    public:
        TagHandler()
            : XmlElementHandler(HANDLE_ATTRIBUTE)
        {}

        void reset(LogFilter *filter)
        {
            assert(nullptr != filter);
            _filter = filter;
        }

        virtual void handle_attribute(const std::string& name, const std::string& value) override
        {
            if (name == "name")
            {
                _tag_name = value;
            }
            else if (name == "forbids")
            {
                std::vector<std::string> forbids;
                chr_split(value.c_str(), ',', &forbids, true);

                _forbid_mask = 0;
                for (size_t i = 0, sz = forbids.size(); i < sz; ++i)
                    _forbid_mask |= str_to_log_level(forbids.at(i).c_str());
            }
        }

        virtual void handle_finish() override
        {
            _filter->forbid(_tag_name.c_str(), _forbid_mask);
        }
    } tag_xml_handler;

    class FilterHandler : public XmlElementHandler
    {
        TagHandler *_tag_xml_handler = nullptr;
        LogFilter *_filter = nullptr;

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

        virtual XmlElementHandler* handle_child(const std::string& name) override
        {
            if (name == "Tag")
            {
                _tag_xml_handler->reset(_filter);
                return _tag_xml_handler;
            }
            return nullptr;
        }
    } filter_xml_handler(&tag_xml_handler);

    class HandlerHandler : public XmlElementHandler
    {
        FilterHandler *_filter_xml_handler = nullptr;

        std::string _type;
        std::string _path;
        std::string _file_prefix;
        bool _append = false;
        bool _close_syslog_on_exit = false;
        bool _cross_file = true;
        size_t _circle = 10;
        long _max_file_size = 1 * 1024 * 1024;
        ll_mask_type _flush_mask = LL_FATAL;
        LogFilter _filter;

    public:
        HandlerHandler(FilterHandler *filter_xml_handler)
            : XmlElementHandler(HANDLE_ATTRIBUTE | HANDLE_CHILD),
              _filter_xml_handler(filter_xml_handler)
        {
            assert(nullptr != filter_xml_handler);
        }

        virtual XmlElementHandler* handle_child(const std::string& name) override
        {
            if (name == "Filter")
            {
                _filter_xml_handler->reset(&_filter);
                return _filter_xml_handler;
            }
            return nullptr;
        }

        virtual void handle_attribute(const std::string& name, const std::string& value) override
        {
            if (name == "type")
            {
                _type = value;
            }
            else if (name == "flushs")
            {
                std::vector<std::string> flushs;
                chr_split(value.c_str(), ',', &flushs, true);

                _flush_mask = 0;
                for (size_t i = 0, sz = flushs.size(); i < sz; ++i)
                    _flush_mask |= str_to_log_level(flushs.at(i).c_str());
            }
            else if (name == "path")
            {
                _path = value;
            }
            else if (name == "append")
            {
                _append = (value == "true" || value == "1");
            }
            else if (name == "circle")
            {
                _circle = str_to_long(value);
            }
            else if (name == "file_prefix")
            {
                _file_prefix = value;
            }
            else if (name == "max_file_size")
            {
                _max_file_size = str_to_long(value);
            }
            else if (name == "cross_file")
            {
                _cross_file = (value == "true" || value == "1");
            }
            else if (name == "close_syslog_on_exit")
            {
                _close_syslog_on_exit = (value == "true" || value == "1");
            }
        }

        virtual void handle_finish() override
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
                rc_ptr<FileLogHandler> handler = rc_new<FileLogHandler>(_path.c_str(), _append);
                handler->set_flush_mask(_flush_mask);
                handler->get_filter().swap(&_filter);
                Logger::get_instance()->add_handler(handler);
            }
            else if (_type == "cicle_file_by_size")
            {
                if (_path.empty())
                    return;
                rc_ptr<CircleFileBySizeLogHandler> handler = rc_new<CircleFileBySizeLogHandler>(
                            _path, _file_prefix, _circle, _max_file_size, _cross_file);
                handler->set_flush_mask(_flush_mask);
                handler->get_filter().swap(&_filter);
                Logger::get_instance()->add_handler(handler);
            }
            else if (_type == "file_cicle_by_time")
            {
                if (_path.empty())
                    return;
                rc_ptr<CircleFileByTimeLogHandler> handler = rc_new<CircleFileByTimeLogHandler>(
                            _path, _file_prefix, _circle);
                handler->set_flush_mask(_flush_mask);
                handler->get_filter().swap(&_filter);
                Logger::get_instance()->add_handler(handler);
            }
#if NUT_PLATFORM_OS_MAC || NUT_PLATFORM_OS_LINUX
            else if (_type == "syslog")
            {
                rc_ptr<SyslogLogHandler> handler = rc_new<SyslogLogHandler>(_close_syslog_on_exit);
                handler->set_flush_mask(_flush_mask);
                handler->get_filter().swap(&_filter);
                Logger::get_instance()->add_handler(handler);
            }
#endif
        }
    } handler_xml_handler(&filter_xml_handler);

    class LoggerHandler : public XmlElementHandler
    {
        FilterHandler *_filter_xml_handler = nullptr;
        HandlerHandler *_handler_xml_handler = nullptr;

    public:
        LoggerHandler(FilterHandler *filter_xml_handler, HandlerHandler *handler_xml_handler)
            : XmlElementHandler(HANDLE_CHILD), _filter_xml_handler(filter_xml_handler),
            _handler_xml_handler(handler_xml_handler)
        {
            assert(nullptr != filter_xml_handler && nullptr != handler_xml_handler);
        }

        virtual XmlElementHandler* handle_child(const std::string& name) override
        {
            if (name == "Filter")
            {
                _filter_xml_handler->reset(&Logger::get_instance()->get_filter());
                return _filter_xml_handler;
            }
            else if (name == "Handler")
            {
                return _handler_xml_handler;
            }
            return nullptr;
        }
    } logger_xml_handler(&filter_xml_handler, &handler_xml_handler);

    class RootHandler : public XmlElementHandler
    {
        LoggerHandler *_logger_xml_handler = nullptr;

    public:
        RootHandler(LoggerHandler *logger_xml_handler)
            : XmlElementHandler(HANDLE_CHILD), _logger_xml_handler(logger_xml_handler)
        {
            assert(nullptr != logger_xml_handler);
        }

        virtual XmlElementHandler* handle_child(const std::string& name)
        {
            if (name == "Logger")
                return _logger_xml_handler;
            return nullptr;
        }
    } root_handler(&logger_xml_handler);

    _filter.clear_forbids();
    clear_handlers();

    XmlParser parser(&root_handler);
    parser.input(config.c_str());
    parser.finish();
}

}
