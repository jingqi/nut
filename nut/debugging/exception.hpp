/**
 * @file -
 * @author jingqi
 * @date 2011-01-06
 */

#ifndef ___HEADFILE___1767487A_1A0E_11E0_A9DB_485B391122F8_
#define ___HEADFILE___1767487A_1A0E_11E0_A9DB_485B391122F8_

namespace nut
{
    
/**
 * 带源文件名等调试信息的异常类型
 * @param CharT 错误信息的字符格式, 可以是 char、wchar_t 等
 */
template <typename CharT>
class ExceptionBase
{
    const char *m_sourceFile;    // 源代码文件
    int m_sourceLine;            // 源代码行数
    const char *m_func;          // 源代码函数
    long m_code;                 // 错误码
    const CharT *m_message;      // 错误附带信息

public :
    /**
     * @param cd 错误码
     * @param msg 错误信息
     * @param sf 源代码文件， __FILE__
     * @param sl 源代码行数， __LINE__
     * @param fc 源代码函数， __FUNCTION__
     */
    ExceptionBase(long cd, const CharT *msg = NULL, const char *sf = NULL, int sl = -1, const char *fc = NULL)
        : m_code(cd), m_message(msg), m_sourceFile(sf), m_sourceLine(sl), m_func(fc)
    {}

    /// 获取错误码
    long getCode() const { return m_code; }

    /// 获取出错信息
    const CharT* getMessage() const { return m_message; }

    /// 获取源文件名
    const char* getSourceFile() const
    {
        if (NULL == m_sourceFile)
            return NULL;

        const char *ret = m_sourceFile;
        for (const char *i = m_sourceFile; '\0' != *i; ++i)
        {
            if ('\\' == *i || '/' == *i)
                ret = i + 1;
        }
        return ret;
    }

    /// 获取源文件路径
    const char* getSourcePath() const { return m_sourceFile; }

    /// 获取源文件行数
    int getSourceLine() const { return m_sourceLine; }

    /// 获取源文件函数名
    const char* getSourceFunc() const { return m_func; }
};

typedef ExceptionBase<char> ExceptionA;
typedef ExceptionBase<wchar_t> ExceptionW;
typedef ExceptionA Exception;

}

#if !defined(THROW_ON_FALSE)
#   define THROW_ON_FALSE(e,c) do { if (!(e)) throw nut::ExceptionA((c), (#e), __FILE__, __LINE__, __FUNCTION__); } while (false)
#endif

#endif /* head file guarder */

