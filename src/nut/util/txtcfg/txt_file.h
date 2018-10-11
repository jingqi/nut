
#ifndef ___HEADFILE_94887FC7_D32F_4E34_AAAC_DE01BE2089C8_
#define ___HEADFILE_94887FC7_D32F_4E34_AAAC_DE01BE2089C8_

#include <string>

#include "../../nut_config.h"


namespace nut
{

class NUT_API TxtFile
{
public:
    static bool read_file(const char *path, std::string *rs);
    static bool write_file(const char *path, const std::string& content);

private:
    TxtFile() = delete;
};

}

#endif
