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
#include <nut/util/txtcfg/property_dom.hpp>
#include <nut/util/txtcfg/txt_file.hpp>
#include <nut/threading/sync/mutex.hpp>
#include <nut/threading/sync/guard.hpp>

#include "logger.hpp"

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(push)
#   pragma warning(disable:4190)
#endif

DLL_API void* nut_get_root_logger();

#define NUT_LOGGING_IMPL \
DLL_API void* nut_get_root_logger() \
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
    return root.pointer(); \
}

#if defined(NUT_PLATFORM_CC_VC)
#   pragma warning(pop)
#endif

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
        return ((Logger*)nut_get_root_logger())->getLogger(path);
    }

    static void loadConfig(ref<PropertyDom> config)
    {
        assert(!config.isNull());
		std::vector<std::string> handlers;
		config->getList("LogHandlers", &handlers, ',');
        for (std::vector<std::string>::const_iterator it = handlers.begin(), ite = handlers.end();
            it != ite; ++it)
        {
            std::string strtype = config->getString((*it + "_type").c_str());
			std::vector<std::string> strpos;
			config->getList((*it + "_pos").c_str(), &strpos, ':');
            std::string strfilter = config->getString((*it + "_filter").c_str());

            ref<LogHandler> handler = LogHandlerFactory::createLogHandler(strtype);
            ref<LogFilter> filter = LogFilterFactory::createLogFilter(strfilter);
            handler->addFilter(filter);
            if (strpos.size() == 0)
                ((Logger*)nut_get_root_logger())->addHandler(handler);
            else
                for (std::vector<std::string>::const_iterator it = strpos.begin(), ite = strpos.end(); it != ite; ++it)
                    getLogger(*it)->addHandler(handler);
        }
    }

    static void loadConfig(const char *configfile)
    {
		assert(NULL != configfile);
		ref<PropertyDom> pd = gc_new<PropertyDom>();
		std::string all;
		TxtFile::read_file(configfile, &all);
		pd->parse(all);
        loadConfig(pd);
    }
};

}

#endif //head file guarder

