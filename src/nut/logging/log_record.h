
#ifndef ___HEADFILE___8E93C94A_595D_4161_A718_E024606E84A8_
#define ___HEADFILE___8E93C94A_595D_4161_A718_E024606E84A8_

#include <string>

#include <nut/util/time/date_time.h>

#include "../nut_config.h"
#include "log_level.h"


namespace nut
{

class NUT_API LogRecord
{
public:
    LogRecord(LogLevel level, const char *tag, const char *file_path, int line,
        const char *func, char *message);

    ~LogRecord();

    const DateTime& get_time() const;
    LogLevel get_level() const;
    const char* get_tag() const;
    const char* get_file_path() const;
    const char* get_file_name() const;
    int get_line() const;
    const char* get_message() const;
    std::string to_string() const;

private:
    // Non-copyable
    LogRecord(const LogRecord&) = delete;
    LogRecord& operator=(const LogRecord&) = delete;

private:
    DateTime _time;
    LogLevel _level = LogLevel::Debug;
    const char *_tag = nullptr; // Can be null, which indicated a root tag
    const char *_file_path = nullptr;
    int _line = -1;
    const char *_func = nullptr; // Can be null, when the source location is out of any function
    char *_message = nullptr; // Need to be freed
};

}

#endif
