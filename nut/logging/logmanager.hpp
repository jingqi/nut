/**
 * @file LogManager.h
 * @author jingqi
 * @date 2010-8-17
 * @brief
 * last-edit : jingqi 2011-03-16 10:29:35
 */

#ifndef ___HEADFILE___6FFFC04E_FFB6_47F6_80AA_8FD1AF3B201F_
#define ___HEADFILE___6FFFC04E_FFB6_47F6_80AA_8FD1AF3B201F_

#include <util/ConfigFile.h>
#if defined(WIN32)
#   include <memory>
#else
#   include <tr1/memory>         // for shared_ptr
#endif

#include "Logger.h"

namespace nut
{

class LogManager
{
    // make sure that no one can construct it
    LogManager();
    ~LogManager();

    static Logger& getRootLogger()
    {
        static Logger* volatile root = NULL;
        static threading::Mutex root_mutex;

        if (NULL == root)
        {
            threading::Guard<threading::Mutex> guard(&root_mutex);
            if (NULL == root)
                root = new Logger(NULL, "");
        }
        return *root;
    }

public :
    static Logger& getLogger(const std::string &path)
    {
        return getRootLogger().getLogger(path);
    }

    static void loadConfig(const util::ConfigFile &config)
    {
        std::vector<std::string> handlers = config.readList("LogHandlers",',');
        for (std::vector<std::string>::const_iterator it = handlers.begin(), ite = handlers.end();
            it != ite; ++it)
        {
            std::string strtype = config.readString((*it + "_type").c_str());
            std::vector<std::string> strpos = config.readList((*it + "_pos").c_str(), ':');
            std::string strfilter = config.readString((*it + "_filter").c_str());

            std::tr1::shared_ptr<LogHandler> handler = createLogHandler(strtype);
            std::tr1::shared_ptr<LogFilter> filter = createLogFilter(strfilter);
            handler->setFilter(filter);
            if (strpos.size() == 0)
                getRootLogger().addHandler(handler);
            else
                for (std::vector<std::string>::const_iterator it = strpos.begin(), ite = strpos.end(); it != ite; ++it)
                    getLogger(*it).addHandler(handler);
        }
    }

    static void loadConfig(const char *configfile)
    {
        loadConfig(util::ConfigFile(configfile));
    }
};

}

#endif //head file guarder

