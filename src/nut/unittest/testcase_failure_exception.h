
#ifndef ___HEADFILE___AD10542C_56CB_4BD3_B17A_1355A3FC0EF6_
#define ___HEADFILE___AD10542C_56CB_4BD3_B17A_1355A3FC0EF6_

namespace nut
{

/**
 * 单元测试失败时所抛出的异常
 */
class TestCaseFailureException
{
    const char *m_description;
    const char *m_file;
    int m_line;

public:
    TestCaseFailureException(const char *des, const char *file, int line)
        : m_description(des), m_file(file), m_line(line)
    {}

    const char* get_description() const
    {
        return m_description;
    }

    const char* get_file() const
    {
        return m_file;
    }

    int get_line() const
    {
        return m_line;
    }
};

}

#endif /* head file guarder */
