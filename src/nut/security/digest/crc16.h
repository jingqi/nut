
#ifndef ___HEADFILE_D987B8A3_F316_40EE_AB40_F97F406915AD_
#define ___HEADFILE_D987B8A3_F316_40EE_AB40_F97F406915AD_

#include <stdint.h>
#include <stddef.h> // for size_t

#include "../../nut_config.h"


namespace nut
{

/**
 * CRC-16/XMODEM
 */
class NUT_API CRC16
{
public:
    void reset();

    void update(uint8_t byte);
    void update(const void *data, size_t cb);

    uint16_t get_result() const;

private:
    uint16_t _crc16 = 0;
};

}

#endif
