
#ifndef ___HEADFILE___AD10542C_56CB_4BD3_B17A_1355A3FC0EF6_
#define ___HEADFILE___AD10542C_56CB_4BD3_B17A_1355A3FC0EF6_

#include <stdlib.h> // for NULL

namespace nut
{

/**
 * 单元测试失败时所抛出的异常
 */
class TestCaseFailureException
{
    const char *_description = NULL;
    const char *_file = NULL;
    int _line = -1;

public:
    TestCaseFailureException(const char *des, const char *file, int line)
        : _description(des), _file(file), _line(line)
    {}

    const char* get_description() const
    {
        return _description;
    }

    const char* get_file() const
    {
        return _file;
    }

    int get_line() const
    {
        return _line;
    }
};

}

#endif /* head file guarder */
