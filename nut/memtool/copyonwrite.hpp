/**
 * @file -
 * @author jingqi
 * @date 2013-03-24
 * @last-edit 2013-03-24 17:01:29 jingqi
 * @brief
 */

#ifndef ___HEADFILE_5A46584A_7E82_49AB_A7DE_9C1525091EA7_
#define ___HEADFILE_5A46584A_7E82_49AB_A7DE_9C1525091EA7_

#include <nut/gc/gc.hpp>

namespace nut
{

/**
 * 用于 copy-on-write
 */
template <typename T>
struct FixedBuf
{
    NUT_GC_REFERABLE

    T * buf;
    size_t len;

    FixedBuf(size_t _len)
        : buf(0 == _len ? NULL : (T*) ::malloc(_len * sizeof(T))), len(_len)
    {}

    ~FixedBuf()
    {
        if (NULL != buf)
            ::free(buf);
    }

    void realloc(size_t _len)
    {
        assert(get_ref() == 1 && _len > 0);
        buf = (T*) ::realloc(buf, _len * sizeof(T));
        len = _len;
    }
};

}

#endif
