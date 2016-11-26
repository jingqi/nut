
#ifndef ___HEADFILE_5F20CD14_1EF8_41B3_AE28_4FA6A872CBB5_
#define ___HEADFILE_5F20CD14_1EF8_41B3_AE28_4FA6A872CBB5_

#include <iostream>

#include "../../nut_config.h"
#include "log_handler.h"


namespace nut
{

class NUT_API StreamLogHandler : public LogHandler
{
    std::ostream &_os;

public:
    StreamLogHandler(std::ostream& os);

    virtual void handle_log(const LogRecord& rec) override;
};


}

#endif
