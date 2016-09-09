
#ifndef ___HEADFILE_FB24F7BC_D536_4A9E_AF29_78920D9ADD53_
#define ___HEADFILE_FB24F7BC_D536_4A9E_AF29_78920D9ADD53_

#include "input_stream.h"

namespace nut
{

class NUT_API RandomAccessInputStream : public InputStream
{
public:
    virtual size_t tell() const = 0;

    virtual size_t length() const = 0;

    virtual void seek(size_t pos) = 0;
};

}

#endif
