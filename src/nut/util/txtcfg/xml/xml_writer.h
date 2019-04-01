
#ifndef ___HEADFILE_66E380A0_1B5F_4503_AEB6_19DB1A28D1AE_
#define ___HEADFILE_66E380A0_1B5F_4503_AEB6_19DB1A28D1AE_

#include <assert.h>
#include <string>
#include <vector>
#include <iostream>

#include <nut/platform/int_type.h> // for ssize_t in Windows VC

#include "../../../nut_config.h"


namespace nut
{

class NUT_API XmlWriter
{
public:
    explicit XmlWriter(std::ostream *os = nullptr);

    std::ostream* get_output_stream() const;
    void set_output_stream(std::ostream *os);

    void start_element(const std::string& name);
    void end_element();

    void write_attribute(const std::string& name, const std::string& value);
    void write_text(const std::string& text);
    void write_comment(const std::string& comment);

private:
    void write(const char *s, ssize_t len = -1);
    void write(const std::string& s);
    void write_encode(const char *s, ssize_t len = -1);

private:
    class ElemState
    {
    public:
        explicit ElemState(std::string&& n)
            : name(std::forward<std::string>(n))
        {}

        explicit ElemState(const std::string& n)
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
