
#ifndef ___HEADFILE_2697A272_5E2E_4738_A84E_E310ECC5BFE3_
#define ___HEADFILE_2697A272_5E2E_4738_A84E_E310ECC5BFE3_

#include <nut/rc/rc_ptr.h>

namespace nut
{

class Runnable
{
    NUT_REF_COUNTABLE

public:
    virtual ~Runnable()
    {}

    virtual void run() = 0;
};

}

#endif
