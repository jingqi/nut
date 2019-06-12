
#ifndef ___HEADFILE___AD10542C_56CB_4BD3_B17A_1355A3FC0EF6_
#define ___HEADFILE___AD10542C_56CB_4BD3_B17A_1355A3FC0EF6_

#include <exception>
#include <string>

#include "../nut_config.h"


namespace nut
{

/**
 * 单元测试失败时所抛出的异常
 */
class NUT_API TestCaseFailureException : public std::exception
{
public:
    TestCaseFailureException(std::string&& msg, const char *file, int line) noexcept;
    TestCaseFailureException(const std::string& msg, const char *file, int line) noexcept;

    virtual ~TestCaseFailureException() noexcept override = default;

    virtual const char* what() const noexcept override;

    const std::string& get_message() const noexcept;
    const char* get_source_file() const noexcept;
    const char* get_source_path() const noexcept;
    int get_source_line() const noexcept;

private:
    std::string _message;
    const char *_source_path = nullptr;
    int _source_line = -1;
};

}

#endif /* head file guarder */
