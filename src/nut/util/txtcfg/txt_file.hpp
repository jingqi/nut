/**
 * @file -
 * @author jingqi
 * @date 2014-07-30
 * @last-edit 2014-09-13 19:37:39 jingqi
 * @brief
 */

#ifndef ___HEADFILE_94887FC7_D32F_4E34_AAAC_DE01BE2089C8_
#define ___HEADFILE_94887FC7_D32F_4E34_AAAC_DE01BE2089C8_

#include <assert.h>
#include <fstream>
#include <string>

#include <nut/platform/path.hpp>

namespace nut
{

class TxtFile
{
    // invalid methods
    TxtFile();

public:
    static bool read_file(const char *path, std::string *rs)
    {
        assert(NULL != path && NULL != rs);
        if (!Path::exists(path))
            return false;

        std::ifstream ifs(path);
        std::string line;
        bool first_line = true;
        while (getline(ifs, line))
        {
            // 兼容文件做跨平台拷贝时的换行符问题
            if (line.length() > 0)
            {
                const char c = line.at(line.length() - 1);
                if ('\r' == c || '\n' == c)
                    line.erase(line.length() - 1);
            }

            if (!first_line)
                rs->push_back('\n');
            *rs += line;
            first_line = false;
        }
        ifs.close();
        return true;
    }

    static bool write_file(const char *path, const std::string& content)
    {
        std::ofstream ofs(path);
        ofs << content;
        ofs.flush();
        ofs.close();
        return true;
    }
};

}

#endif
