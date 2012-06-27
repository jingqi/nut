/**
 * @file -
 * @author jingqi
 * @date 2012-06-23
 * @last-edit 2012-06-23 15:41:27 jingqi
 */

#ifndef ___HEADFILE_291DFB4C_7D29_4D61_A691_EF83FB86CD36_
#define ___HEADFILE_291DFB4C_7D29_4D61_A691_EF83FB86CD36_

#include <assert.h>
#include <string>
#include <vector>
#include <stdio.h> // for sprintf()

#include "platform.hpp"

#if defined(NUT_PLATFORM_OS_WINDOWS)
#   include <windows.h>
#   include <io.h>    // for mkdir()
#else
#   include <dirent.h>  // for DIR, dirent
#   include <limits.h>   // for PATH_MAX
#   include <sys/stat.h> // for lstat()
#   include <sys/types.h>  // for mkdir()
#endif

namespace nut
{

class OS
{
private:
    OS() {}

public:

    /**
     * 枚举目录下的文件/文件夹
     *
     * @param except_file 如果传入true, 则返回值不会包含文件
     * @parma except_dir 如果传入true, 则返回值不会包含文件夹
     * @parma except_initial_dot 如果传入true, 则返回值不会包含以'.'开头的文件/文件夹
     */
    static std::vector<std::string> listdir(const char *path, bool except_file = false,
            bool except_dir = false, bool except_initial_dot = false)
    {
        assert(NULL != path);
        std::vector<std::string> ret;

#if defined(NUT_PLATFORM_OS_WINDOWS)
        char search_path[MAX_PATH];
        ::sprintf(search_path, "%s\\*", path); /* 加上通配符 */

        WIN32_FIND_DATAA wfd;
        HANDLE hFind = ::FindFirstFileA(search_path, &wfd);
        if (hFind == INVALID_HANDLE_VALUE)
            return ret;

        do
        {
            if (except_initial_dot && wfd.cFileName[0] == '.')
                continue;
            if (except_file && !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                continue;
            if (except_dir && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                continue;

            ret.push_back(wfd.cFileName);
        } while (::FindNextFileA(hFind, &wfd));

        // 关闭查找句柄
        ::FindClose(hFind);
        return ret;
#else
        DIR *dp = NULL;
        struct dirent *dirp = NULL;
        if ((dp = ::opendir(path)) == NULL)
            return ret;

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

            ret.push_back(dirp->d_name);
        }

        // 释放DIR (struct dirent是由DIR维护的，无需额外释放)
        ::closedir(dp);
        return ret;
#endif
    }

    /**
     * 复制文件
     */
    static bool copyfile(const char *src, const char *dest)
    {
        assert(NULL != src && NULL != dest);

#if defined(NUT_PLATFORM_OS_WINDOWS)
        return FALSE != ::CopyFileA(src, dest, TRUE);
#else
        FILE *inFile = ::fopen(src, "rb");
        if (NULL == inFile)
            return false;
            
        FILE *outFile = ::fopen(dest, "wb+");
        if (NULL == outFile)
        {
            ::fclose(inFIle);
            return false;
        }
        
        const int BUF_LEN = 4096;
        char buf[BUF_LEN];
        int readed = -1;
        while ((readed = ::fread(buf, BUF_LEN, 1, inFile)) > 0)
        {
            ::fwrite(buf, readed, 1, outFile);
        }
        ::fclose(inFile);
        ::fclose(outFile);
        return true;
#endif
    }

    static bool removefile(const char *path)
    {
        assert(NULL != path);
        return -1 != ::remove(path);
    }

    static bool mkdir(const char *path)
    {
        assert(NULL != path);
#if defined(NUT_PLATFORM_OS_WINDOWS)
        return 0 == ::mkdir(path);
#else
        return 0 == ::mkdir(dir.c_str(), S_IWRITE);
#endif
    }
};

}

#endif

