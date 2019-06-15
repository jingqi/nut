
#include <string>

#include "../../platform/platform.h"

#if !NUT_PLATFORM_OS_WINDOWS
#   include <fcntl.h> // for ::open()
#   include <unistd.h> // for ::write() / ::close()
#endif

#include "../../platform/path.h"
#include "../../platform/os.h"
#include "file_log_handler.h"


namespace nut
{

FileLogHandler::FileLogHandler(const char *file, bool trunc) noexcept
{
    assert(nullptr != file);

    const bool exists = Path::exists(file);

    std::string bar;
    if (!trunc && exists && Path::get_size(file) > 0)
        bar = "\n\n------------- ---------------- ---------------\n";

    if (trunc && exists)
        OS::removefile(file);

#if NUT_PLATFORM_OS_WINDOWS
    _ofs.open(file, std::ios::app);
    if (!bar.empty())
        _ofs << bar;
#else
    // NOTE 使用 O_APPEND 标记打开的文件支持 多线程、多进程 并发写入
    _fd = ::open(file, O_CREAT | O_WRONLY | O_APPEND,
                 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    assert(_fd >= 0);
    if (!bar.empty())
        ::write(_fd, bar.data(), bar.length());
#endif
}

void FileLogHandler::handle_log(const LogRecord& rec) noexcept
{
    const std::string msg = rec.to_string() + "\n";

#if NUT_PLATFORM_OS_WINDOWS
    std::lock_guard<SpinLock> guard(_ofs_lock);
    _ofs << msg;
    if (0 != (_flush_mask & rec.get_level()))
        _ofs.flush();
#else
    // NOTE
    // - 使用 O_APPEND 标记打开的文件本身支持并发写入，无需额外加锁
    // - 内容要一次性完整写入，避免并发穿插
    // - 因直接向内核写入，故无需 flush
    ::write(_fd, msg.data(), msg.length());
#endif
}

}
