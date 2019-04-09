
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
              int line, const char *func);
    ~LogRecord();

    const DateTime& get_time() const;
    enum LogLevel get_level() const;
    const char* get_tag() const;
    const char* get_file_path() const;
    const char* get_file_name() const;
    int get_line() const;
    const char* get_message() const;
    std::string to_string() const;

private:
    LogRecord(const LogRecord&) = delete;
    LogRecord& operator=(const LogRecord&) = delete;

    void delay_init(char *message);

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
