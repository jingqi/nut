
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
    long m_code;                 // 错误码
    StringT m_message;           // 错误附带信息
    const char *m_source_file;   // 源代码文件
    int m_source_line;           // 源代码行数
    const char *m_func;          // 源代码函数

public :
    /**
     * @param cd 错误码
     * @param msg 错误信息
     * @param sf 源代码文件， __FILE__
     * @param sl 源代码行数， __LINE__
     * @param fc 源代码函数， __FUNCTION__
     */
    ExceptionBase(long cd, StringT msg, const char *sf = NULL, int sl = -1, const char *fc = NULL)
        : m_code(cd), m_message(msg), m_source_file(sf), m_source_line(sl), m_func(fc)
    {}

    virtual ~ExceptionBase() throw()
    {}

    /// 获取错误码
    long get_code() const
    {
        return m_code;
    }

    /// 获取出错信息
    StringT get_message() const
    {
        return m_message;
    }

    /// 获取源文件名
    const char* get_source_file() const
    {
        if (NULL == m_source_file)
            return NULL;

        const char *ret = m_source_file;
        for (const char *i = m_source_file; '\0' != *i; ++i)
        {
            if ('\\' == *i || '/' == *i)
                ret = i + 1;
        }
        return ret;
    }

    /// 获取源文件路径
    const char* get_source_path() const
    {
        return m_source_file;
    }

    /// 获取源文件行数
    int get_source_line() const
    {
        return m_source_line;
    }

    /// 获取源文件函数名
    const char* get_source_func() const
    {
        return m_func;
    }
};

typedef ExceptionBase<std::string> ExceptionA;
typedef ExceptionBase<std::wstring> ExceptionW;
typedef ExceptionA Exception;

}

#if !defined(THROW_ON_FALSE)
#   define THROW_ON_FALSE(e,c) do { if (!(e)) throw nut::ExceptionA((c), (#e), __FILE__, __LINE__, __FUNCTION__); } while (false)
#endif

#endif /* head file guarder */
