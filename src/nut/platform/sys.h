
#ifndef ___HEADFILE_DCE3E367_44F1_4698_A7FC_FA32D04C0D4E_
#define ___HEADFILE_DCE3E367_44F1_4698_A7FC_FA32D04C0D4E_

#include "../nut_config.h"


namespace nut
{

class NUT_API Sys
{
private:
    Sys() = delete;

public:
    /**
     * 获得 CPU 核心数
     */
    static unsigned get_processor_num();
};

}

#endif
