﻿
#include "stream_log_handler.h"

namespace nut
{

StreamLogHandler::StreamLogHandler(std::ostream& os)
    : _os(os)
{}

void StreamLogHandler::handle_log(const LogRecord& rec)
{
    _os << "[" << rec.get_time().format_time("%H:%M:%S.%3f") << "] " <<
         log_level_to_str(rec.get_level()) << " (" <<
            rec.get_file_name() << ":" << rec.get_line() << ") " <<
            rec.get_message() << std::endl;

    if (0 != (_flush_mask & static_cast<loglevel_mask_type>(rec.get_level())))
        _os.flush();
}

}
