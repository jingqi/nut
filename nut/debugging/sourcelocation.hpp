/**
 * @file -
 * @author jingqi
 * @date 2010-4-24
 */

#ifndef ___HEADFILE___2BD47B8B_B18E_421F_8A9D_DC72BAF49ABF_
#define ___HEADFILE___2BD47B8B_B18E_421F_8A9D_DC72BAF49ABF_

#include <assert.h>
#include <string.h> /* for strcmp() */
#include <stdio.h> /* for sprintf() */
#include <string>

namespace nut
{

class SourceLocation
{
    const char *m_path;    // source file path
    int m_line;            // source file line
    const char *m_func;    // source function
    char m_buf[20];        // source file line string

public :
    SourceLocation (const char *file, int line, const char *func)
        : m_path(file), m_line(line), m_func(func)
    {
        assert(NULL != file && line >= 0 && NULL != func);
        sprintf(m_buf,"%d",line);
        m_buf[19] = 0;
    }

    bool operator== (const SourceLocation& x) const
    {
        assert(NULL != m_path && NULL != m_func);
        assert(NULL != x.m_path && NULL != x.m_func);
        return m_line == x.m_line && 0 == strcmp(m_path, x.m_path) && 0 == strcmp(m_func, x.m_func);
    }

    bool operator!= (const SourceLocation& x) const
    {
        return !(*this == x);
    }

    const char* getFilePath () const { return m_path; }

    const char* getFileName () const
    {
        assert(NULL != m_path);
        const char *ret = m_path;
        for (const char *tmp = m_path; '\0' != *tmp; ++tmp)
        {
            if ('\\' == *tmp || '/' == *tmp)
                ret = tmp + 1;
        }
        return ret;
    }

    int getLineNumber () const { return m_line; }

    const char* getLineNumberStr () const { return m_buf; }

    /**
     * @return this method may return NULL if the souce is not in a function
     */
    const char* getFunctionName () const { return m_func; }

    std::string toString () const
    {
        std::string ret = std::string(getFileName()) + ":" + m_buf;
        if (m_func != NULL)
            ret += std::string(" ") + m_func + "()";

        return ret;
    }
};

}

/** used to build a SourceLocation instance */
#define NUT_SOURCE_LOCATION       nut::SourceLocation(__FILE__,__LINE__,__FUNCTION__)

/** used to initilize a SourceLocation instance */
#define NUT_SOURCE_LOCATION_ARGS  __FILE__,__LINE__,__FUNCTION__


#endif /* head file guarder */

