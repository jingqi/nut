
#ifndef ___HEADFILE_067B6549_8608_42D0_A979_AB3E08E1B4B3_
#define ___HEADFILE_067B6549_8608_42D0_A979_AB3E08E1B4B3_

#include <string>
#include <vector>

#include <nut/platform/platform.h>
#include <nut/debugging/destroy_checker.h>
#include <nut/debugging/source_location.h>

#include "../nut_config.h"
#include "log_level.h"
#include "log_filter.h"
#include "log_handler/log_handler.h"


namespace nut
{

class NUT_API Logger
{
    LogFilter _filter;
    std::vector<LogHandler*> _handlers;

    NUT_DEBUGGING_DESTROY_CHECKER

public:
    ~Logger();

    static Logger* get_instance();

    LogFilter& get_filter();

    void add_handler(LogHandler *handler);

    void remove_handler(LogHandler *handler);

    void clear_handlers();

    void log(LogLevel level, const char *tag, const char *file, int line,
             const char *func, const char *fmt, ...) const;

    /**
     * 加载配置文件
     */
    void load_xml_config(const std::string& config);
};

}

#define NUT_LOG_D(tag, fmt, ...)                                        \
    do                                                                  \
    {                                                                   \
        ::nut::Logger::get_instance()->log(nut::LL_DEBUG, (tag),        \
                                           NUT_SOURCE_LOCATION_ARGS,    \
                                           (fmt), ##__VA_ARGS__);       \
    } while (false)

#define NUT_LOG_I(tag, fmt, ...)                                        \
    do                                                                  \
    {                                                                   \
        ::nut::Logger::get_instance()->log(nut::LL_INFO, (tag),         \
                                           NUT_SOURCE_LOCATION_ARGS,    \
                                           (fmt), ##__VA_ARGS__);       \
    } while (false)

#define NUT_LOG_W(tag, fmt, ...)                                        \
    do                                                                  \
    {                                                                   \
        ::nut::Logger::get_instance()->log(nut::LL_WARN, (tag),         \
                                           NUT_SOURCE_LOCATION_ARGS,    \
                                           (fmt), ##__VA_ARGS__);       \
    } while (false)

#define NUT_LOG_E(tag, fmt, ...)                                        \
    do                                                                  \
    {                                                                   \
        ::nut::Logger::get_instance()->log(nut::LL_ERROR, (tag),        \
                                           NUT_SOURCE_LOCATION_ARGS,    \
                                           (fmt), ##__VA_ARGS__);       \
    } while (false)

#define NUT_LOG_F(tag, fmt, ...)                                        \
    do                                                                  \
    {                                                                   \
        ::nut::Logger::get_instance()->log(nut::LL_FATAL, (tag),        \
                                           NUT_SOURCE_LOCATION_ARGS,    \
                                           (fmt), ##__VA_ARGS__);       \
    } while (false)

#endif
