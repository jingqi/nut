/**
 * @file -
 * @author jingqi
 * @date 2011-11-11 18:49
 */

#ifndef ___HEADFILE___B77B6DC2_6C30_4732_943B_117AF7E35B83_
#define ___HEADFILE___B77B6DC2_6C30_4732_943B_117AF7E35B83_

#include <stdint.h>
#include <stdlib.h>
#include <new.h>

#include <nut/debugging/destroychecker.hpp>

#include "refcounter.hpp"
#include "ref.hpp"

namespace nut
{

/**
 * 可引用计数对象包装器
 *
 * @note
 * 使用包装器来包装的好处：
 * 1. 析构函数的部分见gc_new的note
 * 2. 如果对象是多继承的，计数器仍然只有一份，仍然能正常运行
 */
template <typename T, typename COUNTER = RefCounter>
class GCWrapper : public T
{
protected:
	/** 销毁器类型 */
	typedef void (*destroyer)(GCWrapper<T,COUNTER>*);

	/** 引用计数 */
	COUNTER m_counter;
	/** 销毁器 */
	destroyer m_destroyer;

	/** 避免多次销毁的检查器 */
#ifndef NDEBUG
	DestroyChecker m_checker;
#endif

public:
	GCWrapper(int c, destroyer d) : T(), m_counter(c), m_destroyer(d) {}

	template <typename Arg1>
	GCWrapper(int c, destroyer d, Arg1 arg1) : T(arg1), m_counter(c), m_destroyer(d) {}

	template <typename Arg1, typename Arg2>
	GCWrapper(int c, destroyer d, Arg1 arg1, Arg2 arg2) : T(arg1, arg2), m_counter(c), m_destroyer(d) {}

	template <typename Arg1, typename Arg2, typename Arg3>
	GCWrapper(int c, destroyer d, Arg1 arg1, Arg2 arg2, Arg3 arg3) : T(arg1, arg2, arg3), m_counter(c), m_destroyer(d) {}

	virtual void add_ref()
	{
#ifndef NDEBUG
		m_checker.checkDestroy();
#endif
		++m_counter;
	}

	virtual void rls_ref()
	{
#ifndef NDEBUG
		m_checker.checkDestroy();
#endif
		if (--m_counter == 0)
			m_destroyer(this);
	}
};

/**
 * 可引用计数对象生成器
 */
template <typename T, typename COUNTER = RefCounter>
class gc_new : public ref<typename RefTraits<T>::plain_type>
{
	typedef typename RefTraits<T>::plain_type plain_type;
	typedef GCWrapper<plain_type, COUNTER> wrapper_type;

	using weak_ref<plain_type>::m_ptr;

	static inline wrapper_type* alloc_wrapper()
	{
		return (wrapper_type*)::malloc(sizeof(wrapper_type));
	}

    /**
     * @note
     * 该处如此实现的好处：
     * 1. 保证内存分配和销毁的代码在同一模块
     * 2. 即使对象没有申明虚的析构函数也不会析构出错
     */
	static void destroy_wrapper(wrapper_type *p)
	{
		p->~wrapper_type();
		::free(p);
	}

public:
	gc_new()
	{
		m_ptr = alloc_wrapper();
		new (m_ptr) wrapper_type(1, destroy_wrapper);
	}

	template <typename Arg1>
	gc_new(Arg1 arg1)
	{
		m_ptr = alloc_wrapper();
		new (m_ptr) wrapper_type(1, destroy_wrapper, arg1);
	}

	template <typename Arg1, typename Arg2>
	gc_new(Arg1 arg1, Arg2 arg2)
	{
		m_ptr = alloc_wrapper();
		new (m_ptr) wrapper_type(1, destroy_wrapper, arg1, arg2);
	}

	template <typename Arg1, typename Arg2, typename Arg3>
	gc_new(Arg1 arg1, Arg2 arg2, Arg3 arg3)
	{
		m_ptr = alloc_wrapper();
		new (m_ptr) wrapper_type(1, destroy_wrapper, arg1, arg2, arg3);
	}
};

/**
 * 声明可引用计数
 */
#define DECLARE_GC_ENABLE \
	virtual void add_ref() = 0; \
	virtual void rls_ref() = 0; \
	template <typename ___T> friend class nut::ref;

}

#endif /* head file guarder */
