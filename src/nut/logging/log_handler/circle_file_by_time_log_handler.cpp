
#include <algorithm> // for std::sort()
#include <mutex> // for std::lock_guard

#include "../../platform/platform.h"

#if !NUT_PLATFORM_OS_WINDOWS
#   include <fcntl.h> // for ::open()
#   include <unistd.h> // for ::write() / ::close()
#endif

#include "../../platform/os.h"
#include "../../platform/path.h"
#include "../../time/date_time.h"
#include "../../util/string/to_string.h"
#include "../../util/string/string_utils.h"
#include "circle_file_by_time_log_handler.h"


namespace nut
{

CircleFileByTimeLogHandler::CircleFileByTimeLogHandler(
    const std::string& dir_path, const std::string& prefix,
    const char *time_format, size_t circle_size) noexcept
{
    assert(nullptr != time_format && circle_size > 0);

    // 找到目录下所有的日志文件
    const std::string log_suffix(".log");
    const std::vector<std::string> file_names = OS::listdir(dir_path, false, true, true);
    std::vector<std::string> logfile_names;
    for (size_t i = 0; i < file_names.size(); ++i)
    {
        const std::string& name = file_names.at(i);

        // 匹配前缀、后缀
        if (!starts_with(name, prefix) || !ends_with(name, log_suffix))
            continue;

        logfile_names.push_back(name);
    }

    // 删除多余的日志文件
    if (logfile_names.size() > circle_size - 1)
    {
        std::sort(logfile_names.begin(), logfile_names.end());
        for (size_t i = 0, del_count = logfile_names.size() - circle_size + 1;
             i < del_count; ++i)
        {
            const std::string full_path = Path::join(dir_path, logfile_names.at(i));
            OS::removefile(full_path);
        }
    }

    // 打开日志文件
    std::string file_name = prefix;
    file_name += DateTime::now().format_time(time_format);
    file_name += log_suffix;
    const std::string full_path = Path::join(dir_path, file_name);

    std::string bar;
    if (Path::exists(full_path) && Path::get_size(full_path) > 0)
        bar = "\n\n------------- ---------------- ---------------\n";

#if NUT_PLATFORM_OS_WINDOWS
    _ofs.open(full_path.c_str(), std::ios::app);
    if (!bar.empty())
        _ofs << bar;
#else
    // NOTE 使用 O_APPEND 标记打开的文件支持 多线程、多进程 并发写入
    _fd = ::open(full_path.c_str(), O_CREAT | O_WRONLY | O_APPEND,
                 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    assert(_fd >= 0);
    if (!bar.empty())
        ::write(_fd, bar.data(), bar.length());
#endif
}

void CircleFileByTimeLogHandler::handle_log(const LogRecord& rec) noexcept
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
