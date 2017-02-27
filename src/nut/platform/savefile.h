
#ifndef ___HEADFILE_2CF707F0_54CC_4B40_AF2C_9760D1864CBC_
#define ___HEADFILE_2CF707F0_54CC_4B40_AF2C_9760D1864CBC_

#include <string>


namespace nut
{

/**
 * 安全的保存文件，避免崩溃时丢失原始文件数据
 */
class SaveFile
{
    std::string _path, _tmp_path;
    int _fd = -1;

public:
    SaveFile(const std::string& path);
    ~SaveFile();

    bool open();
    bool write(const void *buf, size_t len);
    void cancel();
    bool commit();
};

}

#endif
