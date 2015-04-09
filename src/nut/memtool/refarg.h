
#ifndef ___HEADFILE_699C2419_3BD9_4E56_8C80_1E0EDEF4EC84_
#define ___HEADFILE_699C2419_3BD9_4E56_8C80_1E0EDEF4EC84_

namespace nut
{

template <typename T>
struct ref_arg
{
    T& ref;
    ref_arg(T& arg)
        : ref(arg)
    {}
};

template <typename T>
struct const_ref_arg
{
    const T& ref;
    const_ref_arg(const T& arg)
        : ref(arg)
    {}
};

template <typename T>
struct RefargTraits
{
    static const T& value(const T& v)
    {
        return v;
    }
};

template <typename T>
struct RefargTraits<ref_arg<T> >
{
    static T& value(const ref_arg<T>& v)
    {
        return v.ref;
    }
};

template <typename T>
struct RefargTraits<const_ref_arg<T> >
{
    static const T& value(const const_ref_arg<T>& v)
    {
        return v.ref;
    }
};

}

#endif /* head file guarder */
