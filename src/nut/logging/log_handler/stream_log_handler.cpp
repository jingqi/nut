
#include <mutex>

#include "../../util/string/to_string.h"
#include "stream_log_handler.h"


namespace nut
{

StreamLogHandler::StreamLogHandler(std::ostream& os) noexcept
    : _os(os)
{}

void StreamLogHandler::handle_log(const LogRecord& rec) noexcept
{
    const std::string msg = std::string("[") +
        rec.get_time().format_time("%H:%M:%S.%3f") + "] " +
        log_level_to_str(rec.get_level()) + " (" + rec.get_file_name() + ":" +
        int_to_str(rec.get_line()) + ") " + rec.get_message() + "\n";

    std::lock_guard<SpinLock> guard(_lock);
    _os << msg;
    if (0 != (_flush_mask & rec.get_level()))
        _os.flush();
}

}
