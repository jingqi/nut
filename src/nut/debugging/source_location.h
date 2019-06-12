
#ifndef ___HEADFILE___2BD47B8B_B18E_421F_8A9D_DC72BAF49ABF_
#define ___HEADFILE___2BD47B8B_B18E_421F_8A9D_DC72BAF49ABF_

#include <assert.h>
#include <string>

#include "../nut_config.h"


namespace nut
{

/**
 * C/C++ 源码位置
 */
class NUT_API SourceLocation
{
public:
    constexpr SourceLocation(const char *file, int line, const char *func) noexcept
        : _path(file), _line(line), _func(func)
    {}

    bool operator==(const SourceLocation& x) const noexcept;
    bool operator!=(const SourceLocation& x) const noexcept;

    constexpr const char* get_file_path() const noexcept
    {
        return _path;
    }

    const char* get_file_name() const noexcept;

    constexpr int get_line_number() const noexcept
    {
        return _line;
    }

    /**
     * @return This method may return nullptr if the location is not in any function
     */
    constexpr const char* get_function_name() const noexcept
    {
        return _func;
    }

    std::string to_string() const noexcept;

private:
    const char *_path = nullptr;    // source file path
    int _line = -1;                 // source file line
    const char *_func = nullptr;    // source function
};

}

/** used to build a SourceLocation instance */
#define NUT_SOURCE_LOCATION       nut::SourceLocation(__FILE__,__LINE__,__FUNCTION__)

/** used to initialize a SourceLocation instance */
#define NUT_SOURCE_LOCATION_ARGS  __FILE__,__LINE__,__FUNCTION__

#endif /* head file guarder */
