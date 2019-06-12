
#ifndef ___HEADFILE_DCE3E367_44F1_4698_A7FC_FA32D04C0D4E_
#define ___HEADFILE_DCE3E367_44F1_4698_A7FC_FA32D04C0D4E_

#include <random>

#include "../nut_config.h"


namespace nut
{

class NUT_API Sys
{
public:
    /**
     * 获得 CPU 核心数
     * 1. 这里指系统配置的可用核心数
     * 2. 对于某些 CPU 使用虚拟核心，则返回虚拟核心数
     */
    static unsigned get_processor_num() noexcept;

    /**
     * 一般用途的随机数引擎
     */
    static std::mt19937_64& random_engine() noexcept;

private:
    Sys() = delete;
};

}

#endif
