
#include <nut/platform/platform.h>
#include <nut/util/txtcfg/property_dom.h>
#include <nut/util/txtcfg/txt_file.h>
#include <nut/threading/sync/mutex.h>
#include <nut/threading/sync/guard.hpp>

#include "log_manager.h"
#include "logger.h"

namespace nut
{

Logger* LogManager::get_logger(const std::string &path)
{
    return ((Logger*)nut_get_root_logger())->get_logger(path);
}

void LogManager::load_config(rc_ptr<PropertyDom> config)
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

        rc_ptr<LogHandler> handler = LogHandlerFactory::create_log_handler(str_type);
        rc_ptr<LogFilter> filter = LogFilterFactory::create_log_filter(str_filter);
        handler->add_filter(filter);
        if (strpos.size() == 0)
        {
            ((Logger*)nut_get_root_logger())->add_handler(handler);
        }
        else
        {
            for (std::vector<std::string>::const_iterator it = strpos.begin(), ite = strpos.end();
                it != ite; ++it)
            {
                get_logger(*it)->add_handler(handler);
            }
        }
    }
}

void LogManager::load_config(const char *config_file)
{
    assert(NULL != config_file);
    rc_ptr<PropertyDom> pd = RC_NEW(NULL, PropertyDom);
    std::string all;
    TxtFile::read_file(config_file, &all);
    pd->parse(all);
    load_config(pd);
}

}
