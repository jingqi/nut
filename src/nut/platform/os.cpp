
#include <assert.h>
#include <stdio.h> // for sprintf(), rename()

#include "platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#   include <io.h>    // for mkdir()
#elif NUT_PLATFORM_OS_MAC
#   include <dirent.h>  // for DIR, dirent
#   include <limits.h>   // for PATH_MAX
#   include <sys/stat.h> // for ::lstat()
#   include <sys/types.h>  // for ::mkdir()
#   include <unistd.h> // for ::rmdir()
#   include <copyfile.h> // for ::copyfile()
#else
#   include <dirent.h>  // for DIR, dirent
#   include <limits.h>   // for PATH_MAX
#   include <sys/stat.h> // for ::lstat()
#   include <sys/types.h>  // for ::mkdir()
#   include <unistd.h> // for ::rmdir()
#endif

#include <nut/util/string/string_util.h>

#include "os.h"
#include "path.h"

namespace nut
{

/**
 * 枚举目录下的文件/文件夹
 *
 * @param exclude_file 如果传入true, 则返回值不会包含文件
 * @parma exclude_dir 如果传入true, 则返回值不会包含文件夹
 * @parma exclude_initial_dot 如果传入true, 则返回值不会包含以'.'开头的文件/文件夹
 */
void OS::list_dir(const char *path, std::vector<std::string> *result,
                  bool exclude_file, bool exclude_dir, bool exclude_initial_dot)
{
    assert(nullptr != path && nullptr != result);

#if NUT_PLATFORM_OS_WINDOWS
    char search_path[MAX_PATH];
#   if NUT_PLATFORM_CC_VC
    ::sprintf_s(search_path, MAX_PATH, "%s\\*", path);
#   else
    ::sprintf(search_path, "%s\\*", path); /* 加上通配符 */
#   endif

    WIN32_FIND_DATAA wfd;
    const HANDLE hfind = ::FindFirstFileA(search_path, &wfd);
    if (hfind == INVALID_HANDLE_VALUE)
        return;

    do
    {
        if (exclude_initial_dot && '.' == wfd.cFileName[0])
            continue;
        if (exclude_file && !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;
        if (exclude_dir && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;

        result->push_back(wfd.cFileName);
    } while (::FindNextFileA(hfind, &wfd));

    // 关闭查找句柄
    ::FindClose(hfind);
#else
    DIR *dp = nullptr;
    struct dirent *dirp = nullptr;
    if ((dp = ::opendir(path)) == nullptr)
        return;

    while ((dirp = ::readdir(dp)) != nullptr)
    {
        if (exclude_initial_dot && dirp->d_name[0] == '.')
            continue;

        if (exclude_file || exclude_dir)
        {
            char file_path[PATH_MAX];
            ::sprintf(file_path, "%s/%s", path, dirp->d_name);
            struct stat buf;
            if (::lstat(file_path, &buf) < 0)
                continue;
            if (exclude_file && (S_ISLNK(buf.st_mode) || !S_ISDIR(buf.st_mode)))
                continue;
            if (exclude_dir && (!S_ISLNK(buf.st_mode) && S_ISDIR(buf.st_mode)))
                continue;
        }

        result->push_back(dirp->d_name);
    }

    // 释放DIR (struct dirent是由DIR维护的，无需额外释放)
    ::closedir(dp);
#endif
}

void OS::list_dir(const wchar_t *path, std::vector<std::wstring> *result,
                  bool exclude_file, bool exclude_dir, bool exclude_initial_dot)
{
    assert(nullptr != path && nullptr != result);

#if NUT_PLATFORM_OS_WINDOWS
    wchar_t search_path[MAX_PATH];
#   if NUT_PLATFORM_CC_VC
    ::swprintf_s(search_path, MAX_PATH, L"%s\\*", path);
#   else
    ::swprintf(search_path, L"%s\\*", path); /* 加上通配符 */
#   endif

    WIN32_FIND_DATAW wfd;
    const HANDLE hfind = ::FindFirstFileW(search_path, &wfd);
    if (hfind == INVALID_HANDLE_VALUE)
        return;

    do
    {
        if (exclude_initial_dot && L'.' == wfd.cFileName[0])
            continue;
        if (exclude_file && !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;
        if (exclude_dir && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;

        result->push_back(wfd.cFileName);
    } while (::FindNextFileW(hfind, &wfd));

    // 关闭查找句柄
    ::FindClose(hfind);
#else
    std::string p;
    wstr_to_ascii(path, &p);
    std::vector<std::string> dirs;
    OS::list_dir(p.c_str(), &dirs, exclude_file, exclude_dir, exclude_initial_dot);
    std::wstring s;
    for (size_t i = 0, size = dirs.size(); i < size; ++i)
    {
        s.clear();
        ascii_to_wstr(dirs.at(i).c_str(), &s);
        result->push_back(s);
    }
#endif
}

void OS::list_dir(const std::string& path, std::vector<std::string> *result,
                  bool exclude_file, bool exclude_dir, bool exclude_initial_dot)
{
    OS::list_dir(path.c_str(), result, exclude_file, exclude_dir, exclude_initial_dot);
}

void OS::list_dir(const std::wstring& path, std::vector<std::wstring> *result,
                  bool exclude_file, bool exclude_dir, bool exclude_initial_dot)
{
    OS::list_dir(path.c_str(), result, exclude_file, exclude_dir, exclude_initial_dot);
}

/**
 * 复制文件
 */
bool OS::copy_file(const char *src, const char *dst)
{
    assert(nullptr != src && nullptr != dst);

#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::CopyFileA(src, dst, FALSE);
#elif NUT_PLATFORM_OS_MAC
    return 0 == ::copyfile(src, dst, nullptr, COPYFILE_ALL | COPYFILE_NOFOLLOW);
#else
    FILE *in_file = ::fopen(src, "rb");
    if (nullptr == in_file)
        return false;

    FILE *out_file = ::fopen(dst, "wb");
    if (nullptr == out_file)
    {
        ::fclose(in_file);
        return false;
    }

    bool succeed = true;
    const int BUF_LEN = 4096;
    char buf[BUF_LEN];
    size_t readed = 0;
    while ((readed = ::fread(buf, 1, BUF_LEN, in_file)) > 0)
    {
        if (::fwrite(buf, 1, readed, out_file) != readed)
        {
            succeed = false;
            break;
        }
    }

    ::fclose(out_file);

    if (0 != ::ferror(in_file) || 0 == ::feof(in_file))
        succeed = false;
    ::fclose(in_file);

    return succeed;
#endif
}

bool OS::copy_file(const wchar_t *src, const wchar_t *dst)
{
    assert(nullptr != src && nullptr != dst);

#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::CopyFileW(src, dst, FALSE);
#else
    std::string s, d;
    wstr_to_ascii(src, &s);
    wstr_to_ascii(dst, &d);
    return OS::copy_file(s.c_str(), d.c_str());
#endif
}

bool OS::copy_file(const std::string& src, const std::string& dst)
{
    return OS::copy_file(src.c_str(), dst.c_str());
}

bool OS::copy_file(const std::wstring& src, const std::wstring& dst)
{
    return OS::copy_file(src.c_str(), dst.c_str());
}

bool OS::remove_file(const char *path)
{
    assert(nullptr != path);
    return 0 == ::remove(path);
}

bool OS::remove_file(const wchar_t *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::DeleteFileW(path);
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return OS::remove_file(p.c_str());
#endif
}

bool OS::remove_file(const std::string& path)
{
    return OS::remove_file(path.c_str());
}

bool OS::remove_file(const std::wstring& path)
{
    return OS::remove_file(path.c_str());
}

bool OS::mkdir(const char *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::CreateDirectoryA(path, nullptr);
#else
    return 0 == ::mkdir(path, S_IREAD | S_IWRITE | S_IEXEC);
#endif
}

bool OS::mkdir(const wchar_t *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::CreateDirectoryW(path, nullptr);
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return OS::mkdir(p.c_str());
#endif
}

bool OS::mkdir(const std::string& path)
{
    return OS::mkdir(path.c_str());
}

bool OS::mkdir(const std::wstring& path)
{
    return OS::mkdir(path.c_str());
}

bool OS::mkdirs(const char *path)
{
    assert(nullptr != path);

    // 可能目录已经存在
    if (Path::exists(path))
        return Path::is_dir(path);

    // 可能是根目录
    std::string fullpath;
    Path::abs_path(path, &fullpath);
    std::string parent, name;
    Path::split(fullpath.c_str(), &parent, &name);
    if (parent.length() == fullpath.length() || name.empty())
        return false; // 根目录是无法创建的

    // 递归创建
    if (!Path::exists(parent.c_str()) && !OS::mkdirs(parent.c_str()))
        return false;
    return OS::mkdir(path);
}

bool OS::mkdirs(const wchar_t *path)
{
    assert(nullptr != path);

    std::string p;
    wstr_to_ascii(path, &p);
    return OS::mkdirs(p.c_str());
}

bool OS::mkdirs(const std::string& path)
{
    return OS::mkdirs(path.c_str());
}

bool OS::mkdirs(const std::wstring& path)
{
    return OS::mkdirs(path.c_str());
}

/**
 * 删除空目录
 */
bool OS::rmdir(const char *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::RemoveDirectoryA(path);
#else
    return 0 == ::rmdir(path);
#endif
}

bool OS::rmdir(const wchar_t *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    return FALSE != ::RemoveDirectoryW(path);
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return OS::rmdir(p.c_str());
#endif
}

bool OS::rmdir(const std::string& path)
{
    return OS::rmdir(path.c_str());
}

bool OS::rmdir(const std::wstring& path)
{
    return OS::rmdir(path.c_str());
}

/**
 * 删除目录树
 */
bool OS::remove_tree(const char *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    // 删除文件
    if (0 == (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesA(path)))
        return FALSE != ::DeleteFileA(path);

    // 遍历文件夹
    char full_path[MAX_PATH];
#   if NUT_PLATFORM_CC_VC
    ::sprintf_s(full_path, MAX_PATH, "%s\\*", path);
#   else
    ::sprintf(full_path, "%s\\*", path); /* 加上通配符 */
#   endif

    WIN32_FIND_DATAA wfd;
    const HANDLE hfind = ::FindFirstFileA(full_path, &wfd);
    if (hfind == INVALID_HANDLE_VALUE)
        return false;

    bool ret = true;
    do
    {
        // 忽略 . 和 ..
        if (('.' == wfd.cFileName[0] && '\0' == wfd.cFileName[1]) ||
            ('.' == wfd.cFileName[0] && '.' == wfd.cFileName[1] && '\0' == wfd.cFileName[2]))
            continue;

#   if NUT_PLATFORM_CC_VC
        ::sprintf_s(full_path, MAX_PATH, "%s\\%s", path, wfd.cFileName);
#   else
        ::sprintf(full_path, "%s\\%s", path, wfd.cFileName);
#   endif
        ret = remove_tree(full_path);
    } while (ret && ::FindNextFileA(hfind, &wfd));

    // 关闭查找句柄
    ::FindClose(hfind);

    // 移除空文件夹
    if (ret)
        ret = (FALSE != ::RemoveDirectoryA(path));
    return ret;
#else
    struct stat info;
    if (0 != ::lstat(path, &info))
        return false;

    // 删除文件
    if (!S_ISDIR(info.st_mode))
        return 0 == ::remove(path); // Same as ::unlink()

    // 遍历文件夹
    DIR *dp = nullptr;
    struct dirent *dirp = nullptr;
    if ((dp = ::opendir(path)) == nullptr)
        return false;

    bool ret = true;
    char full_path[PATH_MAX];
    while (ret && (dirp = ::readdir(dp)) != nullptr)
    {
        // 忽略 . 和 ..
        if (('.' == dirp->d_name[0] && '\0' == dirp->d_name[1]) ||
            ('.' == dirp->d_name[0] && '.' == dirp->d_name[1] && '\0' == dirp->d_name[2]))
            continue;

        ::sprintf(full_path, "%s/%s", path, dirp->d_name);
        ret = OS::remove_tree(full_path);
    }

    // 释放DIR (struct dirent是由DIR维护的，无需额外释放)
    ::closedir(dp);

    // 删除空目录
    if (ret)
        ret = (0 == ::rmdir(path));
    return ret;
#endif
}

bool OS::remove_tree(const wchar_t *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    // 删除文件
    if (0 == (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesW(path)))
        return FALSE != ::DeleteFileW(path);

    // 遍历文件夹
    wchar_t full_path[MAX_PATH];
#   if NUT_PLATFORM_CC_VC
    ::swprintf_s(full_path, MAX_PATH, L"%s\\*", path);
#   else
    ::swprintf(full_path, L"%s\\*", path); /* 加上通配符 */
#   endif

    WIN32_FIND_DATAW wfd;
    const HANDLE hfind = ::FindFirstFileW(full_path, &wfd);
    if (hfind == INVALID_HANDLE_VALUE)
        return false;

    bool ret = true;
    do
    {
        // 忽略 . 和 ..
        if ((L'.' == wfd.cFileName[0] && L'\0' == wfd.cFileName[1]) ||
            (L'.' == wfd.cFileName[0] && L'.' == wfd.cFileName[1] && L'\0' == wfd.cFileName[2]))
            continue;

#   if NUT_PLATFORM_CC_VC
        ::swprintf_s(full_path, MAX_PATH, L"%s\\%s", path, wfd.cFileName);
#   else
        ::swprintf(full_path, L"%s\\%s", path, wfd.cFileName);
#   endif
        ret = remove_tree(full_path);
    } while (ret && ::FindNextFileW(hfind, &wfd));

    // 关闭查找句柄
    ::FindClose(hfind);

    // 移除空文件夹
    if (ret)
        ret = (FALSE != ::RemoveDirectoryW(path));
    return ret;
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return OS::remove_tree(p.c_str());
#endif
}

bool OS::remove_tree(const std::string& path)
{
    return OS::remove_tree(path.c_str());
}

bool OS::remove_tree(const std::wstring& path)
{
    return OS::remove_tree(path.c_str());
}

bool OS::read_link(const char *path, std::string *result)
{
    assert(nullptr != path && nullptr != result);

#if NUT_PLATFORM_OS_WINDOWS
    UNUSED(path);
    UNUSED(result);
    return false; // windows 上没有软链接功能
#else
    const size_t buf_len = 1024;
    char buf[buf_len + 1];
    const ssize_t rs = ::readlink(path, buf, buf_len);
    if (rs < 0)
        return false;
    buf[rs] = 0;
    *result += buf;
    return true;
#endif
}

bool OS::read_link(const wchar_t *path, std::wstring *result)
{
    assert(nullptr != path && nullptr != result);

#if NUT_PLATFORM_OS_WINDOWS
    UNUSED(path);
    UNUSED(result);
    return false; // windows 上没有软链接功能
#else
    std::string p;
    wstr_to_ascii(path, &p);
    std::string lk;
    if (!OS::read_link(p.c_str(), &lk))
        return false;
    ascii_to_wstr(lk.c_str(), result);
    return true;
#endif
}

bool OS::read_link(const std::string& path, std::string *result)
{
    return OS::read_link(path.c_str(), result);
}

bool OS::read_link(const std::wstring& path, std::wstring *result)
{
    return OS::read_link(path.c_str(), result);
}

bool OS::symlink(const char *link, const char *path)
{
    assert(nullptr != link && nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    UNUSED(link);
    UNUSED(path);
    return false; // windows 上没有软链接功能
#else
    return 0 == ::symlink(link, path);
#endif
}

bool OS::symlink(const wchar_t *link, const wchar_t *path)
{
    assert(nullptr != link && nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    UNUSED(link);
    UNUSED(path);
    return false; // windows 上没有软链接功能
#else
    std::string l, p;
    wstr_to_ascii(link, &l);
    wstr_to_ascii(path, &p);
    return OS::symlink(l.c_str(), p.c_str());
#endif
}

bool OS::symlink(const std::string& link, const std::string& path)
{
    return OS::symlink(link.c_str(), path.c_str());
}

bool OS::symlink(const std::wstring& link, const std::wstring& path)
{
    return OS::symlink(link.c_str(), path.c_str());
}

bool OS::rename(const char *from, const char *to)
{
    assert(nullptr != from && nullptr != to);
    return 0 == ::rename(from, to);
}

bool OS::rename(const wchar_t *from, const wchar_t *to)
{
    assert(nullptr != from && nullptr != to);

    std::string f, t;
    wstr_to_ascii(from, &f);
    wstr_to_ascii(to, &t);
    return 0 == ::rename(f.c_str(), t.c_str());
}

bool OS::rename(const std::string& from, const std::string& to)
{
    return OS::rename(from.c_str(), to.c_str());
}

bool OS::rename(const std::wstring& from, const std::wstring& to)
{
    return OS::rename(from.c_str(), to.c_str());
}

}
