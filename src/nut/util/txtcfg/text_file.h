
#ifndef ___HEADFILE_94887FC7_D32F_4E34_AAAC_DE01BE2089C8_
#define ___HEADFILE_94887FC7_D32F_4E34_AAAC_DE01BE2089C8_

#include <string>

#include "../../nut_config.h"


namespace nut
{

class NUT_API TextFile
{
public:
    static std::string read_file(const std::string& path) noexcept;
    static bool write_file(const std::string& path, const std::string& content) noexcept;

private:
    TextFile() = delete;
};

}

#endif
