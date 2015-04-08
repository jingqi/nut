
#include <assert.h>
#include <stdio.h> // for sprintf()

#include "platform.h"

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#   include <io.h>    // for mkdir()
#else
#   include <dirent.h>  // for DIR, dirent
#   include <limits.h>   // for PATH_MAX
#   include <sys/stat.h> // for lstat()
#   include <sys/types.h>  // for mkdir()
#   include <unistd.h> // for rmdir()
#endif

#include <nut/util/string/string_util.h>

#include "os.h"

namespace nut
{

/**
 * 枚举目录下的文件/文件夹
 *
 * @param except_file 如果传入true, 则返回值不会包含文件
 * @parma except_dir 如果传入true, 则返回值不会包含文件夹
 * @parma except_initial_dot 如果传入true, 则返回值不会包含以'.'开头的文件/文件夹
 */
void OS::listdir(const char *path, std::vector<std::string> *appended, bool except_file,
        bool except_dir, bool except_initial_dot)
{
    assert(NULL != path && NULL != appended);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    char search_path[MAX_PATH];
#   if defined(NUT_PLATFORM_CC_VC)
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
        if (except_initial_dot && '.' == wfd.cFileName[0])
            continue;
        if (except_file && !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;
        if (except_dir && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;

        appended->push_back(wfd.cFileName);
    } while (::FindNextFileA(hfind, &wfd));

    // 关闭查找句柄
    ::FindClose(hfind);
#else
    DIR *dp = NULL;
    struct dirent *dirp = NULL;
    if ((dp = ::opendir(path)) == NULL)
        return;

    while ((dirp = ::readdir(dp)) != NULL)
    {
        if (except_initial_dot && dirp->d_name[0] == '.')
            continue;

        if (except_file || except_dir)
        {
            char file_path[PATH_MAX];
            ::sprintf(file_path, "%s/%s", path, dirp->d_name);
            struct stat buf;
            if (::lstat(file_path, &buf) < 0)
                continue;
            if (except_file && !S_ISDIR(buf.st_mode))
                continue;
            if (except_dir && S_ISDIR(buf.st_mode))
                continue;
        }

        appended->push_back(dirp->d_name);
    }

    // 释放DIR (struct dirent是由DIR维护的，无需额外释放)
    ::closedir(dp);
#endif
}

void OS::listdir(const wchar_t *path, std::vector<std::wstring> *appended, bool except_file,
    bool except_dir, bool except_initial_dot)
{
    assert(NULL != path && NULL != appended);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    wchar_t search_path[MAX_PATH];
#   if defined(NUT_PLATFORM_CC_VC)
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
        if (except_initial_dot && L'.' == wfd.cFileName[0])
            continue;
        if (except_file && !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;
        if (except_dir && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;

        appended->push_back(wfd.cFileName);
    } while (::FindNextFileW(hfind, &wfd));

    // 关闭查找句柄
    ::FindClose(hfind);
#else
    std::string p;
    wstr_to_ascii(path, &p);
    std::vector<std::string> dirs;
    listdir(p.c_str(), &dirs, except_file, except_dir, except_initial_dot);
    std::wstring s;
    for (size_t i = 0, size = dirs.size(); i < size; ++i)
    {
        s.clear();
        ascii_to_wstr(dirs[i], &s);
        appended->push_back(s);
    }
#endif
}

/**
 * 复制文件
 */
bool OS::copyfile(const char *src, const char *dest)
{
    assert(NULL != src && NULL != dest);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    return FALSE != ::CopyFileA(src, dest, TRUE);
#else
    FILE *in_file = ::fopen(src, "rb");
    if (NULL == in_file)
        return false;

    FILE *out_file = ::fopen(dest, "wb+");
    if (NULL == out_file)
    {
        ::fclose(in_file);
        return false;
    }

    const int BUF_LEN = 4096;
    char buf[BUF_LEN];
    int readed = -1;
    while ((readed = (int) ::fread(buf, 1, BUF_LEN, in_file)) > 0)
    {
        ::fwrite(buf, 1, readed, out_file);
    }
    ::fclose(in_file);
    ::fclose(out_file);
    return true;
#endif
}

bool OS::copyfile(const std::string& src, const std::string& dest)
{
    return copyfile(src.c_str(), dest.c_str());
}

bool OS::copyfile(const wchar_t *src, const wchar_t *dest)
{
    assert(NULL != src && NULL != dest);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    return FALSE != ::CopyFileW(src, dest, TRUE);
#else
    std::string s, d;
    wstr_to_ascii(src, &s);
    wstr_to_ascii(dest, &d);
    return copyfile(s.c_str(), d.c_str());
#endif
}

bool OS::copyfile(const std::wstring& src, const std::wstring& dest)
{
    return copyfile(src.c_str(), dest.c_str());
}

bool OS::removefile(const char *path)
{
    assert(NULL != path);
    return -1 != ::remove(path);
}

bool OS::removefile(const std::string& path)
{
    return removefile(path.c_str());
}

bool OS::removefile(const wchar_t *path)
{
    assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
    return FALSE != ::DeleteFileW(path);
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return removefile(p.c_str());
#endif
}

bool OS::removefile(const std::wstring& path)
{
    return removefile(path.c_str());
}

bool OS::mkdir(const char *path)
{
    assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
    return FALSE != ::CreateDirectoryA(path, NULL);
#else
    return 0 == ::mkdir(path, S_IWRITE);
#endif
}

bool OS::mkdir(const std::string& path)
{
    return mkdir(path.c_str());
}

bool OS::mkdir(const wchar_t *path)
{
    assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
    return FALSE != ::CreateDirectoryW(path, NULL);
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return OS::mkdir(p.c_str());
#endif
}

bool OS::mkdir(const std::wstring& path)
{
    return mkdir(path.c_str());
}

/**
 * 删除空目录
 */
bool OS::removedir(const char *path)
{
    assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
    return FALSE != ::RemoveDirectoryA(path);
#else
    return 0 == ::rmdir(path);
#endif
}

bool OS::removedir(const std::string& path)
{
    return removedir(path.c_str());
}

bool OS::removedir(const wchar_t *path)
{
    assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
    return FALSE != ::RemoveDirectoryW(path);
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return removedir(p.c_str());
#endif
}

bool OS::removedir(const std::wstring& path)
{
    return removedir(path.c_str());
}

/**
 * 删除目录树
 */
bool OS::removetree(const char *path)
{
    assert(NULL != path);

#if defined(NUT_PLATFORM_OS_WINDOWS)
    // 删除文件
    if (0 == (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesA(path)))
        return FALSE != ::DeleteFileA(path);

    // 遍历文件夹
    char full_path[MAX_PATH];
#   if defined(NUT_PLATFORM_CC_VC)
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

#   if defined(NUT_PLATFORM_CC_VC)
        ::sprintf_s(full_path, MAX_PATH, "%s\\%s", path, wfd.cFileName);
#   else
        ::sprintf(full_path, "%s\\%s", path, wfd.cFileName);
#   endif
        ret = removetree(full_path);
    } while (ret && ::FindNextFileA(hfind, &wfd));

    // 关闭查找句柄
    ::FindClose(hfind);

    // 移除空文件夹
    if (ret)
        ret = (FALSE != ::RemoveDirectoryA(path));
    return ret;
#else
    struct stat info;
    if (0 != ::stat(path, &info))
        return false;

    // 删除文件
    if (!S_ISDIR(info.st_mode))
        return 0 == ::unlink(path); // 这里就不用 remove() 了

    // 遍历文件夹
    DIR *dp = NULL;
    struct dirent *dirp = NULL;
    if ((dp = ::opendir(path)) == NULL)
        return false;

    bool ret = true;
    char full_path[PATH_MAX];
    while (ret && (dirp = ::readdir(dp)) != NULL)
    {
        // 忽略 . 和 ..
        if (('.' == dirp->d_name[0] && '\0' == dirp->d_name[1]) ||
            ('.' == dirp->d_name[0] && '.' == dirp->d_name[1] && '\0' == dirp->d_name[2]))
            continue;

        ::sprintf(full_path, "%s/%s", path, dirp->d_name);
        ret = removetree(full_path);
    }

    // 释放DIR (struct dirent是由DIR维护的，无需额外释放)
    ::closedir(dp);

    // 删除空目录
    if (ret)
        ret = ::rmdir(path);
    return ret;
#endif
}

bool OS::removetree(const std::string& path)
{
    return removetree(path.c_str());
}

bool OS::removetree(const wchar_t *path)
{
    assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
    // 删除文件
    if (0 == (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesW(path)))
        return FALSE != ::DeleteFileW(path);

    // 遍历文件夹
    wchar_t full_path[MAX_PATH];
#   if defined(NUT_PLATFORM_CC_VC)
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

#   if defined(NUT_PLATFORM_CC_VC)
        ::swprintf_s(full_path, MAX_PATH, L"%s\\%s", path, wfd.cFileName);
#   else
        ::swprintf(full_path, L"%s\\%s", path, wfd.cFileName);
#   endif
        ret = removetree(full_path);
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
    return removetree(p.c_str());
#endif
}

bool OS::removetree(const std::wstring& path)
{
    return removetree(path.c_str());
}

}
