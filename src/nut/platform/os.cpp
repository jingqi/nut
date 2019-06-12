
#include <assert.h>
#include <stdio.h> // for sprintf(), rename()

#include "platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#   include <io.h>    // for mkdir()
#elif NUT_PLATFORM_OS_MACOS
#   include <dirent.h>  // for DIR, dirent
#   include <limits.h>   // for PATH_MAX
#   include <sys/stat.h> // for ::lstat()
#   include <sys/types.h>  // for ::mkdir()
#   include <unistd.h> // for ::rmdir()
#   include <copyfile.h> // for ::copyfile()
#elif NUT_PLATFORM_OS_LINUX
#   include <dirent.h>  // for DIR, dirent
#   include <limits.h>   // for PATH_MAX
#   include <sys/stat.h> // for ::lstat()
#   include <sys/types.h>  // for ::mkdir()
#   include <unistd.h> // for ::rmdir()
#   include <fcntl.h> // for ::open()
#   include <sys/sendfile.h> // for ::sendfile()
#else
#   include <dirent.h>  // for DIR, dirent
#   include <limits.h>   // for PATH_MAX
#   include <sys/stat.h> // for ::lstat()
#   include <sys/types.h>  // for ::mkdir()
#   include <unistd.h> // for ::rmdir()
#   include <fcntl.h> // for ::open(), ::posix_fadvise(), ::posix_fallocate() and so on
#endif

#include "../util/string/string_utils.h"
#include "os.h"
#include "path.h"


namespace nut
{

std::vector<std::string> OS::listdir(const std::string& path, bool exclude_file,
    bool exclude_dir, bool exclude_initial_dot) noexcept
{
    const std::string abspath = Path::abspath(path);
    std::vector<std::string> result;

#if NUT_PLATFORM_OS_WINDOWS
    const std::string search_path = abspath + "\\*"; /* 加上通配符 */
    WIN32_FIND_DATAA wfd;
    const HANDLE hfind = ::FindFirstFileA(search_path.c_str(), &wfd);
    if (INVALID_HANDLE_VALUE == hfind)
        return result;

    do
    {
        if (exclude_initial_dot && '.' == wfd.cFileName[0])
            continue;
        if (exclude_file && !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;
        if (exclude_dir && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;

        result.emplace_back(wfd.cFileName);
    } while (::FindNextFileA(hfind, &wfd));

    // 关闭查找句柄
    ::FindClose(hfind);
#else
    DIR *const dir = ::opendir(abspath.c_str());
    if (nullptr == dir)
        return result;

    while (true)
    {
        struct dirent *const ent = ::readdir(dir);
        if (nullptr == ent)
            break;
        if (exclude_initial_dot && ent->d_name[0] == '.')
            continue; // 'ent' 是由 'dir' 维护的，无需额外释放

        if (exclude_file || exclude_dir)
        {
            const std::string filepath = Path::join(abspath, ent->d_name);
            struct stat status;
            if (::stat(filepath.c_str(), &status) < 0)
                continue;
            if (exclude_file && !S_ISDIR(status.st_mode))
                continue;
            if (exclude_dir && S_ISDIR(status.st_mode))
                continue;
        }

        result.emplace_back(ent->d_name);
    }

    // 释放DIR
    ::closedir(dir);
#endif

    return result;
}

std::vector<std::wstring> OS::listdir(const std::wstring& path, bool exclude_file,
    bool exclude_dir, bool exclude_initial_dot) noexcept
{
    std::vector<std::wstring> result;

#if NUT_PLATFORM_OS_WINDOWS
    const std::wstring search_path = Path::abspath(path) + L"\\*"; /* 加上通配符 */
    WIN32_FIND_DATAW wfd;
    const HANDLE hfind = ::FindFirstFileW(search_path.c_str(), &wfd);
    if (INVALID_HANDLE_VALUE == hfind)
        return result;

    do
    {
        if (exclude_initial_dot && L'.' == wfd.cFileName[0])
            continue;
        if (exclude_file && !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;
        if (exclude_dir && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;

        result.emplace_back(wfd.cFileName);
    } while (::FindNextFileW(hfind, &wfd));

    // 关闭查找句柄
    ::FindClose(hfind);
#else
    const std::vector<std::string> dirs = OS::listdir(
        wstr_to_ascii(path), exclude_file, exclude_dir, exclude_initial_dot);
    for (size_t i = 0, size = dirs.size(); i < size; ++i)
        result.push_back(ascii_to_wstr(dirs.at(i)));
#endif

    return result;
}

bool OS::copyfile(const std::string& src, const std::string& dst) noexcept
{
    const std::string abssrc = Path::abspath(src), absdst = Path::abspath(dst);

#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::CopyFileA(abssrc.c_str(), absdst.c_str(), FALSE);
#elif NUT_PLATFORM_OS_MACOS
    return 0 == ::copyfile(abssrc.c_str(), absdst.c_str(), nullptr, COPYFILE_ALL | COPYFILE_NOFOLLOW);
#elif NUT_PLATFORM_OS_LINUX
    const int in_file = ::open(abssrc.c_str(), O_RDONLY);
    if (in_file < 0)
        return false;

    struct stat info;
    ::fstat(in_file, &info);

    const int out_file = ::open(absdst.c_str(), O_WRONLY | O_CREAT | O_TRUNC, info.st_mode);
    if (out_file < 0)
    {
        ::close(in_file);
        return false;
    }

    off_t offset = 0;
    const ssize_t wrote = ::sendfile(out_file, in_file, &offset, info.st_size);

    ::close(out_file);
    ::close(in_file);

    return wrote == info.st_size;
#else
    const int in_file = ::open(abssrc.c_str(), O_RDONLY);
    if (in_file < 0)
        return false;

    // 给系统一个建议，使其优化顺序读
    // see http://stackoverflow.com/questions/7463689/most-efficient-way-to-copy-a-file-in-linux
    ::posix_fadvise(in_file, 0, 0, POSIX_FADV_SEQUENTIAL);

    struct stat info;
    ::fstat(in_file, &info);

    const int out_file = ::open(absdst.c_str(), O_WRONLY | O_CREAT | O_TRUNC, info.st_mode);
    if (out_file < 0)
    {
        ::close(in_file);
        return false;
    }

    // 给文件预先分配大小，同时提前探知磁盘空间不足的情况
    if (0 != ::posix_fallocate(out_file, 0, info.st_size))
    {
        ::close(out_file);
        ::close(in_file);
        return false; // 磁盘空间不足
    }

    bool success = true;
    const int BUF_LEN = 8 * 1024;
    char buf[BUF_LEN];
    while (true)
    {
        const ssize_t readed = ::read(in_file, buf, BUF_LEN);
        if (readed < 0)
            success = false;
        if (readed <= 0)
            break;
        const ssize_t wrote = ::write(out_file, buf, readed);
        if (wrote != readed)
        {
            success = false;
            break;
        }
    }

    ::close(out_file);
    ::close(in_file);

    return success;
#endif
}

bool OS::copyfile(const std::wstring& src, const std::wstring& dst) noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    const std::wstring abssrc = Path::abspath(src), absdst = Path::abspath(dst);
    return FALSE != ::CopyFileW(abssrc.c_str(), absdst.c_str(), FALSE);
#else
    return OS::copyfile(wstr_to_ascii(src), wstr_to_ascii(dst));
#endif
}

bool OS::removefile(const std::string& path) noexcept
{
    const std::string abspath = Path::abspath(path);

#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::DeleteFileA(abspath.c_str());
#else
    return 0 == ::remove(abspath.c_str());
#endif
}

bool OS::removefile(const std::wstring& path) noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::DeleteFileW(Path::abspath(path).c_str());
#else
    return OS::removefile(wstr_to_ascii(path));
#endif
}

bool OS::mkdir(const std::string& path) noexcept
{
    const std::string abspath = Path::abspath(path);

#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::CreateDirectoryA(abspath.c_str(), nullptr);
#else
    return 0 == ::mkdir(abspath.c_str(), S_IREAD | S_IWRITE | S_IEXEC);
#endif
}

bool OS::mkdir(const std::wstring& path) noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::CreateDirectoryW(Path::abspath(path).c_str(), nullptr);
#else
    return OS::mkdir(wstr_to_ascii(path));
#endif
}

bool OS::mkdirs(const std::string& path) noexcept
{
    // 可能目录已经存在
    const std::string abspath = Path::abspath(path);
    if (Path::exists(abspath))
        return Path::is_dir(abspath);

    // 可能是根目录
    std::string parent, name;
    Path::split(abspath, &parent, &name);
    if (parent.length() == abspath.length() || name.empty())
        return false; // 根目录是无法创建的

    // 递归创建
    if (!Path::exists(parent) && !OS::mkdirs(parent))
        return false;
    return OS::mkdir(abspath);
}

bool OS::mkdirs(const std::wstring& path) noexcept
{
    return OS::mkdirs(wstr_to_ascii(path));
}

bool OS::rmdir(const std::string& path) noexcept
{
    const std::string abspath = Path::abspath(path);

#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::RemoveDirectoryA(abspath.c_str());
#else
    return 0 == ::rmdir(abspath.c_str());
#endif
}

bool OS::rmdir(const std::wstring& path) noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::RemoveDirectoryW(Path::abspath(path).c_str());
#else
    return OS::rmdir(wstr_to_ascii(path));
#endif
}

bool OS::rmtree(const std::string& path) noexcept
{
    const std::string abspath = Path::abspath(path);

#if NUT_PLATFORM_OS_WINDOWS
    // 删除文件
    if (0 == (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesA(abspath.c_str())))
        return FALSE != ::DeleteFileA(abspath.c_str());

    // 遍历文件夹
    const std::string search_path = abspath + "\\*"; /* 加上通配符 */
    WIN32_FIND_DATAA wfd;
    const HANDLE hfind = ::FindFirstFileA(search_path.c_str(), &wfd);
    if (INVALID_HANDLE_VALUE == hfind)
        return false;

    bool ret = true;
    do
    {
        // 忽略 . 和 ..
        if (('.' == wfd.cFileName[0] && '\0' == wfd.cFileName[1]) ||
            ('.' == wfd.cFileName[0] && '.' == wfd.cFileName[1] && '\0' == wfd.cFileName[2]))
            continue;

        ret = OS::rmtree(Path::join(abspath, wfd.cFileName));
    } while (ret && ::FindNextFileA(hfind, &wfd));

    // 关闭查找句柄
    ::FindClose(hfind);

    // 移除空文件夹
    if (ret)
        ret = (FALSE != ::RemoveDirectoryA(abspath.c_str()));
    return ret;
#else
    struct stat info;
    if (0 != ::lstat(abspath.c_str(), &info))
        return false;

    // 删除文件、软连接
    if (!S_ISDIR(info.st_mode))
        return 0 == ::unlink(abspath.c_str()); // Same as ::remove(), but won't follow link

    // 遍历文件夹
    DIR *const dir = ::opendir(abspath.c_str());
    if (nullptr == dir)
        return false;

    bool ret = true;
    do
    {
        struct dirent *const ent = ::readdir(dir);
        if (nullptr == ent)
            break;

        // 忽略 . 和 ..
        if (('.' == ent->d_name[0] && '\0' == ent->d_name[1]) ||
            ('.' == ent->d_name[0] && '.' == ent->d_name[1] && '\0' == ent->d_name[2]))
            continue; // 'ent' 是由 'dir' 维护的，无需额外释放

        ret = OS::rmtree(Path::join(abspath, ent->d_name));
    } while (ret);

    // 释放DIR
    ::closedir(dir);

    // 删除空目录
    if (ret)
        ret = (0 == ::rmdir(abspath.c_str()));
    return ret;
#endif
}

bool OS::rmtree(const std::wstring& path) noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    // 删除文件
    const std::wstring abspath = Path::abspath(path);
    if (0 == (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesW(abspath.c_str())))
        return FALSE != ::DeleteFileW(abspath.c_str());

    // 遍历文件夹
    const std::wstring search_path = abspath + L"\\*"; /* 加上通配符 */
    WIN32_FIND_DATAW wfd;
    const HANDLE hfind = ::FindFirstFileW(search_path.c_str(), &wfd);
    if (INVALID_HANDLE_VALUE == hfind)
        return false;

    bool ret = true;
    do
    {
        // 忽略 . 和 ..
        if ((L'.' == wfd.cFileName[0] && L'\0' == wfd.cFileName[1]) ||
            (L'.' == wfd.cFileName[0] && L'.' == wfd.cFileName[1] && L'\0' == wfd.cFileName[2]))
            continue;

        ret = OS::rmtree(Path::join(abspath, wfd.cFileName));
    } while (ret && ::FindNextFileW(hfind, &wfd));

    // 关闭查找句柄
    ::FindClose(hfind);

    // 移除空文件夹
    if (ret)
        ret = (FALSE != ::RemoveDirectoryW(abspath.c_str()));
    return ret;
#else
    return OS::rmtree(wstr_to_ascii(path));
#endif
}

std::string OS::readlink(const std::string& path) noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    UNUSED(path);
    return std::string(); // windows 上没有软链接功能
#else
    std::string result(PATH_MAX, 0);
    const ssize_t rs = ::readlink(
        Path::abspath(path).c_str(), const_cast<char*>(result.data()), PATH_MAX);
    result.resize(rs < 0 ? 0 : rs);
    return result;
#endif
}

std::wstring OS::readlink(const std::wstring& path) noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    UNUSED(path);
    return std::wstring(); // windows 上没有软链接功能
#else
    return ascii_to_wstr(OS::readlink(wstr_to_ascii(path)));
#endif
}

bool OS::symlink(const std::string& link, const std::string& path) noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    UNUSED(link);
    UNUSED(path);
    return false; // windows 上没有软链接功能
#else
    return 0 == ::symlink(link.c_str(), Path::abspath(path).c_str());
#endif
}

bool OS::symlink(const std::wstring& link, const std::wstring& path) noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    UNUSED(link);
    UNUSED(path);
    return false; // windows 上没有软链接功能
#else
    return OS::symlink(wstr_to_ascii(link), wstr_to_ascii(path));
#endif
}

bool OS::rename(const std::string& from, const std::string& to) noexcept
{
    return 0 == ::rename(Path::abspath(from).c_str(), Path::abspath(to).c_str());
}

bool OS::rename(const std::wstring& from, const std::wstring& to) noexcept
{
#if NUT_PLATFORM_OS_WINDOWS
    return 0 == ::_wrename(Path::abspath(from).c_str(), Path::abspath(to).c_str());
#else
    return OS::rename(wstr_to_ascii(from), wstr_to_ascii(to));
#endif
}

}
