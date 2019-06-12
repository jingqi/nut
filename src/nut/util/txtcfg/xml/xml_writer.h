
#ifndef ___HEADFILE_66E380A0_1B5F_4503_AEB6_19DB1A28D1AE_
#define ___HEADFILE_66E380A0_1B5F_4503_AEB6_19DB1A28D1AE_

#include <assert.h>
#include <string>
#include <vector>
#include <iostream>

#include "../../../nut_config.h"
#include "../../../platform/int_type.h" // for ssize_t in Windows VC


namespace nut
{

class NUT_API XmlWriter
{
public:
    explicit XmlWriter(std::ostream *os = nullptr) noexcept;

    std::ostream* get_output_stream() const noexcept;
    void set_output_stream(std::ostream *os) noexcept;

    void start_element(const std::string& name) noexcept;
    void end_element() noexcept;

    void write_attribute(const std::string& name, const std::string& value) noexcept;
    void write_text(const std::string& text) noexcept;
    void write_comment(const std::string& comment) noexcept;

private:
    void write(const char *s, ssize_t len = -1) noexcept;
    void write(const std::string& s) noexcept;
    void write_encode(const char *s, ssize_t len = -1) noexcept;

private:
    class ElemState
    {
    public:
        explicit ElemState(std::string&& n) noexcept
            : name(std::forward<std::string>(n))
        {}

        explicit ElemState(const std::string& n) noexcept
            : name(n)
        {}

    public:
        std::string name;
        bool has_child = false;
    };

    std::ostream *_os = nullptr;
    std::vector<ElemState> _elem_path;
};

}

#endif
