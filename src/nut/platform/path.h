
#ifndef ___HEADFILE_E6D40B10_E5D6_4092_A38B_4E69E5B8E123_
#define ___HEADFILE_E6D40B10_E5D6_4092_A38B_4E69E5B8E123_

#include <string>

namespace nut
{

class Path
{
private:
    Path(); // invalid

public:
    /**
     * 路径分隔符
     */
    static char seperator();
    static wchar_t wseperator();

    /**
     * 检查字符是否是路径分隔符 '\\' '/'
     */
    static bool is_path_separator(char c);
    static bool is_path_separator(wchar_t c);

    static void get_cwd(std::string *appended);
    static void get_cwd(std::wstring *appended);
    static std::string get_cwd();
    static std::wstring get_wcwd();

    static bool is_abs(const std::string& p);
    static bool is_abs(const std::wstring& p);

    static void abs_path(const std::string& p, std::string *appended);
    static void abs_path(const std::wstring& p, std::wstring *appended);
    static std::string abs_path(const std::string& p);
    static std::wstring abs_path(const std::wstring& p);

    // TODO static std::string relative_path() {}
    // TODO static std::string real_path() {}

    /**
     * 从路径中划分出父路径和 文件/文件夹 名
     *
     * 例如：
     * "ab/c/d" -> "ab/c" "d"
     * "/ab.txt" -> "/" "ab.txt"
     * "c:\\tmp" -> "c:\\" "tmp"
     */
    static void split(const std::string &path, std::string *parent_appended, std::string *child_appended);
    static void split(const std::wstring& path, std::wstring *parent_appended, std::wstring *child_appended);

    /**
     * 从路径中划分出磁盘号和路径(linux路径的磁盘号假定为"")
     *
     * 例如：
     * "c:\\mn\\p" -> "c:" "\\mn\\p"
     * "/mnt/sdcard" -> "" "/mnt/sdcard"
     */
    static void split_drive(const std::string& path, std::string *drive_appended, std::string *rest_appended);
    static void split_drive(const std::wstring& path, std::wstring *drive_appended, std::wstring *rest_appended);

    /**
     * 从路径或者文件名中分离出后缀名
     *
     * 例如：
     * "a.txt" -> "a" ".txt"
     */
    static void split_ext(const std::string& path, std::string *prefix_appended, std::string *ext_appended);
    static void split_ext(const std::wstring& path, std::wstring *prefix_appended, std::wstring *ext_appended);

    // TODO static void splitunc() {}

    /**
     * 检查路径存在性(不解析符号链接)
     */
    static bool lexists(const char *path);
    static bool lexists(const std::string& path);
    static bool lexists(const wchar_t *path);
    static bool lexists(const std::wstring& path);

    /**
     * 检查路径存在性(解析符号链接)
     */
    static bool exists(const char *path);
    static bool exists(const std::string& path);
    static bool exists(const wchar_t *path);
    static bool exists(const std::wstring& path);

    /**
     * last access time
     */
    static time_t get_atime(const char *path);
    static time_t get_atime(const std::string& path);
    static time_t get_atime(const wchar_t *path);
    static time_t get_atime(const std::wstring& path);

    /**
     * last modified time
     */
    static time_t get_mtime(const char *path);
    static time_t get_mtime(const std::string& path);
    static time_t get_mtime(const wchar_t *path);
    static time_t get_mtime(const std::wstring& path);

    /**
     * created time
     */
    static time_t get_ctime(const char *path);
    static time_t get_ctime(const std::string& path);
    static time_t get_ctime(const wchar_t *path);
    static time_t get_ctime(const std::wstring& path);

    /**
     * 获取文件大小
     */
    static long get_size(const char *path);
    static long get_size(const std::string& path);
    static long get_size(const wchar_t *path);
    static long get_size(const std::wstring& path);

    static bool is_dir(const char *path);
    static bool is_dir(const std::string& path);
    static bool is_dir(const wchar_t *path);
    static bool is_dir(const std::wstring& path);

    static bool is_file(const char *path);
    static bool is_file(const std::string& path);
    static bool is_file(const wchar_t *path);
    static bool is_file(const std::wstring& path);

    static bool is_link(const char *path);
    static bool is_link(const std::string& path);
    static bool is_link(const wchar_t *path);
    static bool is_link(const std::wstring& path);

    // TODO static bool ismount() {}

    /**
     * 连接两个子路径
     *
     * 例如：
     * "a" "b" -> "a/b"
     * "/" "sd" -> "/sd"
     * "c:" "\\tmp" -> "c:\\tmp"
     */
    static void join(const std::string& a, const std::string& b, std::string *appended);
    static void join(const std::wstring& a, const std::wstring& b, std::wstring *appended);

    static std::string join(const std::string& a, const std::string& b);
    static std::wstring join(const std::wstring& a, const std::wstring& b);
    static std::string join(const std::string& a, const std::string& b, const std::string& c);
    static std::wstring join(const std::wstring& a, const std::wstring& b, const std::wstring& c);
    static std::string join(const std::string& a, const std::string& b, const std::string& c, const std::string& d);
    static std::wstring join(const std::wstring& a, const std::wstring& b, const std::wstring& c, const std::wstring& d);
};

}

#endif
