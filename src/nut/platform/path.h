
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

    static bool is_abs(const char *path);
    static bool is_abs(const wchar_t *path);

    static void abs_path(const char *path, std::string *appended);
    static void abs_path(const wchar_t *path, std::wstring *appended);

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
    static void split(const char *path, std::string *parent_appended, std::string *child_appended);
    static void split(const wchar_t *path, std::wstring *parent_appended, std::wstring *child_appended);

    /**
     * 从路径中划分出磁盘号和路径(linux路径的磁盘号假定为"")
     *
     * 例如：
     * "c:\\mn\\p" -> "c:" "\\mn\\p"
     * "/mnt/sdcard" -> "" "/mnt/sdcard"
     */
    static void split_drive(const char *path, std::string *drive_appended, std::string *rest_appended);
    static void split_drive(const wchar_t *path, std::wstring *drive_appended, std::wstring *rest_appended);

    /**
     * 从路径或者文件名中分离出后缀名
     *
     * 例如：
     * "a.txt" -> "a" ".txt"
     */
    static void split_ext(const char *path, std::string *prefix_appended, std::string *ext_appended);
    static void split_ext(const wchar_t *path, std::wstring *prefix_appended, std::wstring *ext_appended);

    // TODO static void splitunc() {}

    /**
     * 检查路径存在性(不解析符号链接)
     */
    static bool lexists(const char *path);
    static bool lexists(const wchar_t *path);

    /**
     * 检查路径存在性(解析符号链接)
     */
    static bool exists(const char *path);
    static bool exists(const wchar_t *path);

    /**
     * last access time
     */
    static time_t get_atime(const char *path);
    static time_t get_atime(const wchar_t *path);

    /**
     * last modified time
     */
    static time_t get_mtime(const char *path);
    static time_t get_mtime(const wchar_t *path);

    /**
     * created time
     */
    static time_t get_ctime(const char *path);
    static time_t get_ctime(const wchar_t *path);

    /**
     * 获取文件大小
     */
    static long long get_size(const char *path);
    static long long get_size(const wchar_t *path);

    static bool is_dir(const char *path);
    static bool is_dir(const wchar_t *path);

    static bool is_file(const char *path);
    static bool is_file(const wchar_t *path);

    static bool is_link(const char *path);
    static bool is_link(const wchar_t *path);

    // TODO static bool ismount() {}

    /**
     * 连接两个子路径
     *
     * 例如：
     * "a" "b" -> "a/b"
     * "/" "sd" -> "/sd"
     * "c:" "\\tmp" -> "c:\\tmp"
     */
    static void join(const char *a, const char *b, std::string *appended);
    static void join(const wchar_t *a, const wchar_t *b, std::wstring *appended);

    static void join(const char *a, const char *b, const char *c, std::string *appended);
    static void join(const wchar_t *a, const wchar_t *b, const wchar_t *c, std::wstring *appended);

    static void join(const char *a, const char *b, const char *c, const char *d, std::string *appended);
    static void join(const wchar_t *a, const wchar_t *b, const wchar_t *c, const wchar_t *d, std::wstring *appended);
};

}

#endif
