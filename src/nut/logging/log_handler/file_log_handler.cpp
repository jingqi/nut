
#include "../../platform/path.h"
#include "file_log_handler.h"


namespace nut
{

FileLogHandler::FileLogHandler(const char *file, bool append) noexcept
    : _ofs(file, (append ? std::ios::app : std::ios::trunc))
{
    if (append)
    {
        if (Path::exists(file))
        {
            const long long file_size = Path::get_size(file);
            if (file_size > 0)
                _ofs << "\n\n";
        }
        _ofs << "------------- ---------------- ---------------\n";
    }
}

void FileLogHandler::handle_log(const LogRecord& rec) noexcept
{
    _ofs << rec.to_string() << std::endl;

    if (0 != (_flush_mask & rec.get_level()))
        _ofs.flush();
}

}
