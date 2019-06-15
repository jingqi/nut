
#include <algorithm> // for std::sort()

#include "../../platform/platform.h"

#if !NUT_PLATFORM_OS_WINDOWS
#   include <fcntl.h> // for ::open()
#   include <unistd.h> // for ::write() / ::close()
#endif

#include "../../platform/os.h"
#include "../../platform/path.h"
#include "../../util/string/to_string.h"
#include "../../util/string/string_utils.h"
#include "circle_file_by_size_log_handler.h"


namespace nut
{

// 计算十进制数字长度
static size_t decimal_digit_count(size_t num)
{
    size_t ret = 0;
    while (num > 0)
    {
        ++ret;
        num /= 10;
    }
    return 0 == ret ? 1 : ret;
}

// 格式化序号到指定长度
//  e.g. format_seq(12, 4) -> "0012"
static std::string format_seq(size_t seq, size_t len)
{
    std::string ret;
    const std::string s = ulong_to_str(seq);
    for (size_t i = s.length(); i < len; ++i)
        ret.push_back('0');
    ret += s;
    return ret;
}

// 判断字串是否全是有数字组成
static bool is_all_digits(const std::string& s, size_t start, size_t len)
{
    for (size_t i = start; i < start + len; ++i)
    {
        const char c = s.at(i);
        if (c < '0' || c > '9')
            return false;
    }
    return true;
}

CircleFileBySizeLogHandler::CircleFileBySizeLogHandler(
    const std::string& dir_path, const std::string& prefix, size_t circle_size,
    long long max_file_size, bool cross_file) noexcept
    : _dir_path(dir_path), _file_prefix(prefix), _circle_size(circle_size),
      _max_file_size(max_file_size), _cross_file(cross_file)
{
    assert(_circle_size > 0);

    const size_t digits_len = decimal_digit_count(circle_size - 1);
    const std::string name = prefix + format_seq(0, digits_len) + ".log";
    const std::string path = Path::join(dir_path, name);
    long long filesz = 0;
    if (Path::exists(path))
        filesz = Path::get_size(path);
    _file_size.store(filesz, std::memory_order_relaxed);

    if (filesz >= max_file_size)
        circle_once();
    else
        open_log_file(path);
}

#if !NUT_PLATFORM_OS_WINDOWS
CircleFileBySizeLogHandler::~CircleFileBySizeLogHandler()
{
    const int fd = _fd.exchange(-1, std::memory_order_relaxed);
    if (fd >= 0)
        ::close(fd);
}
#endif

void CircleFileBySizeLogHandler::open_log_file(const std::string& file) noexcept
{
    long long filesz = 0;
    if (Path::exists(file))
        filesz = Path::get_size(file);

    std::string msg;
    if (filesz > 0)
        msg = "\n\n";
    msg += "------------- ---------------- ---------------\n";
    filesz += msg.length();

#if NUT_PLATFORM_OS_WINDOWS
    std::lock_guard<SpinLock> guard(_ofs_lock);
    _ofs.close();
    _ofs.clear();
    _ofs.open(file.c_str(), std::ios::app);
    _ofs << msg;
    _file_size.store(filesz, std::memory_order_relaxed);
#else
    // NOTE 使用 O_APPEND 标记打开的文件支持 多线程、多进程 并发写入
    const int fd = ::open(file.c_str(), O_CREAT | O_WRONLY | O_APPEND,
                          S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    assert(fd >= 0);
    ::write(fd, msg.data(), msg.length());
    const int old_fd = _fd.exchange(fd, std::memory_order_relaxed);
    _file_size.store(filesz, std::memory_order_relaxed);
    if (old_fd >= 0)
        ::close(old_fd);
#endif
}

void CircleFileBySizeLogHandler::circle_once() noexcept
{
    // 只需要一个线程来处理
    std::unique_lock<std::mutex> guard(_fileop_lock, std::try_to_lock);
    if (!guard.owns_lock())
        return;

    // Double check
    if (_file_size.load(std::memory_order_relaxed) < _max_file_size)
        return;

    // 找到目录下所有的日志文件
    const size_t prefix_len = _file_prefix.length();
    const size_t digits_len = decimal_digit_count(_circle_size - 1);
    const std::string log_suffix = ".log";
    const std::vector<std::string> file_names = OS::listdir(_dir_path, false, true, true);
    std::vector<std::string> logfile_names;
    for (size_t i = 0, sz = file_names.size(); i < sz; ++i)
    {
        const std::string& name = file_names.at(i);

        // 名称长度
        if (name.length() != prefix_len + digits_len + log_suffix.length())
            continue;

        // 匹配前缀、后缀
        if (!starts_with(name, _file_prefix) || !ends_with(name, log_suffix))
            continue;

        // 数字序列验证
        if (!is_all_digits(name, prefix_len, digits_len))
            continue;

        logfile_names.push_back(name);
    }

    // 删除多余的日志文件，重命名日志文件
    std::sort(logfile_names.begin(), logfile_names.end());
    for (ssize_t i = logfile_names.size() - 1; i >= 0; --i)
    {
        const std::string& name = logfile_names.at(i);
        const std::string& ori_path = Path::join(_dir_path, name);
        const size_t seq = (size_t) str_to_long(name.substr(prefix_len, digits_len));

        // 删除多余的日志文件
        if (seq + 1 >= _circle_size)
        {
            OS::removefile(ori_path);
            continue;
        }

        // 文件名中的序号加1
        const std::string new_name = _file_prefix + format_seq(seq + 1, digits_len) + log_suffix;
        OS::rename(ori_path, Path::join(_dir_path, new_name));
    }

    // 打开日志文件
    const std::string name = _file_prefix + format_seq(0, digits_len) + log_suffix;
    const std::string path = Path::join(_dir_path, name);
    open_log_file(path);
}

void CircleFileBySizeLogHandler::handle_log(const LogRecord& rec) noexcept
{
    const std::string msg = rec.to_string() + "\n";
    const size_t msglen = msg.length();

#if NUT_PLATFORM_OS_WINDOWS
    {
        std::lock_guard<SpinLock> guard(_ofs_lock);
        _ofs << msg;
        if (0 != (_flush_mask & rec.get_level()))
            _ofs.flush();
    }
#else
    // NOTE
    // - 使用 O_APPEND 标记打开的文件本身支持并发写入，无需额外加锁
    // - 内容要一次性完整写入，避免并发穿插
    // - 因直接向内核写入，故无需 flush
    ::write(_fd.load(std::memory_order_relaxed), msg.data(), msglen);
#endif

    const long long filesz = _file_size.fetch_add(msglen, std::memory_order_relaxed) + msglen;

    // Open a new log file
    if (_cross_file && filesz >= _max_file_size)
        circle_once();
}

}
