
#ifndef ___HEADFILE_66E380A0_1B5F_4503_AEB6_19DB1A28D1AE_
#define ___HEADFILE_66E380A0_1B5F_4503_AEB6_19DB1A28D1AE_

#include <assert.h>
#include <string>
#include <vector>

#include "string_writer.h"

namespace nut
{

class XmlWriter
{
    struct ElemState
    {
        std::string name;
        bool has_child = false;

        ElemState(const char *n)
            : name(n)
        {}
    };

    StringWriter *_writer = NULL;
    std::vector<ElemState> _elem_path;

public:
    XmlWriter(StringWriter *writer = NULL);

    StringWriter* get_writer() const
    {
        return _writer;
    }

    void set_writer(StringWriter *writer)
    {
        _writer = writer;
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
