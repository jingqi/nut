﻿
#ifndef ___HEADFILE_22ABA5CE_8E56_4F32_A48F_78AEC1A0A8C5_
#define ___HEADFILE_22ABA5CE_8E56_4F32_A48F_78AEC1A0A8C5_

#include <stdint.h>

#include "../../nut_config.h"


namespace nut
{

/**
 * CRC-32
 */
class NUT_API CRC32
{
public:
    void reset();

    void update(uint8_t byte);
    void update(const void *data, size_t cb);

    uint32_t get_result() const;

private:
    uint32_t _crc32 = 0xffffffff;
};

}

#endif
