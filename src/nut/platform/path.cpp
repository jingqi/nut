
#include <assert.h>
#include <string.h> // for ::strlen()

#include "platform.h"

#if NUT_PLATFORM_OS_WINDOWS
#   include <windows.h>
#   include <io.h> // for _access()
#   include <sys/stat.h> // for stat()
#   include <direct.h> // for getcwd()
#else
#   include <unistd.h> // for access(), getcwd()
#   include <fcntl.h> // for AT_FDCWD, AT_SYMLINK_NOFOLLOW
#   include <sys/stat.h> // for stat()
#   include <limits.h> // for PATH_MAX
#endif

#include "../util/string/string_utils.h"
#include "int_type.h" // for ssize_t
#include "path.h"
#include "os.h"


namespace nut
{

std::string Path::get_cwd()
{
#if NUT_PLATFORM_OS_WINDOWS
    char buf[MAX_PATH + 1];
    buf[0] = 0;
    ::_getcwd(buf, MAX_PATH + 1); // MSVC says name of 'getcwd()' is deprecated
    return buf;
#else
    char buf[PATH_MAX + 1];
    buf[0] = 0;
    ::getcwd(buf, PATH_MAX + 1);
    return buf;
#endif
}

std::wstring Path::get_wcwd()
{
#if NUT_PLATFORM_OS_WINDOWS
    wchar_t buf[MAX_PATH + 1];
    buf[0] = 0;
    ::_wgetcwd(buf, MAX_PATH + 1);
    return buf;
#else
    return ascii_to_wstr(Path::get_cwd());
#endif
}

void Path::chdir(const std::string& cwd)
{
    const std::string fullpath = Path::abspath(cwd);

#if NUT_PLATFORM_OS_WINDOWS
    ::_chdir(fullpath.c_str());
#else
    ::chdir(fullpath.c_str());
#endif
}

void Path::chdir(const std::wstring& cwd)
{
#if NUT_PLATFORM_OS_WINDOWS
    ::_wchdir(Path::abspath(cwd).c_str());
#else
    Path::chdir(wstr_to_ascii(cwd));
#endif
}

#if NUT_PLATFORM_OS_WINDOWS
template <typename C>
static constexpr bool is_letter(C c)
{
    return (int('A') <= c && c <= int('Z')) || (int('a') <= c && c <= int('z'));
}

// "c:" or "c:/"
template <typename STR>
static bool starts_with_win_drive(const STR& path)
{
    const size_t len = path.length();
    return len >= 2 && is_letter(path.at(0)) && int(':') == path.at(1) &&
        (len < 3 || Path::is_sep(path.at(2)));
}
#endif

bool Path::is_root(const std::string& path)
{
    if (path.empty())
        return false;
    if (path.length() == 1 && is_sep(path.at(0)))
        return true;

#if NUT_PLATFORM_OS_WINDOWS
    // "c:" or "c:/"
    return path.length() <= 3 && starts_with_win_drive(path);
#else
    return false;
#endif
}

bool Path::is_root(const std::wstring& path)
{
    if (path.empty())
        return false;
    if (path.length() == 1 && is_sep(path.at(0)))
        return true;

#if NUT_PLATFORM_OS_WINDOWS
    // "c:" or "c:/"
    return path.length() <= 3 && starts_with_win_drive(path);
#else
    return false;
#endif
}

bool Path::is_abs(const std::string& path)
{
    if (path.empty())
        return false;

    // Unix root or home
    if (is_sep(path.at(0)) || '~' == path.at(0))
        return true;

#if NUT_PLATFORM_OS_WINDOWS
    // Windows partition root
    return starts_with_win_drive(path);
#else
    return false;
#endif
}

bool Path::is_abs(const std::wstring& path)
{
    if (path.empty())
        return false;

    // Unix root or home
    if (is_sep(path.at(0)) || L'~' == path.at(0))
        return true;

#if NUT_PLATFORM_OS_WINDOWS
    // Windows partition root
    return starts_with_win_drive(path);
#else
    return false;
#endif
}

std::string Path::abspath(const std::string& path)
{
    if (path.empty())
        return get_cwd();

    // 探测是否从根路径开始
    std::string result;
    if (!is_abs(path))
        result = get_cwd();

    // 组装路径
    std::string part;
    for (size_t i = 0, len = path.length(); i < len; ++i)
    {
        const char c = path.at(i);
        if (!is_sep(c))
        {
            part.push_back(c);
            if (i + 1 < len)
                continue;
        }

        // 处理根目录
        if (result.empty())
        {
            if (part.empty())
            {
                // Unix root
                result.push_back(sep());
                continue;
            }
            else if (part == "~")
            {
#if NUT_PLATFORM_CC_VC
                // Windows c:\Users\xxx
                // NOTE getenv() may not safe, replaced by _dupenv_s()
                char *buf = nullptr;
                ::_dupenv_s(&buf, nullptr, "USERPROFILE");
                if (nullptr != buf)
                {
                    result += buf;
                    ::free(buf);
                }
#elif NUT_PLATFORM_OS_WINDOWS
                // Windows c:\Users\xxx
                result += ::getenv("USERPROFILE");
#else
                // MacOS /Users/xxx
                // Unix /home/xxx
                result += ::getenv("HOME");
#endif
                result.push_back(sep());
                part.clear();
                continue;
            }
#if NUT_PLATFORM_OS_WINDOWS
            else if (starts_with_win_drive(part))
            {
                // Windows partition root
                result += part;
                result.push_back(sep());
                part.clear();
                continue;
            }
#endif
        }

        // 组装
        if (!part.empty())
        {
            if (part == "..")
            {
                ssize_t j = (ssize_t) result.size() - 1;
                if (is_sep(result.at(j)))
                    --j;
                while (j >= 0)
                {
                    if (is_sep(result.at(j)))
                        break;
                    --j;
                }
#if NUT_PLATFORM_OS_WINDOWS
                if (0 == j || (2 == j && starts_with_win_drive(result)))
#else
                if (0 == j)
#endif
                    result.resize(j + 1);
                else if (j > 0)
                    result.resize(j);
                // else: parent directory out of range
            }
            else if (part != ".")
            {
                if (result.length() > 0 && !is_sep(result.at(result.length() - 1)))
                {
                    result.push_back(sep());
                }
                result += part;
            }
            part.clear();
        }
    }

    return result;
}

std::wstring Path::abspath(const std::wstring& path)
{
    if (path.empty())
        return get_wcwd();

    // 探测是否从根路径开始
    std::wstring result;
    if (!is_abs(path))
        result = get_wcwd();

    // 组装路径
    std::wstring part;
    for (size_t i = 0, len = path.length(); i < len; ++i)
    {
        const wchar_t c = path.at(i);
        if (!is_sep(c))
        {
            part.push_back(c);
            if (i + 1 < len)
                continue;
        }

        // 处理根目录
        if (result.empty())
        {
            if (part.empty())
            {
                // Unix root
                result.push_back(wsep());
                continue;
            }
            else if (part == L"~")
            {
#if NUT_PLATFORM_CC_VC
                // Windows c:\Users\xxx
                // NOTE getenv() may not safe, replaced by _dupenv_s()
                wchar_t *buf = nullptr;
                ::_wdupenv_s(&buf, nullptr, L"USERPROFILE");
                if (nullptr != buf)
                {
                    result += buf;
                    ::free(buf);
                }
#elif NUT_PLATFORM_OS_WINDOWS
                // Windows c:\Users\xxx
                result += ascii_to_wstr(::getenv("USERPROFILE"));
#else
                // MacOS /Users/xxx
                // Unix /home/xxx
                result += ascii_to_wstr(::getenv("HOME"));
#endif
                result.push_back(wsep());
                part.clear();
                continue;
            }
#if NUT_PLATFORM_OS_WINDOWS
            else if (starts_with_win_drive(part))
            {
                // Windows partition root
                result += part;
                result.push_back(wsep());
                part.clear();
                continue;
            }
#endif
        }

        // 组装
        if (!part.empty())
        {
            if (part == L"..")
            {
                ssize_t j = (ssize_t) result.size() - 1;
                if (is_sep(result.at(j)))
                    --j;
                while (j >= 0)
                {
                    if (is_sep(result.at(j)))
                        break;
                    --j;
                }
#if NUT_PLATFORM_OS_WINDOWS
                if (0 == j || (2 == j && starts_with_win_drive(result)))
#else
                if (0 == j)
#endif
                    result.resize(j + 1);
                else if (j > 0)
                    result.resize(j);
                // else: parent directory out of range
            }
            else if (part != L".")
            {
                if (result.length() > 0 && !is_sep(result.at(result.length() - 1)))
                {
                    result.push_back(wsep());
                }
                result += part;
            }
            part.clear();
        }
    }

    return result;
}

std::string Path::realpath(const std::string& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    return Path::abspath(path);
#else
    std::string ret = path;
    while (Path::is_link(ret))
    {
        const std::string link = OS::readlink(ret);
        if (link == ret)
            break; // Link to self
        ret = link;
    }
    return Path::abspath(ret);
#endif
}

std::wstring Path::realpath(const std::wstring& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    return Path::abspath(path);
#else
    std::wstring ret = path;
    while (Path::is_link(ret))
    {
        const std::wstring link = OS::readlink(ret);
        if (link == ret)
            break; // Link to self
        ret = link;
    }
    return Path::abspath(ret);
#endif
}

std::string Path::relpath(const std::string& input_path, const std::string& ref_path)
{
    const std::string abs_input_path = Path::abspath(input_path);
    const std::string abs_ref_path = Path::abspath(ref_path);

    // 查找公共父目录
    const size_t input_length = abs_input_path.length(),
        ref_length = abs_ref_path.length();
    size_t common_length = 0;
    for (size_t i = 0; i <= input_length && i <= ref_length; ++i)
    {
        const char c1 = (i < input_length ? abs_input_path.at(i) : sep()),
            c2 = (i < ref_length ? abs_ref_path.at(i) : sep());
        const bool s1 = is_sep(c1), s2 = is_sep(c2);
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
        return abs_input_path;

    // 参考路径距离公共父目录的层数
    size_t parent_level = 0, name_length = 0;
    for (size_t i = common_length; i < ref_length; ++i)
    {
        const char c = abs_ref_path.at(i);
        if (is_sep(c))
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
           is_sep(abs_input_path.at(tail_start)))
        ++tail_start;
    if (0 == parent_level && tail_start >= input_length)
        return ".";
    std::string result;
    for (size_t i = 0; i < parent_level; ++i)
    {
        if (0 != i)
            result.push_back(sep());
        result += "..";
    }
    if (tail_start < input_length)
    {
        if (parent_level > 0)
            result.push_back(sep());
        result += abs_input_path.c_str() + tail_start;
    }
    return result;
}

std::wstring Path::relpath(const std::wstring& input_path, const std::wstring& ref_path)
{
    std::wstring abs_input_path = Path::abspath(input_path);
    std::wstring abs_ref_path = Path::abspath(ref_path);

    // 查找公共父目录
    const size_t input_length = abs_input_path.length(),
        ref_length = abs_ref_path.length();
    size_t common_length = 0;
    for (size_t i = 0; i <= input_length && i <= ref_length; ++i)
    {
        const wchar_t c1 = (i < input_length ? abs_input_path.at(i) : wsep()),
            c2 = (i < ref_length ? abs_ref_path.at(i) : wsep());
        const bool s1 = is_sep(c1), s2 = is_sep(c2);
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
        return abs_input_path;

    // 参考路径距离公共父目录的层数
    size_t parent_level = 0, name_length = 0;
    for (size_t i = common_length; i < ref_length; ++i)
    {
        const wchar_t c = abs_ref_path.at(i);
        if (is_sep(c))
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
           is_sep(abs_input_path.at(tail_start)))
        ++tail_start;
    if (0 == parent_level && tail_start >= input_length)
        return L".";
    std::wstring result;
    for (size_t i = 0; i < parent_level; ++i)
    {
        if (0 != i)
            result.push_back(wsep());
        result += L"..";
    }
    if (tail_start < input_length)
    {
        if (parent_level > 0)
            result.push_back(wsep());
        result += abs_input_path.c_str() + tail_start;
    }
    return result;
}

std::string Path::dirname(const std::string& path)
{
    std::string ret;
    Path::split(path, &ret, nullptr);
    return ret;
}

std::wstring Path::dirname(const std::wstring& path)
{
    std::wstring ret;
    Path::split(path, &ret, nullptr);
    return ret;
}

std::string Path::basename(const std::string& path)
{
    std::string ret;
    Path::split(path, nullptr, &ret);
    return ret;
}

std::wstring Path::basename(const std::wstring& path)
{
    std::wstring ret;
    Path::split(path, nullptr, &ret);
    return ret;
}

void Path::split(const std::string& path, std::string *parent_result, std::string *child_result)
{
    assert(nullptr != parent_result || nullptr != child_result);

    // 找到最后一个 '/'
    ssize_t pos = -1;
    for (ssize_t i = path.length() - 1; i >= 0; --i)
    {
        if (is_sep(path.at(i)))
        {
            pos = i;
            break;
        }
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
        // Unix root; Windows 磁盘号 + 根目录
        if (0 == pos || (2 == pos && starts_with_win_drive(path)))
#else
        // Unix root
        if (0 == pos)
#endif
            parent_result->append(path.c_str(), pos + 1);
        else
            parent_result->append(path.c_str(), pos);
    }
    if (nullptr != child_result)
        child_result->append(path.c_str() + pos + 1);
}

void Path::split(const std::wstring& path, std::wstring *parent_result, std::wstring *child_result)
{
    assert(nullptr != parent_result || nullptr != child_result);

    // 找到最后一个 '/'
    ssize_t pos = -1;
    for (ssize_t i = path.length() - 1; i >= 0; --i)
    {
        if (is_sep(path.at(i)))
        {
            pos = i;
            break;
        }
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
        // Unix root; Windows 磁盘号 + 根目录
        if (0 == pos || (2 == pos && starts_with_win_drive(path)))
#else
        // Unix root
        if (0 == pos)
#endif
            parent_result->append(path.c_str(), pos + 1);
        else
            parent_result->append(path.c_str(), pos);
    }
    if (nullptr != child_result)
        child_result->append(path.c_str() + pos + 1);
}

std::vector<std::string> Path::split_entries(const std::string &path)
{
    std::vector<std::string> result;
    std::string part;
    for (size_t i = 0, len = path.length(); i < len; ++i)
    {
        const char c = path.at(i);
        if (!is_sep(c))
        {
            part.push_back(c);
            if (i + 1 < len)
                continue;
        }

        // 处理根目录
        if (result.empty())
        {
            if (part.empty())
            {
                // Unix root
                result.push_back("/");
                continue;
            }
#if NUT_PLATFORM_OS_WINDOWS
            else if (starts_with_win_drive(part))
            {
                // Windows partition root
                part.push_back(sep());
                result.push_back(part);
                part.clear();
                continue;
            }
#endif
        }

        if (!part.empty())
        {
            result.push_back(part);
            part.clear();
        }
    }

    return result;
}

std::vector<std::wstring> Path::split_entries(const std::wstring &path)
{
    std::vector<std::wstring> result;
    std::wstring part;
    for (size_t i = 0, len = path.length(); i < len; ++i)
    {
        const wchar_t c = path.at(i);
        if (!is_sep(c))
        {
            part.push_back(c);
            if (i + 1 < len)
                continue;
        }

        // 处理根目录
        if (result.empty())
        {
            if (part.empty())
            {
                // Unix root
                result.push_back(L"/");
                continue;
            }
#if NUT_PLATFORM_OS_WINDOWS
            else if (starts_with_win_drive(part))
            {
                // Windows partition root
                part.push_back(sep());
                result.push_back(part);
                part.clear();
                continue;
            }
#endif
        }

        if (!part.empty())
        {
            result.push_back(part);
            part.clear();
        }
    }

    return result;
}

void Path::split_drive(const std::string& path, std::string *drive_result, std::string *rest_result)
{
    assert(nullptr != drive_result || nullptr != rest_result);

#if NUT_PLATFORM_OS_WINDOWS
    if (!starts_with_win_drive(path))
    {
        if (nullptr != rest_result)
            *rest_result += path;
        return;
    }

    if (nullptr != drive_result)
        drive_result->append(path.c_str(), 2);
    if (nullptr != rest_result)
        rest_result->append(path.c_str() + 2);
#else
    if (nullptr != rest_result)
        *rest_result += path;
#endif
}

void Path::split_drive(const std::wstring& path, std::wstring *drive_result, std::wstring *rest_result)
{
    assert(nullptr != drive_result || nullptr != rest_result);

#if NUT_PLATFORM_OS_WINDOWS
    if (!starts_with_win_drive(path))
    {
        if (nullptr != rest_result)
            *rest_result += path;
        return;
    }

    if (nullptr != drive_result)
        drive_result->append(path.c_str(), 2);
    if (nullptr != rest_result)
        rest_result->append(path.c_str() + 2);
#else
    if (nullptr != rest_result)
        *rest_result += path;
#endif
}

void Path::split_ext(const std::string& path, std::string *prefix_result, std::string *ext_result)
{
    assert(nullptr != prefix_result || nullptr != ext_result);

    ssize_t pos = -1;
    for (ssize_t i = path.length() - 1; i >= 0; --i)
    {
        if (is_sep(path.at(i)))
            break;
        if ('.' == path.at(i))
        {
            // 排除 "a/.abc" 的情况
            if (i - 1 >= 0 && !is_sep(path.at(i - 1)))
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
        prefix_result->append(path.c_str(), pos);
    if (nullptr != ext_result)
        ext_result->append(path.c_str() + pos);
}

void Path::split_ext(const std::wstring& path, std::wstring *prefix_result, std::wstring *ext_result)
{
    assert(nullptr != prefix_result || nullptr != ext_result);

    ssize_t pos = -1;
    for (ssize_t i = path.length() - 1; i >= 0; --i)
    {
        if (is_sep(path.at(i)))
            break;
        if (L'.' == path.at(i))
        {
            // 排除 "a/.abc" 的情况
            if (i - 1 >= 0 && !is_sep(path.at(i - 1)))
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
        prefix_result->append(path.c_str(), pos);
    if (nullptr != ext_result)
        ext_result->append(path.c_str() + pos);
}

std::string Path::join(const std::string& a, const std::string& b)
{
#if NUT_PLATFORM_OS_WINDOWS
    // 处理 windows 盘符 + 盘根路径
    // "c:/a/b" "/c/d" -> "c:/c/d"
    if (starts_with_win_drive(a) && !b.empty() && is_sep(b.at(0)))
        return a.substr(0, 2) + b;
#endif

    // 处理第二个参数是根目录的情况 '/', 'c:\'
    if (a.empty() || is_abs(b))
        return b;

    // 连接
    std::string result = a;
    if (!is_sep(*result.rbegin()))
        result.push_back(sep());
    result += b;
    return result;
}

std::wstring Path::join(const std::wstring& a, const std::wstring& b)
{
#if NUT_PLATFORM_OS_WINDOWS
    // 处理 windows 盘符 + 盘根路径
    // "c:/a/b" "/c/d" -> "c:/c/d"
    if (starts_with_win_drive(a) && !b.empty() && is_sep(b.at(0)))
        return a.substr(0, 2) + b;
#endif

    // 处理第二个参数是根目录的情况 '/', 'c:\'
    if (a.empty() || is_abs(b))
        return b;

    // 连接
    std::wstring result = a;
    if (!is_sep(*result.rbegin()))
        result.push_back(wsep());
    result += b;
    return result;
}

std::string Path::join(const std::string& a, const std::string& b, const std::string& c)
{
    return Path::join(Path::join(a, b), c);
}

std::wstring Path::join(const std::wstring& a, const std::wstring& b, const std::wstring& c)
{
    return Path::join(Path::join(a, b), c);
}

std::string Path::join(const std::string& a, const std::string& b, const std::string& c,
                       const std::string& d)
{
    return Path::join(Path::join(a, b, c), d);
}

std::wstring Path::join(const std::wstring& a, const std::wstring& b, const std::wstring& c,
                        const std::wstring& d)
{
    return Path::join(Path::join(a, b, c), d);
}

std::string Path::join(const std::string& a, const std::string& b, const std::string& c,
                       const std::string& d, const std::string& e)
{
    return Path::join(Path::join(a, b, c, d), e);
}

std::wstring Path::join(const std::wstring& a, const std::wstring& b, const std::wstring& c,
                        const std::wstring& d, const std::wstring& e)
{
    return Path::join(Path::join(a, b, c, d), e);
}

bool Path::exists(const std::string& path)
{
    const std::string fullpath = Path::abspath(path);

#if NUT_PLATFORM_OS_WINDOWS
    return -1 != ::_access(fullpath.c_str(), 0);
#else
    return 0 == ::access(fullpath.c_str(), F_OK); // F_OK 检查存在性
#endif
}

bool Path::exists(const std::wstring& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    return -1 != ::_waccess(Path::abspath(path).c_str(), 0);
#else
    return Path::exists(wstr_to_ascii(path));
#endif
}

bool Path::lexists(const std::string& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    return Path::exists(path);
#elif NUT_PLATFORM_OS_MACOS // macOS 下 faccessat() 实现不支持 AT_SYMLINK_NOFOLLOW
    // 对符号链接做特殊处理
    const std::string fullpath = Path::abspath(path);
    struct stat info;
    if (0 != ::lstat(fullpath.c_str(), &info))
        return false;
    if (S_ISLNK(info.st_mode))
        return true;
    // 非符号链接
    return 0 == ::access(fullpath.c_str(), F_OK); // F_OK 检查存在性
#else
    return 0 == ::faccessat(AT_FDCWD, Path::abspath(path).c_str(), F_OK, AT_SYMLINK_NOFOLLOW); // F_OK 检查存在性, AT_SYMLINK_NOFOLLOW 不解析符号链接
#endif
}

bool Path::lexists(const std::wstring& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    return Path::exists(path);
#else
    return Path::lexists(wstr_to_ascii(path));
#endif
}

time_t Path::get_atime(const std::string& path)
{
    const std::string fullpath = Path::abspath(path);

#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_stat(fullpath.c_str(), &info);
    return info.st_atime;
#else
    struct stat info;
    if (0 != ::stat(fullpath.c_str(), &info))
        return 0;
    return info.st_atime;
#endif
}

time_t Path::get_atime(const std::wstring& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_wstat(Path::abspath(path).c_str(), &info);
    return info.st_atime;
#else
    return Path::get_atime(wstr_to_ascii(path));
#endif
}

time_t Path::get_latime(const std::string& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    return Path::get_atime(path);
#else
    struct stat info;
    if (0 != ::lstat(Path::abspath(path).c_str(), &info))
        return 0;
    return info.st_atime;
#endif
}

time_t Path::get_latime(const std::wstring& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    return Path::get_atime(path);
#else
    return Path::get_latime(wstr_to_ascii(path));
#endif
}

time_t Path::get_mtime(const std::string& path)
{
    const std::string fullpath = Path::abspath(path);

#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_stat(fullpath.c_str(), &info);
    return info.st_mtime;
#else
    struct stat info;
    if (0 != ::stat(fullpath.c_str(), &info))
        return 0;
    return info.st_mtime;
#endif
}

time_t Path::get_mtime(const std::wstring& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_wstat(Path::abspath(path).c_str(), &info);
    return info.st_mtime;
#else
    return Path::get_mtime(wstr_to_ascii(path));
#endif
}

time_t Path::get_lmtime(const std::string& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    return Path::get_mtime(path);
#else
    struct stat info;
    if (0 != ::lstat(Path::abspath(path).c_str(), &info))
        return 0;
    return info.st_mtime;
#endif
}

time_t Path::get_lmtime(const std::wstring& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    return Path::get_mtime(path);
#else
    return Path::get_lmtime(wstr_to_ascii(path));
#endif
}

time_t Path::get_ctime(const std::string& path)
{
    const std::string fullpath = Path::abspath(path);

#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_stat(fullpath.c_str(), &info);
    return info.st_ctime;
#else
    struct stat info;
    if (0 != ::stat(fullpath.c_str(), &info))
        return 0;
    return info.st_ctime;
#endif
}

time_t Path::get_ctime(const std::wstring& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_wstat(Path::abspath(path).c_str(), &info);
    return info.st_ctime;
#else
    return Path::get_ctime(wstr_to_ascii(path));
#endif
}

time_t Path::get_lctime(const std::string& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    return Path::get_ctime(path);
#else
    struct stat info;
    if (0 != ::lstat(Path::abspath(path).c_str(), &info))
        return 0;
    return info.st_ctime;
#endif
}

time_t Path::get_lctime(const std::wstring& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    return Path::get_ctime(path);
#else
    return Path::get_lctime(wstr_to_ascii(path));
#endif
}

long long Path::get_size(const std::string& path)
{
    const std::string fullpath = Path::abspath(path);

#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_stat(fullpath.c_str(), &info);
    return info.st_size;
#else
    struct stat info;
    if (0 != ::stat(fullpath.c_str(), &info))
        return -1L;
    return info.st_size;
#endif
}

long long Path::get_size(const std::wstring& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    struct _stat info;
    ::_wstat(Path::abspath(path).c_str(), &info);
    return info.st_size;
#else
    return Path::get_size(wstr_to_ascii(path));
#endif
}

bool Path::is_link(const std::string& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    UNUSED(path);
    return false;
#else
    struct stat info;
    if (0 != ::lstat(Path::abspath(path).c_str(), &info))
        return false;
    return S_ISLNK(info.st_mode);
#endif
}

bool Path::is_link(const std::wstring& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    UNUSED(path);
    return false;
#else
    return Path::is_link(wstr_to_ascii(path));
#endif
}

bool Path::is_dir(const std::string& path)
{
    const std::string fullpath = Path::abspath(path);

#if NUT_PLATFORM_OS_WINDOWS
    return 0 != (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesA(fullpath.c_str()));
#else
    struct stat info;
    if (0 != ::stat(fullpath.c_str(), &info))
        return false;
    return S_ISDIR(info.st_mode);
#endif
}

bool Path::is_dir(const std::wstring& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    return 0 != (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesW(Path::abspath(path).c_str()));
#else
    return Path::is_dir(wstr_to_ascii(path));
#endif
}

bool Path::is_file(const std::string& path)
{
    const std::string fullpath = Path::abspath(path);

#if NUT_PLATFORM_OS_WINDOWS
    return 0 == (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesA(fullpath.c_str()));
#else
    struct stat info;
    if (0 != ::stat(fullpath.c_str(), &info))
        return false;
    return S_ISREG(info.st_mode); // regular file
#endif
}

bool Path::is_file(const std::wstring& path)
{
#if NUT_PLATFORM_OS_WINDOWS
    return 0 == (FILE_ATTRIBUTE_DIRECTORY & ::GetFileAttributesW(Path::abspath(path).c_str()));
#else
    return Path::is_file(wstr_to_ascii(path));
#endif
}

}
