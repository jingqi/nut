
#include <assert.h>
#include <fstream>

#include "../../platform/path.h"
#include "text_file.h"


namespace nut
{

std::string TextFile::read_file(const std::string& path)
{
    std::string ret;
    if (!Path::exists(path))
        return ret;

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
            ret.push_back('\n');
        ret += line;
        first_line = false;
    }
    ifs.close();
    return ret;
}

bool TextFile::write_file(const std::string& path, const std::string& content)
{
    std::ofstream ofs(path);
    ofs << content;
    ofs.flush();
    ofs.close();
    return true;
}

}
