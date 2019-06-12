
#ifndef ___HEADFILE___8E93C94A_595D_4161_A718_E024606E84A8_
#define ___HEADFILE___8E93C94A_595D_4161_A718_E024606E84A8_

#include <string>

#include "../nut_config.h"
#include "../time/date_time.h"
#include "log_level.h"


namespace nut
{

class NUT_API LogRecord
{
public:
    LogRecord(enum LogLevel level, const char *tag, const char *file_path,
              int line, const char *func) noexcept;
    ~LogRecord() noexcept;

    const DateTime& get_time() const noexcept;
    enum LogLevel get_level() const noexcept;
    const char* get_tag() const noexcept;
    const char* get_file_path() const noexcept;
    const char* get_file_name() const noexcept;
    int get_line() const noexcept;
    const char* get_message() const noexcept;
    std::string to_string() const noexcept;

private:
    LogRecord(const LogRecord&) = delete;
    LogRecord& operator=(const LogRecord&) = delete;

    void delay_init(char *message) noexcept;

private:
    DateTime _time;
    enum LogLevel _level = LL_DEBUG;
    const char *_tag = nullptr; // Can be null, which indicated a root tag
    const char *_file_path = nullptr;
    int _line = -1;
    const char *_func = nullptr; // Can be null, when the source location is out of any function
    char *_message = nullptr; // Need to be freed

    friend class Logger;
};

}

#endif
