/**
 * @file -
 * @author jingqi
 * @date 2011-12-23
 * @last-edit 2015-01-06 22:36:43 jingqi
 */

#ifndef ___HEADFILE_B97A0C3D_DF41_415F_A7F1_F1219A9F8C6F_
#define ___HEADFILE_B97A0C3D_DF41_415F_A7F1_F1219A9F8C6F_

namespace nut
{

template <typename T>
class singleton
{
    struct object_creator
    {
        object_creator()
        {
            singleton<T>::instance();
        }

        inline void do_nothing() const
        {}
    };

    static object_creator create_object;

public:
    typedef T object_type;
    static object_type& instance()
    {
        static object_type obj;
        create_object.do_nothing();
        return obj;
    }
};

template <typename T>
typename singleton<T>::object_creator singleton<T>::create_object;

}

#endif
