
#include <assert.h>

#include "platform.h"
#include "stdint_traits.h" // for ssize_t

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#   include<io.h> // for _access()
#   include <sys/stat.h> // for stat()
#   include <direct.h> // for getcwd()
#else
#   include <unistd.h> // for access(), getcwd()
#   include <fcntl.h> // for AT_FDCWD, AT_SYMLINK_NOFOLLOW
#   include <sys/stat.h> // for stat()
#   include <limits.h> // for PATH_MAX
#endif

#include <nut/util/string/string_util.h>

#include "path.h"

namespace nut
{

/**
 * 路径分隔符
 */
char Path::seperator()
{
#if NUT_PLATFORM_OS_WINDOWS
    return '\\';
#else
    return '/';
#endif
}

wchar_t Path::wseperator()
{
#if NUT_PLATFORM_OS_WINDOWS
    return L'\\';
#else
    return L'/';
#endif
}

/**
 * 检查字符是否是路径分隔符 '\\' '/'
 */
bool Path::is_path_separator(char c)
{
    return '\\' == c || '/' == c;
}

bool Path::is_path_separator(wchar_t c)
{
    return L'\\' == c || L'/' == c;
}

void Path::get_cwd(std::string *result)
{
    assert(nullptr != result);

#if NUT_PLATFORM_OS_WINDOWS
#   if NUT_PLATFORM_CC_VC
#       pragma warning(push)
#       pragma warning(disable: 4996)
#   endif
    char buf[MAX_PATH + 1];
    buf[0] = 0;
    getcwd(buf, MAX_PATH + 1);
#   if NUT_PLATFORM_CC_VC
#       pragma warning(pop)
#   endif
#else
    char buf[PATH_MAX + 1];
    buf[0] = 0;
    getcwd(buf, PATH_MAX + 1);
#endif

    *result += buf;
}

void Path::get_cwd(std::wstring *result)
{
    assert(nullptr != result);

#if NUT_PLATFORM_OS_WINDOWS
#   if NUT_PLATFORM_CC_VC
#       pragma warning(push)
#       pragma warning(disable: 4996)
#   endif
    char buf[MAX_PATH + 1];
    buf[0] = 0;
    getcwd(buf, MAX_PATH + 1);
#   if NUT_PLATFORM_CC_VC
#       pragma warning(pop)
#   endif
#else
    char buf[PATH_MAX + 1];
    buf[0] = 0;
    getcwd(buf, PATH_MAX + 1);
#endif

    ascii_to_wstr(buf, result);
}

std::string Path::get_cwd()
{
    std::string ret;
    Path::get_cwd(&ret);
    return ret;
}

std::wstring Path::get_wcwd()
{
    std::wstring ret;
    Path::get_cwd(&ret);
    return ret;
}

void Path::chdir(const char *cwd)
{
    assert(nullptr != cwd);
#if NUT_PLATFORM_OS_WINDOWS
    ::_chdir(cwd);
#else
    ::chdir(cwd);
#endif
}

void Path::chdir(const std::string& cwd)
{
    Path::chdir(cwd.c_str());
}

bool Path::is_abs(const char *path)
{
    assert(nullptr != path);
    if (0 == path[0])
        return false;

    // Linux / Unix root or home
    if ('/' == path[0] || '~' == path[0])
        return true;

#if NUT_PLATFORM_OS_WINDOWS
    // Windows partion root
    for (size_t i = 0; 0 != path[i]; ++i)
    {
        const char c = path[i];
        if (':' == c)
            return true;
        else if (is_path_separator(c))
            break;
    }
#endif

    return false;
}

bool Path::is_abs(const wchar_t *path)
{
    assert(nullptr != path);
    if (0 == path[0])
        return false;

    // Linux / Unix root or home
    if (L'/' == path[0] || L'~' == path[0])
        return true;

#if NUT_PLATFORM_OS_WINDOWS
    // Windows partion root
    for (size_t i = 0; 0 != path[i]; ++i)
    {
        const wchar_t c = path[i];
        if (L':' == c)
            return true;
        else if (is_path_separator(c))
            break;
    }
#endif

    return false;
}

bool Path::is_abs(const std::string& path)
{
    return Path::is_abs(path.c_str());
}

bool Path::is_abs(const std::wstring& path)
{
    return Path::is_abs(path.c_str());
}

void Path::abs_path(const char *path, std::string *result)
{
    assert(nullptr != path && nullptr != result);
    if (0 == path[0])
    {
        get_cwd(result);
        return;
    }

    // 探测是否从根路径开始
    const size_t mark = result->length();
    if (!is_abs(path))
        get_cwd(result);

    // 组装路径
    std::string part;
    for (size_t i = 0; 0 != path[i]; ++i)
    {
        const char c = path[i];
        if (!is_path_separator(c))
        {
            part.push_back(c);
            if (0 != path[i + 1])
                continue;
        }

        // 处理根目录
        if (result->length() == mark)
        {
            if (part.empty())
            {
                // Linux root
                result->push_back('/');
                continue;
            }
            else if (part == "~")
            {
#if NUT_PLATFORM_OS_WINDOWS
                // Windows c:\Users\xxx
                *result += ::getenv("USERPROFILE");
#else
                // MacOS /Users/xxx
                // Linux /home/xxx
                *result += ::getenv("HOME");
#endif
                result->push_back(seperator());
                part.clear();
                continue;
            }
            else if (part.at(part.length() - 1) == ':')
            {
                // Windows partition root
                *result += part;
                result->push_back(seperator());
                part.clear();
                continue;
            }
        }

        // 组装
        if (!part.empty())
        {
            if (part == "..")
            {
                int j = (int) result->size() - 1;
                if (is_path_separator(result->at(j)))
                    --j;
                while (j >= (int) mark)
                {
                    if (is_path_separator(result->at(j)))
                        break;
                    --j;
                }
                if (j == (int) mark || (j > (int) mark && result->at(j - 1) == ':'))
                    result->resize(j + 1);
                else if (j > (int) mark)
                    result->resize(j);
                // else: parent directory out of range
            }
            else if (part != ".")
            {
                if (result->length() > mark && !is_path_separator(result->at(result->length() - 1)))
                {
                    result->push_back(seperator());
                }
                *result += part;
            }
            part.clear();
        }
    }
}

void Path::abs_path(const wchar_t *path, std::wstring *result)
{
    assert(nullptr != path && nullptr != result);
    if (0 == path[0])
    {
        get_cwd(result);
        return;
    }

    // 探测是否从根路径开始
    const size_t mark = result->length();
    if (!is_abs(path))
        get_cwd(result);

    // 组装路径
    std::wstring part;
    for (size_t i = 0; 0 != path[i]; ++i)
    {
        const wchar_t c = path[i];
        if (!is_path_separator(c))
        {
            part.push_back(c);
            if (0 != path[i + 1])
                continue;
        }

        // 处理根目录
        if (result->length() == mark)
        {
            if (part.empty())
            {
                // Linux root
                result->push_back(L'/');
                continue;
            }
            else if (part == L"~")
            {
#if NUT_PLATFORM_OS_WINDOWS
                // Windows c:\Users\xxx
                ascii_to_wstr(::getenv("USERPROFILE"), result);
#else
                // MacOS /Users/xxx
                // Linux /home/xxx
                ascii_to_wstr(::getenv("HOME"), result);
#endif
                result->push_back(wseperator());
                part.clear();
                continue;
            }
            else if (part.at(part.length() - 1) == L':')
            {
                // Windows partition root
                *result += part;
                result->push_back(wseperator());
                part.clear();
                continue;
            }
        }

        // 组装
        if (!part.empty())
        {
            if (part == L"..")
            {
                int j = (int) result->size() - 1;
                if (is_path_separator(result->at(j)))
                    --j;
                while (j >= (int) mark)
                {
                    if (is_path_separator(result->at(j)))
                        break;
                    --j;
                }
                if (j == (int) mark || (j > (int) mark && result->at(j - 1) == L':'))
                    result->resize(j + 1);
                else if (j > (int) mark)
                    result->resize(j);
                // else: parent directory out of range
            }
            else if (part != L".")
            {
                if (result->length() > mark && !is_path_separator(result->at(result->length() - 1)))
                {
                    result->push_back(seperator());
                }
                *result += part;
            }
            part.clear();
        }
    }
}

void Path::abs_path(const std::string& path, std::string *result)
{
    Path::abs_path(path.c_str(), result);
}

void Path::abs_path(const std::wstring& path, std::wstring *result)
{
    Path::abs_path(path.c_str(), result);
}

std::string Path::abs_path(const char *path)
{
    std::string ret;
    Path::abs_path(path, &ret);
    return ret;
}

std::wstring Path::abs_path(const wchar_t *path)
{
    std::wstring ret;
    Path::abs_path(path, &ret);
    return ret;
}

std::string Path::abs_path(const std::string& path)
{
    std::string ret;
    Path::abs_path(path.c_str(), &ret);
    return ret;
}

std::wstring Path::abs_path(const std::wstring& path)
{
    std::wstring ret;
    Path::abs_path(path.c_str(), &ret);
    return ret;
}

void Path::relative_path(const char *input_path, const char *ref_path, std::string *result)
{
    assert(nullptr != input_path && nullptr != ref_path && nullptr != result);

    std::string abs_input_path;
    abs_path(input_path, &abs_input_path);
    std::string abs_ref_path;
    abs_path(ref_path, &abs_ref_path);

    // 查找公共父目录
    const size_t input_length = abs_input_path.length(),
        ref_length = abs_ref_path.length();
    size_t common_length = 0;
    for (size_t i = 0; i <= input_length && i <= ref_length; ++i)
    {
        const char c1 = (i < input_length ? abs_input_path.at(i) : seperator()),
            c2 = (i < ref_length ? abs_ref_path.at(i) : seperator());
        const bool s1 = is_path_separator(c1), s2 = is_path_separator(c2);
#if NUT_PLATFORM_OS_WINDOWS
        if (0 != chricmp(c1, c2) && !(s1 && s2))
            break;
#else
        if (c1 != c2 && !(s1 && s2))
            break;
#endif
        if (s1)
            common_length = i + 1;
    }

    // Windows 下磁盘号不同，无法求得相对路径
    if (0 == common_length)
    {
        *result += abs_input_path;
        return;
    }

    // 参考路径距离公共父目录的层数
    size_t parent_level = 0, name_length = 0;
    for (size_t i = common_length; i < ref_length; ++i)
    {
        const char c = abs_ref_path.at(i);
        if (is_path_separator(c))
        {
            if (name_length > 0)
                ++parent_level;
            name_length = 0;
        }
        else
        {
            ++name_length;
        }
    }
    if (name_length > 0)
        ++parent_level;

    // 拼接最终结果
    size_t tail_start = common_length;
    while (tail_start < input_length &&
           is_path_separator(abs_input_path.at(tail_start)))
        ++tail_start;
    if (0 == parent_level && tail_start >= input_length)
    {
        *result += '.';
        return;
    }
    for (size_t i = 0; i < parent_level; ++i)
    {
        if (0 != i)
            result->push_back(seperator());
        *result += "..";
    }
    if (tail_start < input_length)
    {
        if (parent_level > 0)
            result->push_back(seperator());
        *result += abs_input_path.c_str() + tail_start;
    }
}

void Path::relative_path(const wchar_t *input_path, const wchar_t *ref_path, std::wstring *result)
{
    assert(nullptr != input_path && nullptr != ref_path && nullptr != result);

    std::wstring abs_input_path;
    abs_path(input_path, &abs_input_path);
    std::wstring abs_ref_path;
    abs_path(ref_path, &abs_ref_path);

    // 查找公共父目录
    const size_t input_length = abs_input_path.length(),
        ref_length = abs_ref_path.length();
    size_t common_length = 0;
    for (size_t i = 0; i <= input_length && i <= ref_length; ++i)
    {
        const wchar_t c1 = (i < input_length ? abs_input_path.at(i) : wseperator()),
            c2 = (i < ref_length ? abs_ref_path.at(i) : wseperator());
        const bool s1 = is_path_separator(c1), s2 = is_path_separator(c2);
#if NUT_PLATFORM_OS_WINDOWS
        if (0 != chricmp(c1, c2) && !(s1 && s2))
            break;
#else
        if (c1 != c2 && !(s1 && s2))
            break;
#endif
        if (s1)
            common_length = i + 1;
    }

    // Windows 下磁盘号不同，无法求得相对路径
    if (0 == common_length)
    {
        *result += abs_input_path;
        return;
    }

    // 参考路径距离公共父目录的层数
    size_t parent_level = 0, name_length = 0;
    for (size_t i = common_length; i < ref_length; ++i)
    {
        const wchar_t c = abs_ref_path.at(i);
        if (is_path_separator(c))
        {
            if (name_length > 0)
                ++parent_level;
            name_length = 0;
        }
        else
        {
            ++name_length;
        }
    }
    if (name_length > 0)
        ++parent_level;

    // 拼接最终结果
    size_t tail_start = common_length;
    while (tail_start < input_length &&
           is_path_separator(abs_input_path.at(tail_start)))
        ++tail_start;
    if (0 == parent_level && tail_start >= input_length)
    {
        *result += L'.';
        return;
    }
    for (size_t i = 0; i < parent_level; ++i)
    {
        if (0 != i)
            result->push_back(wseperator());
        *result += L"..";
    }
    if (tail_start < input_length)
    {
        if (parent_level > 0)
            result->push_back(wseperator());
        *result += abs_input_path.c_str() + tail_start;
    }
}

void Path::relative_path(const std::string& input_path, const std::string& ref_path, std::string *result)
{
    assert(nullptr != result);
    relative_path(input_path.c_str(), ref_path.c_str(), result);
}

void Path::relative_path(const std::wstring& input_path, const std::wstring& ref_path, std::wstring *result)
{
    assert(nullptr != result);
    relative_path(input_path.c_str(), ref_path.c_str(), result);
}

std::string Path::relative_path(const char *input_path, const char *ref_path)
{
    assert(nullptr != input_path && nullptr != ref_path);
    std::string result;
    relative_path(input_path, ref_path, &result);
    return result;
}

std::wstring Path::relative_path(const wchar_t *input_path, const wchar_t *ref_path)
{
    assert(nullptr != input_path && nullptr != ref_path);
    std::wstring result;
    relative_path(input_path, ref_path, &result);
    return result;
}

std::string Path::relative_path(const std::string& input_path, const std::string& ref_path)
{
    std::string result;
    relative_path(input_path.c_str(), ref_path.c_str(), &result);
    return result;
}

std::wstring Path::relative_path(const std::wstring& input_path, const std::wstring& ref_path)
{
    std::wstring result;
    relative_path(input_path.c_str(), ref_path.c_str(), &result);
    return result;
}

/**
 * 从路径中划分出父路径和 文件/文件夹 名
 *
 * 例如：
 * "ab/c/d" -> "ab/c" "d"
 * "/ab.txt" -> "/" "ab.txt"
 * "c:\\tmp" -> "c:\\" "tmp"
 */
void Path::split(const char *path, std::string *parent_result, std::string *child_result)
{
    assert(nullptr != path && (nullptr != parent_result || nullptr != child_result));

    // 找到最后一个 '/'
    ssize_t pos = -1;
    for (ssize_t i = 0; 0 != path[i]; ++i)
    {
        if (is_path_separator(path[i]))
            pos = i;
    }
    if (pos < 0)
    {
        if (nullptr != child_result)
            *child_result += path;
        return;
    }

    if (nullptr != parent_result)
    {
#if NUT_PLATFORM_OS_WINDOWS
        if (0 == pos || ':' == path[pos - 1]) // Unix 根目录; 磁盘号 + 根目录
#else
        if (0 == pos) // Unix 根目录
#endif
            parent_result->append(path, pos + 1);
        else
            parent_result->append(path, pos);
    }
    if (nullptr != child_result)
        child_result->append(path + pos + 1);
}

void Path::split(const wchar_t *path, std::wstring *parent_result, std::wstring *child_result)
{
    assert(nullptr != path && (nullptr != parent_result || nullptr != child_result));

    // 找到最后一个 '/'
    ssize_t pos = -1;
    for (ssize_t i = 0; 0 != path[i]; ++i)
    {
        if (is_path_separator(path[i]))
            pos = i;
    }
    if (pos < 0)
    {
        if (nullptr != child_result)
            *child_result += path;
        return;
    }

    if (nullptr != parent_result)
    {
#if NUT_PLATFORM_OS_WINDOWS
        if (0 == pos || L':' == path[pos - 1]) // Unix 根目录; 磁盘号 + 根目录
#else
        if (0 == pos) // Unix 根目录
#endif
            parent_result->append(path, pos + 1);
        else
            parent_result->append(path, pos);
    }
    if (nullptr != child_result)
        child_result->append(path + pos + 1);
}

void Path::split(const std::string& path, std::string *parent_result, std::string *child_result)
{
    Path::split(path.c_str(), parent_result, child_result);
}

void Path::split(const std::wstring& path, std::wstring *parent_result, std::wstring *child_result)
{
    Path::split(path.c_str(), parent_result, child_result);
}

/**
 * 从路径中划分出磁盘号和路径(Linux路径的磁盘号假定为"")
 *
 * 例如：
 * "c:\\mn\\p" -> "c:" "\\mn\\p"
 * "/mnt/sdcard" -> "" "/mnt/sdcard"
 */
void Path::split_drive(const char *path, std::string *drive_result, std::string *rest_result)
{
    assert(nullptr != path && (nullptr != drive_result || nullptr != rest_result));

#if NUT_PLATFORM_OS_WINDOWS
    ssize_t pos = -1;
    for (ssize_t i = 0; 0 != path[i]; ++i)
    {
        if (is_path_separator(path[i]))
            break;
        if (':' == path[i])
        {
            pos = i;
            break;
        }
    }
    if (pos < 0)
    {
        if (nullptr != rest_result)
            *rest_result += path;
        return;
    }
    if (nullptr != drive_result)
        drive_result->append(path, pos + 1);
    if (nullptr != rest_result)
        rest_result->append(path + pos + 1);
#else
    if (nullptr != rest_result)
        *rest_result += path;
#endif
}

void Path::split_drive(const wchar_t *path, std::wstring *drive_result, std::wstring *rest_result)
{
    assert(nullptr != path && (nullptr != drive_result || nullptr != rest_result));

#if NUT_PLATFORM_OS_WINDOWS
    ssize_t pos = -1;
    for (ssize_t i = 0; 0 != path[i]; ++i)
    {
        if (is_path_separator(path[i]))
            break;
        if (L':' == path[i])
        {
            pos = i;
            break;
        }
    }
    if (pos < 0)
    {
        if (nullptr != rest_result)
            *rest_result += path;
        return;
    }
    if (nullptr != drive_result)
        drive_result->append(path, pos + 1);
    if (nullptr != rest_result)
        rest_result->append(path + pos + 1);
#else
    if (nullptr != rest_result)
        *rest_result += path;
#endif
}

void Path::split_drive(const std::string& path, std::string *drive_result, std::string *rest_result)
{
    Path::split_drive(path.c_str(), drive_result, rest_result);
}

void Path::split_drive(const std::wstring& path, std::wstring *drive_result, std::wstring *rest_result)
{
    Path::split_drive(path.c_str(), drive_result, rest_result);
}

/**
 * 从路径或者文件名中分离出后缀名
 *
 * 例如：
 * "a.txt" -> "a" ".txt"
 */
void Path::split_ext(const char *path, std::string *prefix_result, std::string *ext_result)
{
    assert(nullptr != path && (nullptr != prefix_result || nullptr != ext_result));

    ssize_t pos = -1;
    for (ssize_t i = ::strlen(path) - 1; i >= 0; --i)
    {
        if (is_path_separator(path[i]))
            break;
        if ('.' == path[i])
        {
            // 排除 "a/.abc" 的情况
            if (i - 1 >= 0 && !is_path_separator(path[i - 1]))
                pos = i;
            break;
        }
    }
    if (pos < 0)
    {
        if (nullptr != prefix_result)
            *prefix_result += path;
        return;
    }
    if (nullptr != prefix_result)
        prefix_result->append(path, pos);
    if (nullptr != ext_result)
        ext_result->append(path + pos);
}

void Path::split_ext(const wchar_t *path, std::wstring *prefix_result, std::wstring *ext_result)
{
    assert(nullptr != path && (nullptr != prefix_result || nullptr != ext_result));

    ssize_t pos = -1;
    for (ssize_t i = ::wcslen(path) - 1; i >= 0; --i)
    {
        if (is_path_separator(path[i]))
            break;
        if (L'.' == path[i])
        {
            // 排除 "a/.abc" 的情况
            if (i - 1 >= 0 && !is_path_separator(path[i - 1]))
                pos = i;
            break;
        }
    }
    if (pos < 0)
    {
        if (nullptr != prefix_result)
            *prefix_result += path;
        return;
    }
    if (nullptr != prefix_result)
        prefix_result->append(path, pos);
    if (nullptr != ext_result)
        ext_result->append(path + pos);
}

void Path::split_ext(const std::string& path, std::string *prefix_result, std::string *ext_result)
{
    Path::split_ext(path.c_str(), prefix_result, ext_result);
}

void Path::split_ext(const std::wstring& path, std::wstring *prefix_result, std::wstring *ext_result)
{
    Path::split_ext(path.c_str(), prefix_result, ext_result);
}

bool Path::lexists(const char *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    return -1 != ::_access(path, 0);
#elif NUT_PLATFORM_OS_MAC // mac 下 faccessat() 实现不支持 AT_SYMLINK_NOFOLLOW
    // 对符号链接做特殊处理
    struct stat info;
    if (0 != ::lstat(path, &info))
        return false;
    if (S_ISLNK(info.st_mode))
        return true;
    // 非符号链接
    return 0 == ::access(path, F_OK); // F_OK 检查存在性
#else
    return 0 == ::faccessat(AT_FDCWD, path, F_OK, AT_SYMLINK_NOFOLLOW); // F_OK 检查存在性, AT_SYMLINK_NOFOLLOW 不解析符号链接
#endif
}

bool Path::lexists(const wchar_t *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    return -1 != ::_waccess(path, 0);
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return Path::lexists(p.c_str());
#endif
}

bool Path::lexists(const std::string& path)
{
    return Path::lexists(path.c_str());
}

bool Path::lexists(const std::wstring& path)
{
    return Path::lexists(path.c_str());
}

bool Path::exists(const char *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    return -1 != ::_access(path, 0);
#else
    return 0 == ::access(path, F_OK); // F_OK 检查存在性
#endif
}

bool Path::exists(const wchar_t *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    return -1 != ::_waccess(path, 0);
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return Path::exists(p.c_str());
#endif
}

bool Path::exists(const std::string& path)
{
    return Path::exists(path.c_str());
}

bool Path::exists(const std::wstring& path)
{
    return Path::exists(path.c_str());
}

/**
 * last access time
 */
time_t Path::get_atime(const char *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_stat(path, &info);
    return info.st_atime;
#else
    struct stat info;
    if (0 != ::stat(path, &info))
        return 0;
    return info.st_atime;
#endif
}

time_t Path::get_atime(const wchar_t *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_wstat(path, &info);
    return info.st_atime;
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return Path::get_atime(p.c_str());
#endif
}

time_t Path::get_atime(const std::string& path)
{
    return Path::get_atime(path.c_str());
}

time_t Path::get_atime(const std::wstring& path)
{
    return Path::get_atime(path.c_str());
}

/**
 * last modified time
 */
time_t Path::get_mtime(const char *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_stat(path, &info);
    return info.st_mtime;
#else
    struct stat info;
    if (0 != ::stat(path, &info))
        return 0;
    return info.st_mtime;
#endif
}

time_t Path::get_mtime(const wchar_t *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_wstat(path, &info);
    return info.st_mtime;
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return Path::get_mtime(p.c_str());
#endif
}

time_t Path::get_mtime(const std::string& path)
{
    return Path::get_mtime(path.c_str());
}

time_t Path::get_mtime(const std::wstring& path)
{
    return Path::get_mtime(path.c_str());
}

/**
 * created time
 */
time_t Path::get_ctime(const char *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_stat(path, &info);
    return info.st_ctime;
#else
    struct stat info;
    if (0 != ::stat(path, &info))
        return 0;
    return info.st_ctime;
#endif
}

time_t Path::get_ctime(const wchar_t *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_wstat(path, &info);
    return info.st_ctime;
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return Path::get_ctime(p.c_str());
#endif
}

time_t Path::get_ctime(const std::string& path)
{
    return Path::get_ctime(path.c_str());
}

time_t Path::get_ctime(const std::wstring& path)
{
    return Path::get_ctime(path.c_str());
}

/**
 * 获取文件大小
 */
long long Path::get_size(const char *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_stat(path, &info);
    return info.st_size;
#else
    struct stat info;
    if (0 != ::stat(path, &info))
        return -1L;
    return info.st_size;
#endif
}

long long Path::get_size(const wchar_t *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_wstat(path, &info);
    return info.st_size;
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return Path::get_size(p.c_str());
#endif
}

long long Path::get_size(const std::string& path)
{
    return Path::get_size(path.c_str());
}

long long Path::get_size(const std::wstring& path)
{
    return Path::get_size(path.c_str());
}

bool Path::is_dir(const char *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    return 0 != (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesA(path));
#else
    struct stat info;
    if (0 != ::stat(path, &info))
        return false;
    return S_ISDIR(info.st_mode);
#endif
}

bool Path::is_dir(const wchar_t *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    return 0 != (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesW(path));
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return Path::is_dir(p.c_str());
#endif
}

bool Path::is_dir(const std::string& path)
{
    return Path::is_dir(path.c_str());
}

bool Path::is_dir(const std::wstring& path)
{
    return Path::is_dir(path.c_str());
}

bool Path::is_file(const char *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    return 0 == (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesA(path));
#else
    struct stat info;
    if (0 != ::stat(path, &info))
        return false;
    return S_ISREG(info.st_mode); // regular file
#endif
}

bool Path::is_file(const wchar_t *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    return 0 == (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesW(path));
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return Path::is_file(p.c_str());
#endif
}

bool Path::is_file(const std::string& path)
{
    return Path::is_file(path.c_str());
}

bool Path::is_file(const std::wstring& path)
{
    return Path::is_file(path.c_str());
}

bool Path::is_link(const char *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    UNUSED(path);
    return false;
#else
    struct stat info;
    if (0 != ::lstat(path, &info))
        return false;
    return S_ISLNK(info.st_mode);
#endif
}

bool Path::is_link(const wchar_t *path)
{
    assert(nullptr != path);

#if NUT_PLATFORM_OS_WINDOWS
    UNUSED(path);
    return false;
#else
    std::string p;
    wstr_to_ascii(path, &p);
    return Path::is_link(p.c_str());
#endif
}

bool Path::is_link(const std::string& path)
{
    return Path::is_link(path.c_str());
}

bool Path::is_link(const std::wstring& path)
{
    return Path::is_link(path.c_str());
}

template <typename C>
static ssize_t str_find(const C *str, C c)
{
    assert(nullptr != str);
    for (size_t i = 0; 0 != str[i]; ++i)
    {
        if (str[i] == c)
            return i;
    }
    return -1;
}

/**
 * 连接两个子路径
 *
 * 例如：
 * "a" "b" -> "a/b"
 * "/" "sd" -> "/sd"
 * "c:" "\\tmp" -> "c:\\tmp"
 */
void Path::join(const char *a, const char *b, std::string *result)
{
    assert(nullptr != a && nullptr != b && nullptr != result);

    // 处理根目录 '/', 'c:\\'
    if (0 == a[0] || is_path_separator(b[0])
#if NUT_PLATFORM_OS_WINDOWS
        || str_find(b, ':') >= 0
#endif
        )
    {
        *result += b;
        return;
    }

    // 连接
    const size_t a_len = ::strlen(a);
    if (is_path_separator(a[a_len - 1]))
    {
        *result += a;
        *result += b;
    }
    else
    {
        *result += a;
        result->push_back(seperator());
        *result += b;
    }
}

void Path::join(const wchar_t *a, const wchar_t *b, std::wstring *result)
{
    assert(nullptr != a && nullptr != b && nullptr != result);

    // 处理根目录 '/', 'c:\\'
    if (0 == a[0] || is_path_separator(b[0])
#if NUT_PLATFORM_OS_WINDOWS
        || str_find(b, L':') >= 0
#endif
        )
    {
        *result += b;
        return;
    }

    // 连接
    const size_t a_len = ::wcslen(a);
    if (is_path_separator(a[a_len - 1]))
    {
        *result += a;
        *result += b;
    }
    else
    {
        *result += a;
        result->push_back(wseperator());
        *result += b;
    }
}

void Path::join(const std::string& a, const std::string& b, std::string *result)
{
    Path::join(a.c_str(), b.c_str(), result);
}

void Path::join(const std::wstring& a, const std::wstring& b, std::wstring *result)
{
    Path::join(a.c_str(), b.c_str(), result);
}

std::string Path::join(const char *a, const char *b)
{
    std::string ret;
    Path::join(a, b, &ret);
    return ret;
}

std::wstring Path::join(const wchar_t *a, const wchar_t *b)
{
    std::wstring ret;
    Path::join(a, b, &ret);
    return ret;
}

std::string Path::join(const std::string& a, const std::string& b)
{
    std::string ret;
    Path::join(a, b, &ret);
    return ret;
}

void Path::join(const char *a, const char *b, const char *c, std::string *result)
{
    std::string tmp;
    Path::join(a, b, &tmp);
    Path::join(tmp.c_str(), c, result);
}

void Path::join(const wchar_t *a, const wchar_t *b, const wchar_t *c, std::wstring *result)
{
    std::wstring tmp;
    Path::join(a, b, &tmp);
    Path::join(tmp.c_str(), c, result);
}

void Path::join(const std::string& a, const std::string& b, const std::string& c, std::string *result)
{
    Path::join(a.c_str(), b.c_str(), c.c_str(), result);
}

void Path::join(const std::wstring& a, const std::wstring& b, const std::wstring& c, std::wstring *result)
{
    Path::join(a.c_str(), b.c_str(), c.c_str(), result);
}

std::string Path::join(const char *a, const char *b, const char *c)
{
    std::string ret;
    Path::join(a, b, c, &ret);
    return ret;
}

std::wstring Path::join(const wchar_t *a, const wchar_t *b, const wchar_t *c)
{
    std::wstring ret;
    Path::join(a, b, c, &ret);
    return ret;
}

std::string Path::join(const std::string& a, const std::string& b, const std::string& c)
{
    std::string ret;
    Path::join(a.c_str(), b.c_str(), c.c_str(), &ret);
    return ret;
}

std::wstring Path::join(const std::wstring& a, const std::wstring& b, const std::wstring& c)
{
    std::wstring ret;
    Path::join(a.c_str(), b.c_str(), c.c_str(), &ret);
    return ret;
}

void Path::join(const char *a, const char *b, const char *c, const char *d, std::string *result)
{
    std::string tmp;
    join(a, b, c, &tmp);
    join(tmp.c_str(), d, result);
}

void Path::join(const wchar_t *a, const wchar_t *b, const wchar_t *c, const wchar_t *d, std::wstring *result)
{
    std::wstring tmp;
    join(a, b, c, &tmp);
    join(tmp.c_str(), d, result);
}

void Path::join(const std::string& a, const std::string& b, const std::string& c, const std::string& d, std::string *result)
{
    Path::join(a.c_str(), b.c_str(), c.c_str(), d.c_str(), result);
}

void Path::join(const std::wstring& a, const std::wstring& b, const std::wstring& c, const std::wstring& d, std::wstring *result)
{
    Path::join(a.c_str(), b.c_str(), c.c_str(), d.c_str(), result);
}

std::string Path::join(const char *a, const char *b, const char *c, const char *d)
{
    std::string ret;
    Path::join(a, b, c, d, &ret);
    return ret;
}

std::wstring Path::join(const wchar_t *a, const wchar_t *b, const wchar_t *c, const wchar_t *d)
{
    std::wstring ret;
    Path::join(a, b, c, d, &ret);
    return ret;
}

std::string Path::join(const std::string& a, const std::string& b, const std::string& c, const std::string& d)
{
    std::string ret;
    Path::join(a.c_str(), b.c_str(), c.c_str(), d.c_str(), &ret);
    return ret;
}

std::wstring Path::join(const std::wstring& a, const std::wstring& b, const std::wstring& c, const std::wstring& d)
{
    std::wstring ret;
    Path::join(a.c_str(), b.c_str(), c.c_str(), d.c_str(), &ret);
    return ret;
}

}
