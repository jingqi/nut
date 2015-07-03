
#ifndef ___HEADFILE_291DFB4C_7D29_4D61_A691_EF83FB86CD36_
#define ___HEADFILE_291DFB4C_7D29_4D61_A691_EF83FB86CD36_

#include <string>
#include <vector>

namespace nut
{

class OS
{
private:
    OS(); // invalid

public:

    /**
     * 枚举目录下的文件/文件夹
     *
     * @param except_file 如果传入true, 则返回值不会包含文件
     * @parma except_dir 如果传入true, 则返回值不会包含文件夹
     * @parma except_initial_dot 如果传入true, 则返回值不会包含以'.'开头的文件/文件夹
     */
    static void list_dir(const char *path, std::vector<std::string> *appended, bool except_file = false,
            bool except_dir = false, bool except_initial_dot = false);
    static void list_dir(const wchar_t *path, std::vector<std::wstring> *appended, bool except_file = false,
        bool except_dir = false, bool except_initial_dot = false);

    /**
     * 复制文件
     */
    static bool copy_file(const char *src, const char *dest);
    static bool copy_file(const std::string& src, const std::string& dest);
    static bool copy_file(const wchar_t *src, const wchar_t *dest);
    static bool copy_file(const std::wstring& src, const std::wstring& dest);

    /**
     * 删除文件
     */
    static bool remove_file(const char *path);
    static bool remove_file(const std::string& path);
    static bool remove_file(const wchar_t *path);
    static bool remove_file(const std::wstring& path);

    /**
     * 创建目录
     */
    static bool mkdir(const char *path);
    static bool mkdir(const std::string& path);
    static bool mkdir(const wchar_t *path);
    static bool mkdir(const std::wstring& path);

    /**
     * 删除空目录
     */
    static bool rmdir(const char *path);
    static bool rmdir(const std::string& path);
    static bool rmdir(const wchar_t *path);
    static bool rmdir(const std::wstring& path);

    /**
     * 删除目录树
     */
    static bool remove_tree(const char *path);
    static bool remove_tree(const std::string& path);
    static bool remove_tree(const wchar_t *path);
    static bool remove_tree(const std::wstring& path);
};

}

#endif
