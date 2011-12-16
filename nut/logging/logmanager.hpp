/**
 * @file LogManager.h
 * @author jingqi
 * @date 2010-8-17
 * @brief
 * last-edit : 2011-12-16 13:52:33 jingqi
 */

#ifndef ___HEADFILE___6FFFC04E_FFB6_47F6_80AA_8FD1AF3B201F_
#define ___HEADFILE___6FFFC04E_FFB6_47F6_80AA_8FD1AF3B201F_

#include <nut/platform/platform.hpp>
#include <nut/util/configfile.hpp>
#include <nut/threading/mutex.hpp>
#include <nut/threading/guard.hpp>

#include "logger.hpp"

extern "C" __declspec(dllimport) nut::ref<nut::Logger> nut_get_root_logger();

#define NUT_LOGGING_IMPL \
extern "C" __declspec(dllexport) nut::ref<nut::Logger> nut_get_root_logger() \
{ \
    static nut::ref<nut::Logger> root; \
    static nut::Mutex root_mutex; \
 \
    if (root.isNull()) \
    { \
        nut::Guard<nut::Mutex> guard(&root_mutex); \
        if (root.isNull()) \
            root = nut::gc_new<nut::Logger>((nut::Logger*)NULL, ""); \
    } \
    return root; \
}

namespace nut
{

class LogManager
{
    // make sure that no one can construct it
    LogManager();
    ~LogManager();

public :
    static weak_ref<Logger> getLogger(const std::string &path)
    {
        return nut_get_root_logger()->getLogger(path);
    }

    static void loadConfig(const ConfigFile &config)
    {
        std::vector<std::string> handlers = config.readList("LogHandlers",',');
        for (std::vector<std::string>::const_iterator it = handlers.begin(), ite = handlers.end();
            it != ite; ++it)
        {
            std::string strtype = config.readString((*it + "_type").c_str());
            std::vector<std::string> strpos = config.readList((*it + "_pos").c_str(), ':');
            std::string strfilter = config.readString((*it + "_filter").c_str());

            ref<LogHandler> handler = LogHandlerFactory::createLogHandler(strtype);
            ref<LogFilter> filter = LogFilterFactory::createLogFilter(strfilter);
            handler->setFilter(filter);
            if (strpos.size() == 0)
                nut_get_root_logger()->addHandler(handler);
            else
                for (std::vector<std::string>::const_iterator it = strpos.begin(), ite = strpos.end(); it != ite; ++it)
                    getLogger(*it)->addHandler(handler);
        }
    }

    static void loadConfig(const char *configfile)
    {
        loadConfig(ConfigFile(configfile));
    }
};

}

#endif //head file guarder

