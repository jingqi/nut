/**
 * @file -
 * @author jingqi
 * @date 2012-08-09
 * @last-edit 2012-08-09 20:01:39 jingqi
 */

#ifndef ___HEADFILE_FD589E58_04A2_4A16_AF82_9F6AE78BCEAE_
#define ___HEADFILE_FD589E58_04A2_4A16_AF82_9F6AE78BCEAE_

#include <assert.h>
#include <vector>

namespace nut
{

template <typename T> class delegate;

#define __DELEGATE(TEMPLATE_ARGS, FUNCTION_ARGS, FUNCTION_PARA) \
template <typename Ret TEMPLATE_ARGS> \
class delegate<Ret (FUNCTION_ARGS)> \
{ \
    typedef delegate<Ret (FUNCTION_ARGS)> self; \
 \
    enum HolderType \
    { \
        FUNCTOR, \
        MEMBER_FUNCTION \
    }; \
 \
    struct IHolder \
    { \
        virtual ~IHolder() {}; \
        virtual HolderType holderType() const = 0; \
        virtual Ret operator() (FUNCTION_ARGS) = 0; \
        virtual IHolder* clone() const = 0; \
    }; \
    std::vector<IHolder*> m_holders; \
 \
    template <typename FunctorPtr> \
    struct FunctorHolder : public IHolder \
    { \
        FunctorPtr func; \
 \
        FunctorHolder(FunctorPtr pfunc) \
            : func(pfunc) \
        { \
            assert(NULL != pfunc); \
        } \
        virtual HolderType holderType() const  { return FUNCTOR; } \
        virtual Ret operator()(FUNCTION_ARGS) \
        { \
            assert(NULL != func); \
            return (*func)(FUNCTION_PARA); \
        } \
        virtual IHolder* clone() const \
        { \
            return new FunctorHolder<FunctorPtr>(*this); \
        } \
    }; \
 \
    template <typename U, typename MemFun> \
    struct MemHolder : public IHolder \
    { \
        U obj; \
        MemFun memFunc; \
 \
        MemHolder(U o, MemFun f) \
            : obj(o), memFunc(f) \
        { \
            assert(NULL != o && NULL != f); \
        } \
        virtual HolderType holderType() const { return MEMBER_FUNCTION; } \
        virtual Ret operator()(FUNCTION_ARGS) \
        { \
            assert(NULL != obj && NULL != memFunc); \
            return ((*obj).*memFunc)(FUNCTION_PARA); \
        } \
        virtual IHolder* clone() const \
        { \
            return new MemHolder<U,MemFun>(*this); \
        } \
    }; \
 \
public: \
    delegate() {} \
 \
    template <typename FunctorPtr>\
    delegate(FunctorPtr func) \
    { \
        assert(NULL != func); \
        m_holders.push_back(new FunctorHolder<FunctorPtr>(func)); \
    } \
 \
    template <typename U, typename MemFun>\
    delegate(U obj, MemFun mfunc) \
    { \
        assert(NULL != obj && NULL != mfunc); \
        m_holders.push_back(new MemHolder<U,MemFun>(obj, mfunc)); \
    } \
 \
    delegate(const self& x) \
    { \
        for (register size_t i = 0, size = x.m_holders.size(); i < size; ++i) \
        { \
            assert(NULL != x.m_holders[i]); \
            m_holders.push_back(x.m_holders[i]->clone()); \
        } \
    } \
 \
    ~delegate() \
    { \
        disconnectAll();  \
    } \
 \
    delegate& operator=(const self& x)\
    { \
        if (&x != this)\
        { \
            disconnectAll(); \
            for (register size_t i = 0, size = x.m_holders.size(); i < size; ++i) \
            { \
                assert(NULL != x.m_holders[i]); \
                m_holders.push_back(x.m_holders[i]->clone()); \
            } \
        } \
        return *this; \
    } \
 \
    template <typename FunctorPtr> \
    void connect(FunctorPtr func) \
    { \
        assert(NULL != func); \
        m_holders.push_back(new FunctorHolder<FunctorPtr>(func)); \
    } \
 \
    template <typename U, typename MemFun> \
    void connect(U obj, MemFun mfunc) \
    { \
        assert(NULL != obj && NULL != mfunc); \
        m_holders.push_back(new MemHolder<U, MemFun>(obj, mfunc));\
    } \
 \
    template <typename FunctorPtr> \
    bool disconnect(FunctorPtr func) \
    { \
        bool found = false; \
        for (register size_t i = 0; i < m_holders.size(); ++i) \
        { \
            if (m_holders[i]->holderType() == FUNCTOR) \
            { \
                FunctorHolder<FunctorPtr> *h = dynamic_cast<FunctorHolder<FunctorPtr>*>(m_holders[i]); \
                assert(NULL != h); \
                if (h->func == func) \
                { \
                    found = true; \
                    delete h; \
                    m_holders.erase(m_holders.begin() + i); \
                    --i; \
                } \
            } \
        } \
        return found; \
    } \
 \
    template <typename U, typename MemFun> \
    bool disconnect(U obj, MemFun mfunc) \
    { \
        bool found = false; \
        for (register size_t i = 0; i < m_holders.size(); ++i) \
        { \
            if (m_holders[i]->holderType() == MEMBER_FUNCTION) \
            { \
                MemHolder<U,MemFun> *h = dynamic_cast<MemHolder<U,MemFun>*>(m_holders[i]); \
                assert(NULL != h); \
                if (h->obj == obj && h->memFunc == mfunc) \
                { \
                    found = true; \
                    delete h; \
                    m_holders.erase(m_holders.begin() + i); \
                    --i; \
                } \
            } \
        } \
        return found; \
    } \
 \
    void disconnectAll() \
    { \
        for (register size_t i = 0, size = m_holders.size(); i < size; ++i) \
        { \
            assert(NULL != m_holders[i]); \
            delete m_holders[i]; \
        } \
        m_holders.clear(); \
    } \
 \
    bool isConnected() const \
    { \
        return m_holders.size() > 0; \
    } \
 \
    Ret operator()(FUNCTION_ARGS) \
    { \
        for (register size_t i = 0, size = m_holders.size(); i < size; ++i) \
        { \
            assert(NULL != m_holders[i]); \
            if (i == size - 1) \
                return (*m_holders[i])(FUNCTION_PARA); \
            (*m_holders[i])(FUNCTION_PARA); \
        } \
        return Ret(); \
    } \
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

__DELEGATE(__TEMPLATE_ARGS_0, __FUNCTION_ARGS_0, __FUNCTION_PARA_0)
__DELEGATE(__TEMPLATE_ARGS_1, __FUNCTION_ARGS_1, __FUNCTION_PARA_1)
__DELEGATE(__TEMPLATE_ARGS_2, __FUNCTION_ARGS_2, __FUNCTION_PARA_2)
__DELEGATE(__TEMPLATE_ARGS_3, __FUNCTION_ARGS_3, __FUNCTION_PARA_3)
__DELEGATE(__TEMPLATE_ARGS_4, __FUNCTION_ARGS_4, __FUNCTION_PARA_4)
__DELEGATE(__TEMPLATE_ARGS_5, __FUNCTION_ARGS_5, __FUNCTION_PARA_5)
__DELEGATE(__TEMPLATE_ARGS_6, __FUNCTION_ARGS_6, __FUNCTION_PARA_6)

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

#undef __DELEGATE

}

#endif
