
#include "file_log_handler.h"

namespace nut
{

FileLogHandler::FileLogHandler(const char *file, bool append)
    : _ofs(file, (append ? std::ios::app : std::ios::trunc))
{
    if (append)
        _ofs << "\n\n\n\n\n\n------------- ---------------- ---------------\n";
}

void FileLogHandler::handle_log(const LogRecord& rec)
{
    _ofs << rec.to_string() << std::endl;

    if (0 != (_flush_mask & rec.get_level()))
        _ofs.flush();
}

}
