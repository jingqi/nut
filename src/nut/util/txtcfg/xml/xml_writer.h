
#ifndef ___HEADFILE_66E380A0_1B5F_4503_AEB6_19DB1A28D1AE_
#define ___HEADFILE_66E380A0_1B5F_4503_AEB6_19DB1A28D1AE_

#include <assert.h>
#include <string>
#include <vector>
#include <iostream>

#include "../../../nut_config.h"


namespace nut
{

class NUT_API XmlWriter
{
    struct ElemState
    {
        std::string name;
        bool has_child = false;

        ElemState(const char *n)
            : name(n)
        {}
    };

    std::ostream *_os = nullptr;
    std::vector<ElemState> _elem_path;

public:
    explicit XmlWriter(std::ostream *os = nullptr);

    std::ostream* get_output_stream() const
    {
        return _os;
    }

    void set_output_stream(std::ostream *os)
    {
        _os = os;
    }

    void start_element(const char *name);

    void end_element();

    void write_attribute(const char *name, const char *value);

    void write_text(const char *text);

    void write_comment(const char *comment);

private:
    void write(const char *s, int len = -1);
    void write_encode(const char *s, int len = -1);
};

}

#endif
