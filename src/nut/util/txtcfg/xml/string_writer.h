
#ifndef ___HEADFILE_4CB34890_F2AA_4291_AADE_C3CAAC2CF27B_
#define ___HEADFILE_4CB34890_F2AA_4291_AADE_C3CAAC2CF27B_

namespace nut
{

class StringWriter
{
public:
    virtual ~StringWriter()
    {}

    virtual void write(const char *s, int len = -1) = 0;
};

class StdStringWriter : public StringWriter
{
    std::string *_output = NULL;

public:
    StdStringWriter(std::string *output)
        : _output(output)
    {}

    virtual void write(const char *s, int len) override
    {
        if (NULL == s || 0 == len)
            return;
        if (len < 0)
            *_output += s;
        for (int i = 0; i < len && '\0' != s[i]; ++i)
            _output->push_back(s[i]);
    }
};

}

#endif
