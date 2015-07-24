
#ifndef ___HEADFILE___2BD47B8B_B18E_421F_8A9D_DC72BAF49ABF_
#define ___HEADFILE___2BD47B8B_B18E_421F_8A9D_DC72BAF49ABF_

#include <assert.h>
#include <string>

namespace nut
{

/**
 * C/C++ 源码位置
 */
class SourceLocation
{
    const char *_path = NULL;    // source file path
    int _line = -1;            // source file line
    const char *_func = NULL;    // source function

public :
    SourceLocation(const char *file, int line, const char *func)
        : _path(file), _line(line), _func(func)
    {
        assert(NULL != file && line >= 0);
    }

    bool operator==(const SourceLocation& x) const;

    bool operator!=(const SourceLocation& x) const
    {
        return !(*this == x);
    }

    const char* get_file_path() const
    {
        return _path;
    }

    const char* get_file_name() const;

    int get_line_number() const
    {
        return _line;
    }

    /**
     * @return This method may return NULL if the location is not in any function
     */
    const char* get_function_name() const
    {
        return _func;
    }

    void to_string(std::string *appended) const;
};

}

/** used to build a SourceLocation instance */
#define NUT_SOURCE_LOCATION       nut::SourceLocation(__FILE__,__LINE__,__FUNCTION__)

/** used to initialize a SourceLocation instance */
#define NUT_SOURCE_LOCATION_ARGS  __FILE__,__LINE__,__FUNCTION__

#endif /* head file guarder */
