
#ifndef ___HEADFILE___1767487A_1A0E_11E0_A9DB_485B391122F8_
#define ___HEADFILE___1767487A_1A0E_11E0_A9DB_485B391122F8_

#include <string>
#include <exception>


namespace nut
{

/**
 * 带源文件名等调试信息的异常类型
 * @param CharT 错误信息的字符格式, 可以是 char、wchar_t 等
 */
template <typename StringT>
class ExceptionBase : public std::exception
{
public:
    /**
     * @param code 错误码
     * @param msg 错误信息
     * @param file 源代码文件， __FILE__
     * @param line 源代码行数， __LINE__
     * @param func 源代码函数， __FUNCTION__
     */
    constexpr ExceptionBase(long code, StringT msg, const char *file = nullptr,
                            int line = -1, const char *func = nullptr)
        : _code(code), _message(msg), _source_file(file), _source_line(line),
          _function(func)
    {}

    virtual ~ExceptionBase() throw()
    {}

    // 获取错误码
    constexpr long get_code() const
    {
        return _code;
    }

    // 获取出错信息
    constexpr const StringT& get_message() const
    {
        return _message;
    }

    // 获取源文件名
    const char* get_source_file() const
    {
        if (nullptr == _source_file)
            return nullptr;

        const char *ret = _source_file;
        for (const char *i = _source_file; '\0' != *i; ++i)
        {
            if ('\\' == *i || '/' == *i)
                ret = i + 1;
        }
        return ret;
    }

    // 获取源文件路径
    constexpr const char* get_source_path() const
    {
        return _source_file;
    }

    // 获取源文件行数
    constexpr int get_source_line() const
    {
        return _source_line;
    }

    // 获取源文件函数名
    constexpr const char* get_source_func() const
    {
        return _function;
    }

private:
    long _code = 0; // 错误码
    StringT _message; // 错误附带信息
    const char *_source_file = nullptr; // 源代码文件
    int _source_line = -1; // 源代码行数
    const char *_function = nullptr; // 源代码函数
};

typedef ExceptionBase<std::string> ExceptionA;
typedef ExceptionBase<std::wstring> ExceptionW;
typedef ExceptionA Exception;

}

#if !defined(THROW_ON_FALSE)
#   define THROW_ON_FALSE(e,c) do { if (!(e)) throw nut::ExceptionA((c), (#e), __FILE__, __LINE__, __FUNCTION__); } while (false)
#endif

#endif /* head file guarder */
