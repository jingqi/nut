/**
 * @file LogManager.h
 * @author jingqi
 * @date 2010-8-17
 * @brief
 * last-edit : 2012-03-25 19:39:34 jingqi
 */

#ifndef ___HEADFILE___6FFFC04E_FFB6_47F6_80AA_8FD1AF3B201F_
#define ___HEADFILE___6FFFC04E_FFB6_47F6_80AA_8FD1AF3B201F_

#include <nut/platform/platform.hpp>
#include <nut/util/propertyfile.hpp>
#include <nut/threading/mutex.hpp>
#include <nut/threading/guard.hpp>

#include "logger.hpp"

DLL_API nut::ref<nut::Logger> nut_get_root_logger();

#define NUT_LOGGING_IMPL \
DLL_API nut::ref<nut::Logger> nut_get_root_logger() \
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

    static void loadConfig(ref<PropertyFile> config)
    {
        assert(!config.isNull());
        std::vector<std::string> handlers = config->getList("LogHandlers",',');
        for (std::vector<std::string>::const_iterator it = handlers.begin(), ite = handlers.end();
            it != ite; ++it)
        {
            std::string strtype = config->getString((*it + "_type").c_str());
            std::vector<std::string> strpos = config->getList((*it + "_pos").c_str(), ':');
            std::string strfilter = config->getString((*it + "_filter").c_str());

            ref<LogHandler> handler = LogHandlerFactory::createLogHandler(strtype);
            ref<LogFilter> filter = LogFilterFactory::createLogFilter(strfilter);
            handler->addFilter(filter);
            if (strpos.size() == 0)
                nut_get_root_logger()->addHandler(handler);
            else
                for (std::vector<std::string>::const_iterator it = strpos.begin(), ite = strpos.end(); it != ite; ++it)
                    getLogger(*it)->addHandler(handler);
        }
    }

    static void loadConfig(const char *configfile)
    {
        loadConfig(gc_new<PropertyFile>(configfile));
    }
};

}

#endif //head file guarder

