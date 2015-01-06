/**
 * @file LogManager.h
 * @author jingqi
 * @date 2010-8-17
 * @brief
 * last-edit : 2015-01-06 22:13:49 jingqi
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
    if (root.is_null()) \
    { \
        nut::Guard<nut::Mutex> guard(&root_mutex); \
        if (root.is_null()) \
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
    static weak_ref<Logger> get_logger(const std::string &path)
    {
        return ((Logger*)nut_get_root_logger())->get_logger(path);
    }

    static void load_config(ref<PropertyDom> config)
    {
        assert(!config.is_null());
		std::vector<std::string> handlers;
		config->get_list("LogHandlers", &handlers, ',');
        for (std::vector<std::string>::const_iterator it = handlers.begin(), ite = handlers.end();
            it != ite; ++it)
        {
            std::string str_type = config->get_string((*it + "_type").c_str());
			std::vector<std::string> strpos;
			config->get_list((*it + "_pos").c_str(), &strpos, ':');
            std::string str_filter = config->get_string((*it + "_filter").c_str());

            ref<LogHandler> handler = LogHandlerFactory::create_log_handler(str_type);
            ref<LogFilter> filter = LogFilterFactory::create_log_filter(str_filter);
            handler->add_filter(filter);
            if (strpos.size() == 0)
                ((Logger*)nut_get_root_logger())->add_handler(handler);
            else
                for (std::vector<std::string>::const_iterator it = strpos.begin(), ite = strpos.end();
                    it != ite; ++it)
                    get_logger(*it)->add_handler(handler);
        }
    }

    static void load_config(const char *config_file)
    {
		assert(NULL != config_file);
		ref<PropertyDom> pd = gc_new<PropertyDom>();
		std::string all;
		TxtFile::read_file(config_file, &all);
		pd->parse(all);
        load_config(pd);
    }
};

}

#endif //head file guarder
