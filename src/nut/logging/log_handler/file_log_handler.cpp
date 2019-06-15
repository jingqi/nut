
#include "../../platform/path.h"
#include "../../platform/os.h"
#include "file_log_handler.h"


namespace nut
{

FileLogHandler::FileLogHandler(const char *file, bool trunc) noexcept
{
    assert(nullptr != file);

    const bool exists = Path::exists(file);
    const bool need_newline = (!trunc && exists && Path::get_size(file) > 0);

    if (trunc && exists)
        OS::removefile(file);

    // NOTE 'O_APPEND' 模式打开的文件支持并发写
    _ofs.open(file, std::ios::app);

    if (!trunc)
    {
        if (need_newline)
            _ofs << "\n\n";
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
