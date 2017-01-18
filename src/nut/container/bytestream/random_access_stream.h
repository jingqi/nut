
#ifndef ___HEADFILE_FB24F7BC_D536_4A9E_AF29_78920D9ADD53_
#define ___HEADFILE_FB24F7BC_D536_4A9E_AF29_78920D9ADD53_

#include <stddef.h> // For size_t and so on

#include <nut/rc/rc_ptr.h>

#include "../../nut_config.h"


namespace nut
{

class NUT_API RandomAccessStream
{
    NUT_REF_COUNTABLE

public:
    /**
     * 总长度
     */
    virtual size_t size() const = 0;

    /**
     * 读/写位置
     */
    virtual size_t tell() const = 0;

    /**
     * 设置读/写位置
     */
    virtual void seek(size_t pos) = 0;
};

}

#endif
