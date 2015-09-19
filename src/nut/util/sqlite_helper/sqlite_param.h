
#ifndef ___HEADFILE_F597ABF0_3C94_490D_A072_5EA0331BFD83_
#define ___HEADFILE_F597ABF0_3C94_490D_A072_5EA0331BFD83_

#include <string>

#include <nut/rc/enrc.h>
#include <nut/rc/rc_new.h>

namespace nut
{

class SqliteParam
{
public:
    enum ParamType
    {
        NONE,
        INTEGER,
        STRING
    };

    ParamType type = NONE;
    rc_ptr<enrc<std::string> > string_arg;
    int int_arg = 0;

private:
    SqliteParam()
    {}

public:
    SqliteParam(int arg)
        : type(INTEGER), int_arg(arg)
    {}

    SqliteParam(const char *arg)
        : type(STRING), string_arg(rc_new<enrc<std::string> >(arg))
    {}

    SqliteParam(const std::string& arg)
        : type(STRING), string_arg(rc_new<enrc<std::string> >(arg))
    {}

    static const SqliteParam& none()
    {
        static SqliteParam ret;
        return ret;
    }
};

}

#endif
