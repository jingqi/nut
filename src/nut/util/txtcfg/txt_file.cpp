
#include <assert.h>
#include <fstream>

#include <nut/platform/path.h>

#include "txt_file.h"

namespace nut
{

bool TxtFile::read_file(const char *path, std::string *rs)
{
    assert(nullptr != path && nullptr != rs);
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

bool TxtFile::write_file(const char *path, const std::string& content)
{
    std::ofstream ofs(path);
    ofs << content;
    ofs.flush();
    ofs.close();
    return true;
}

}
