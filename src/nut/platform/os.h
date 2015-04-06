
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
    static void listdir(const char *path, std::vector<std::string> *appended, bool except_file = false,
            bool except_dir = false, bool except_initial_dot = false);
    static void listdir(const wchar_t *path, std::vector<std::wstring> *appended, bool except_file = false,
        bool except_dir = false, bool except_initial_dot = false);

    /**
     * 复制文件
     */
    static bool copyfile(const char *src, const char *dest);
    static bool copyfile(const std::string& src, const std::string& dest);
    static bool copyfile(const wchar_t *src, const wchar_t *dest);
    static bool copyfile(const std::wstring& src, const std::wstring& dest);

    static bool removefile(const char *path);
    static bool removefile(const std::string& path);
    static bool removefile(const wchar_t *path);
    static bool removefile(const std::wstring& path);

    static bool mkdir(const char *path);
    static bool mkdir(const std::string& path);
    static bool mkdir(const wchar_t *path);
    static bool mkdir(const std::wstring& path);

    /**
     * 删除空目录
     */
    static bool removedir(const char *path);
    static bool removedir(const std::string& path);
    static bool removedir(const wchar_t *path);
    static bool removedir(const std::wstring& path);

    /**
     * 删除目录树
     */
    static bool removetree(const char *path);
    static bool removetree(const std::string& path);
    static bool removetree(const wchar_t *path);
    static bool removetree(const std::wstring& path);
};

}

#endif
