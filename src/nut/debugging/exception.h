
#ifndef ___HEADFILE___1767487A_1A0E_11E0_A9DB_485B391122F8_
#define ___HEADFILE___1767487A_1A0E_11E0_A9DB_485B391122F8_

#include <string>
#include <exception>

#include "../nut_config.h"


namespace nut
{

/**
 * 带源文件名等调试信息的异常类型
 */
class NUT_API Exception : public std::exception
{
public:
    /**
     * @param code 错误码
     * @param msg 错误信息
     * @param file 源代码文件， __FILE__
     * @param line 源代码行数， __LINE__
     * @param func 源代码函数， __FUNCTION__
     */
    Exception(long code, std::string&& msg, const char *file = nullptr,
              int line = -1, const char *func = nullptr) noexcept;

    Exception(long code, const std::string& msg, const char *file = nullptr,
              int line = -1, const char *func = nullptr) noexcept;

    virtual ~Exception() noexcept override = default;

    virtual const char* what() const noexcept override;

    // 获取错误码
    long get_code() const noexcept;

    // 获取出错信息
    const std::string& get_message() const noexcept;

    // 获取源文件名
    const char* get_source_file() const noexcept;

    // 获取源文件路径
    const char* get_source_path() const noexcept;

    // 获取源文件行数
    int get_source_line() const noexcept;

    // 获取源文件函数名
    const char* get_source_func() const noexcept;

private:
    long _code = 0; // 错误码
    std::string _message; // 错误附带信息
    const char *_source_path = nullptr; // 源代码文件
    int _source_line = -1; // 源代码行数
    const char *_function = nullptr; // 源代码函数
};

}

#if !defined(THROW_ON_FALSE)
#   define THROW_ON_FALSE(e,c) \
    do { if (!(e)) throw nut::Exception((c), (#e), __FILE__, __LINE__, __FUNCTION__); } while (false)
#endif

#endif /* head file guarder */
