
#ifndef ___HEADFILE_B97A0C3D_DF41_415F_A7F1_F1219A9F8C6F_
#define ___HEADFILE_B97A0C3D_DF41_415F_A7F1_F1219A9F8C6F_

namespace nut
{

template <typename T>
class singleton
{
public:
    typedef T object_type;

private:
    class object_creator
    {
    public:
        object_creator() noexcept
        {
            singleton<T>::instance();
        }

        void do_nothing() const noexcept
        {}
    };

public:
    static object_type& instance() noexcept
    {
        static object_type obj;
        _create_object.do_nothing();
        return obj;
    }

private:
    static object_creator _create_object;
};

template <typename T>
typename singleton<T>::object_creator singleton<T>::_create_object;

}

#endif
