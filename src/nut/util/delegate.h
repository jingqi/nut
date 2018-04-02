﻿
#ifndef ___HEADFILE_FD589E58_04A2_4A16_AF82_9F6AE78BCEAE_
#define ___HEADFILE_FD589E58_04A2_4A16_AF82_9F6AE78BCEAE_

#include <assert.h>
#include <vector>

#include <nut/platform/platform.h>

namespace nut
{

template <typename T> class delegate;

#if NUT_PLATFORM_CC_VC
#   define __THISCALL __thiscall
#else
#   define __THISCALL
#endif

#define __DELEGATE(TEMPLATE_ARGS, FUNCTION_ARGS, FUNCTION_PARA)         \
template <typename Ret TEMPLATE_ARGS>                                   \
class delegate<Ret (FUNCTION_ARGS)>                                     \
{                                                                       \
    typedef delegate<Ret (FUNCTION_ARGS)> self_type;                    \
                                                                        \
    enum class HolderType                                               \
    {                                                                   \
        FUNCTOR,                                                        \
        MEMBER_FUNCTION                                                 \
    };                                                                  \
                                                                        \
    struct IHolder                                                      \
    {                                                                   \
        virtual ~IHolder() = default;                                   \
        virtual HolderType type() const = 0;                            \
        virtual Ret operator() (FUNCTION_ARGS) = 0;                     \
        virtual IHolder* clone() const = 0;                             \
    };                                                                  \
    std::vector<IHolder*> _holders;                                     \
                                                                        \
    template <typename FunctorPtr>                                      \
    struct FunctorHolder : public IHolder                               \
    {                                                                   \
        FunctorPtr func = nullptr;                                      \
                                                                        \
        FunctorHolder(FunctorPtr pfunc)                                 \
            : func(pfunc)                                               \
        {                                                               \
            assert(nullptr != pfunc);                                   \
        }                                                               \
                                                                        \
        virtual HolderType type() const                                 \
        {                                                               \
            return HolderType::FUNCTOR;                                 \
        }                                                               \
                                                                        \
        virtual Ret operator()(FUNCTION_ARGS)                           \
        {                                                               \
            assert(nullptr != func);                                    \
            return (*func)(FUNCTION_PARA);                              \
        }                                                               \
                                                                        \
        virtual IHolder* clone() const                                  \
        {                                                               \
            return new FunctorHolder<FunctorPtr>(*this);                \
        }                                                               \
    };                                                                  \
                                                                        \
    template <typename U, typename MemFun>                              \
    struct MemHolder : public IHolder                                   \
    {                                                                   \
        U obj = nullptr;                                                \
        MemFun mem_func = nullptr;                                      \
                                                                        \
        MemHolder(U o, MemFun f)                                        \
            : obj(o), mem_func(f)                                       \
        {                                                               \
            assert(nullptr != o && nullptr != f);                       \
        }                                                               \
                                                                        \
        virtual HolderType type() const                                 \
        {                                                               \
            return HolderType::MEMBER_FUNCTION;                         \
        }                                                               \
                                                                        \
        virtual Ret operator()(FUNCTION_ARGS)                           \
        {                                                               \
            assert(nullptr != obj && nullptr != mem_func);              \
            return ((*obj).*mem_func)(FUNCTION_PARA);                   \
        }                                                               \
                                                                        \
        virtual IHolder* clone() const                                  \
        {                                                               \
            return new MemHolder<U,MemFun>(*this);                      \
        }                                                               \
    };                                                                  \
                                                                        \
public:                                                                 \
    delegate() = default;                                               \
                                                                        \
    template <typename FunctorPtr>                                      \
    delegate(FunctorPtr func)                                           \
    {                                                                   \
        assert(nullptr != func);                                        \
        _holders.push_back(new FunctorHolder<FunctorPtr>(func));        \
    }                                                                   \
                                                                        \
    template <typename U, typename MemFun>                              \
    delegate(U obj, MemFun mfunc)                                       \
    {                                                                   \
        assert(nullptr != obj && nullptr != mfunc);                     \
        _holders.push_back(new MemHolder<U,MemFun>(obj, mfunc));        \
    }                                                                   \
                                                                        \
    delegate(const self_type& x)                                        \
    {                                                                   \
        for (size_t i = 0, sz = x._holders.size(); i < sz; ++i)         \
        {                                                               \
            assert(nullptr != x._holders[i]);                           \
            _holders.push_back(x._holders[i]->clone());                 \
        }                                                               \
    }                                                                   \
                                                                        \
    ~delegate()                                                         \
    {                                                                   \
        disconnect_all();                                               \
    }                                                                   \
                                                                        \
    delegate& operator=(const self_type& x)                             \
    {                                                                   \
        if (this == &x)                                                 \
            return *this;                                               \
        disconnect_all();                                               \
        for (size_t i = 0, sz = x._holders.size(); i < sz; ++i)         \
        {                                                               \
            assert(nullptr != x._holders[i]);                           \
            _holders.push_back(x._holders[i]->clone());                 \
        }                                                               \
        return *this;                                                   \
    }                                                                   \
                                                                        \
    bool operator==(const self_type& x) const                           \
    {                                                                   \
        if (_holders.size() != x._holders.size())                       \
            return false;                                               \
        for (size_t i = _holders.size(); i > 0; --i)                    \
        {                                                               \
            assert(nullptr != _holders[i - 1] && nullptr != x._holders[i - 1]); \
            if (_holders[i - 1]->type() != x._holders[i - 1]->type())   \
                return false;                                           \
            switch (_holders[i - 1]->type())                            \
            {                                                           \
            case HolderType::FUNCTOR: {                                 \
                typedef FunctorHolder<Ret(*)(FUNCTION_ARGS)> *holder_type; \
                holder_type fh1 = dynamic_cast<holder_type>(_holders[i - 1]); \
                holder_type fh2 = dynamic_cast<holder_type>(x._holders[i - 1]); \
                assert(nullptr != fh1 && nullptr != fh2);               \
                if (fh1->func != fh2->func)                             \
                    return false;                                       \
                break;                                                  \
            }                                                           \
                                                                        \
            case HolderType::MEMBER_FUNCTION: {                         \
                typedef MemHolder<IHolder*, Ret(__THISCALL IHolder::*)(FUNCTION_ARGS)> *holder_type; \
                holder_type mh1 = (holder_type)(_holders[i - 1]);       \
                holder_type mh2 = (holder_type)(x._holders[i - 1]);     \
                assert(nullptr != mh1 && nullptr != mh2);               \
                if (mh1->obj != mh2->obj || mh1->mem_func != mh2->mem_func) \
                    return false;                                       \
                break;                                                  \
            }                                                           \
                                                                        \
            default:                                                    \
                assert(false);                                          \
                return false;                                           \
            }                                                           \
        }                                                               \
        return true;                                                    \
    }                                                                   \
                                                                        \
    bool operator!=(const self_type& x) const                           \
    {                                                                   \
        return !(*this == x);                                           \
    }                                                                   \
                                                                        \
    template <typename FunctorPtr>                                      \
    void connect(FunctorPtr func)                                       \
    {                                                                   \
        assert(nullptr != func);                                        \
        _holders.push_back(new FunctorHolder<FunctorPtr>(func));        \
    }                                                                   \
                                                                        \
    template <typename U, typename MemFun>                              \
    void connect(U obj, MemFun mfunc)                                   \
    {                                                                   \
        assert(nullptr != obj && nullptr != mfunc);                     \
        _holders.push_back(new MemHolder<U, MemFun>(obj, mfunc));       \
    }                                                                   \
                                                                        \
    template <typename FunctorPtr>                                      \
    bool disconnect(FunctorPtr func)                                    \
    {                                                                   \
        bool found = false;                                             \
        for (size_t i = 0; i < _holders.size(); ++i)                    \
        {                                                               \
            if (_holders[i]->type() == HolderType::FUNCTOR)             \
            {                                                           \
                FunctorHolder<FunctorPtr> *h = dynamic_cast<FunctorHolder<FunctorPtr>*>(_holders[i]); \
                assert(nullptr != h);                                   \
                if (h->func == func)                                    \
                {                                                       \
                    found = true;                                       \
                    delete h;                                           \
                    _holders.erase(_holders.begin() + i);               \
                    --i;                                                \
                }                                                       \
            }                                                           \
        }                                                               \
        return found;                                                   \
    }                                                                   \
                                                                        \
    template <typename U, typename MemFun>                              \
    bool disconnect(U obj, MemFun mfunc)                                \
    {                                                                   \
        bool found = false;                                             \
        for (size_t i = 0; i < _holders.size(); ++i)                    \
        {                                                               \
            if (_holders[i]->type() == HolderType::MEMBER_FUNCTION)     \
            {                                                           \
                MemHolder<U,MemFun> *h = dynamic_cast<MemHolder<U,MemFun>*>(_holders[i]); \
                assert(nullptr != h);                                   \
                if (h->obj == obj && h->mem_func == mfunc)              \
                {                                                       \
                    found = true;                                       \
                    delete h;                                           \
                    _holders.erase(_holders.begin() + i);               \
                    --i;                                                \
                }                                                       \
            }                                                           \
        }                                                               \
        return found;                                                   \
    }                                                                   \
                                                                        \
    void disconnect_all()                                               \
    {                                                                   \
        for (size_t i = 0, sz = _holders.size(); i < sz; ++i)           \
        {                                                               \
            assert(nullptr != _holders[i]);                             \
            delete _holders[i];                                         \
        }                                                               \
        _holders.clear();                                               \
    }                                                                   \
                                                                        \
    bool is_connected() const                                           \
    {                                                                   \
        return _holders.size() > 0;                                     \
    }                                                                   \
                                                                        \
    Ret operator()(FUNCTION_ARGS) const                                 \
    {                                                                   \
        for (size_t i = 0, sz = _holders.size(); i < sz; ++i)           \
        {                                                               \
            assert(nullptr != _holders[i]);                             \
            if (i == sz - 1)                                            \
                return (*_holders[i])(FUNCTION_PARA);                   \
            (*_holders[i])(FUNCTION_PARA);                              \
        }                                                               \
        return Ret();                                                   \
    }                                                                   \
};


#define __TEMPLATE_ARGS_0
#define __FUNCTION_ARGS_0
#define __FUNCTION_PARA_0

#define __TEMPLATE_ARGS_1      , typename T0
#define __FUNCTION_ARGS_1      T0 t0
#define __FUNCTION_PARA_1      t0

#define __TEMPLATE_ARGS_2 __TEMPLATE_ARGS_1, typename T1
#define __FUNCTION_ARGS_2 __FUNCTION_ARGS_1, T1 t1
#define __FUNCTION_PARA_2 __FUNCTION_PARA_1, t1

#define __TEMPLATE_ARGS_3 __TEMPLATE_ARGS_2, typename T2
#define __FUNCTION_ARGS_3 __FUNCTION_ARGS_2, T2 t2
#define __FUNCTION_PARA_3 __FUNCTION_PARA_2, t2

#define __TEMPLATE_ARGS_4 __TEMPLATE_ARGS_3, typename T3
#define __FUNCTION_ARGS_4 __FUNCTION_ARGS_3, T3 t3
#define __FUNCTION_PARA_4 __FUNCTION_PARA_3, t3

#define __TEMPLATE_ARGS_5 __TEMPLATE_ARGS_4, typename T4
#define __FUNCTION_ARGS_5 __FUNCTION_ARGS_4, T4 t4
#define __FUNCTION_PARA_5 __FUNCTION_PARA_4, t4

#define __TEMPLATE_ARGS_6 __TEMPLATE_ARGS_5, typename T5
#define __FUNCTION_ARGS_6 __FUNCTION_ARGS_5, T5 t5
#define __FUNCTION_PARA_6 __FUNCTION_PARA_5, t5

#define __TEMPLATE_ARGS_7 __TEMPLATE_ARGS_6, typename T6
#define __FUNCTION_ARGS_7 __FUNCTION_ARGS_6, T6 t6
#define __FUNCTION_PARA_7 __FUNCTION_PARA_6, t6

#define __TEMPLATE_ARGS_8 __TEMPLATE_ARGS_7, typename T7
#define __FUNCTION_ARGS_8 __FUNCTION_ARGS_7, T7 t7
#define __FUNCTION_PARA_8 __FUNCTION_PARA_7, t7

#define __TEMPLATE_ARGS_9 __TEMPLATE_ARGS_8, typename T8
#define __FUNCTION_ARGS_9 __FUNCTION_ARGS_8, T8 t8
#define __FUNCTION_PARA_9 __FUNCTION_PARA_8, t8

#define __TEMPLATE_ARGS_10 __TEMPLATE_ARGS_9, typename T9
#define __FUNCTION_ARGS_10 __FUNCTION_ARGS_9, T9 t9
#define __FUNCTION_PARA_10 __FUNCTION_PARA_9, t9

__DELEGATE(__TEMPLATE_ARGS_0, __FUNCTION_ARGS_0, __FUNCTION_PARA_0)
__DELEGATE(__TEMPLATE_ARGS_1, __FUNCTION_ARGS_1, __FUNCTION_PARA_1)
__DELEGATE(__TEMPLATE_ARGS_2, __FUNCTION_ARGS_2, __FUNCTION_PARA_2)
__DELEGATE(__TEMPLATE_ARGS_3, __FUNCTION_ARGS_3, __FUNCTION_PARA_3)
__DELEGATE(__TEMPLATE_ARGS_4, __FUNCTION_ARGS_4, __FUNCTION_PARA_4)
__DELEGATE(__TEMPLATE_ARGS_5, __FUNCTION_ARGS_5, __FUNCTION_PARA_5)
__DELEGATE(__TEMPLATE_ARGS_6, __FUNCTION_ARGS_6, __FUNCTION_PARA_6)
__DELEGATE(__TEMPLATE_ARGS_7, __FUNCTION_ARGS_7, __FUNCTION_PARA_7)
__DELEGATE(__TEMPLATE_ARGS_8, __FUNCTION_ARGS_8, __FUNCTION_PARA_8)
__DELEGATE(__TEMPLATE_ARGS_9, __FUNCTION_ARGS_9, __FUNCTION_PARA_9)
__DELEGATE(__TEMPLATE_ARGS_10, __FUNCTION_ARGS_10, __FUNCTION_PARA_10)

#undef __TEMPLATE_ARGS_0
#undef __FUNCTION_ARGS_0
#undef __FUNCTION_PARA_0

#undef __TEMPLATE_ARGS_1
#undef __FUNCTION_ARGS_1
#undef __FUNCTION_PARA_1

#undef __TEMPLATE_ARGS_2
#undef __FUNCTION_ARGS_2
#undef __FUNCTION_PARA_2

#undef __TEMPLATE_ARGS_3
#undef __FUNCTION_ARGS_3
#undef __FUNCTION_PARA_3

#undef __TEMPLATE_ARGS_4
#undef __FUNCTION_ARGS_4
#undef __FUNCTION_PARA_4

#undef __TEMPLATE_ARGS_5
#undef __FUNCTION_ARGS_5
#undef __FUNCTION_PARA_5

#undef __TEMPLATE_ARGS_6
#undef __FUNCTION_ARGS_6
#undef __FUNCTION_PARA_6

#undef __TEMPLATE_ARGS_7
#undef __FUNCTION_ARGS_7
#undef __FUNCTION_PARA_7

#undef __TEMPLATE_ARGS_8
#undef __FUNCTION_ARGS_8
#undef __FUNCTION_PARA_8

#undef __TEMPLATE_ARGS_9
#undef __FUNCTION_ARGS_9
#undef __FUNCTION_PARA_9

#undef __TEMPLATE_ARGS_10
#undef __FUNCTION_ARGS_10
#undef __FUNCTION_PARA_10

#undef __DELEGATE

}

#endif
