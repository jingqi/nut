
#ifndef ___HEADFILE_62C1DBA1_286E_4BAE_AD7D_FD628348E94C_
#define ___HEADFILE_62C1DBA1_286E_4BAE_AD7D_FD628348E94C_

#include <fstream>

#include "../../nut_config.h"
#include "log_handler.h"


namespace nut
{

class NUT_API FileLogHandler : public LogHandler
{
public:
    explicit FileLogHandler(const char *file, bool append = false);

    virtual void handle_log(const LogRecord& rec) override;

private:
    FileLogHandler(const FileLogHandler&) = delete;
    FileLogHandler& operator=(const FileLogHandler&) = delete;

private:
    std::ofstream _ofs;
};


}

#endif
