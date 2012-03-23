/* -------------------------------------------------------------------------
//	filename	：	memory/autofree.h
//	author		：	许式伟
//	create time	:2005-7-15 23:20:46
//	purpose		：	最袖珍的垃圾回收器（具备自动内存回收能力的allocator）。
//
//	$Id: autofree.h,v 1.13 2006/03/15 02:39:50 xushiwei Exp $
// -----------------------------------------------------------------------*/
#ifndef __MEMORY_AUTOFREE_H__
#define __MEMORY_AUTOFREE_H__



#define MEMORY_BLOCK_SIZE 2048


namespace freeole
{

typedef void (*FnDestructor)(void*);

struct StdLibAlloc
{
    static void* Alloc(size_t cb)	{ return AlgAlloc(cb); }
    static void Free(void* p)		{ AlgFree(p); }
};

typedef StdLibAlloc DefaultStaticAlloc;

// -------------------------------------------------------------------------
// class AutoFreeAlloc

template <class _Alloc>
class TAutoFreeAlloc
{
public:
	enum { MemBlockSize = 2048 };
	enum { HeaderSize = sizeof(void*) };
	enum { BlockSize = MemBlockSize - HeaderSize };
	enum { IsAutoFreeAlloctor = 1 };

private:
	struct _MemBlock
	{
		_MemBlock* pPrev;
		char buffer[BlockSize];
	};
	struct _DestroyNode
	{
		_DestroyNode* pPrev;
		FnDestructor fnDestroy;
	};

	char* m_begin;
	char* m_end;
	_DestroyNode* m_destroyChain;
	_Alloc m_alloc;

private:
	_MemBlock* _ChainHeader() const
	{
		return (_MemBlock*)(m_begin - HeaderSize);
	}

	TAutoFreeAlloc(const TAutoFreeAlloc& rhs);
	const TAutoFreeAlloc& operator=(const TAutoFreeAlloc& rhs);

public:
	TAutoFreeAlloc() : m_destroyChain(NULL)
	{
		m_begin = m_end = (char*)HeaderSize;
	}
	TAutoFreeAlloc(_Alloc alloc) : m_alloc(alloc), m_destroyChain(NULL)
	{
		m_begin = m_end = (char*)HeaderSize;
	}

	~TAutoFreeAlloc()
	{
		Clear();
	}

	void Clear()
	{
		while (m_destroyChain)
		{
			m_destroyChain->fnDestroy(m_destroyChain + 1);
			m_destroyChain = m_destroyChain->pPrev;
		}
		_MemBlock* pHeader = _ChainHeader();
		while (pHeader)
		{
			_MemBlock* pTemp = pHeader->pPrev;
			m_alloc.Free(pHeader);
			pHeader = pTemp;
		}
		m_begin = m_end = (char*)HeaderSize;
	}

	void* Alloc(size_t cb)
	{
		if ((size_t)(m_end - m_begin) < cb)
		{
			if (cb >= BlockSize)
			{
				_MemBlock* pHeader = _ChainHeader();
				_MemBlock* pNew = (_MemBlock*)m_alloc.Alloc(HeaderSize + cb);
				if (pHeader)
				{
					pNew->pPrev = pHeader->pPrev;
					pHeader->pPrev = pNew;
				}
				else
				{
					// xushiwei 2005-9-9 分配内存没有记录，导致内存泄漏
					m_end = m_begin = pNew->buffer;
					pNew->pPrev = NULL;
				}
				return pNew->buffer;
			}
			else
			{
				_MemBlock* pNew = (_MemBlock*)m_alloc.Alloc(sizeof(_MemBlock));
				pNew->pPrev = _ChainHeader();
				m_begin = pNew->buffer;
				m_end = m_begin + BlockSize;
			}
		}
		return m_end -= cb;
	}

	void* Alloc(size_t cb, FnDestructor fn)
	{
		_DestroyNode* pNode = (_DestroyNode*)Alloc(sizeof(_DestroyNode) + cb);
		pNode->fnDestroy = fn;
		pNode->pPrev = m_destroyChain;
		m_destroyChain = pNode;
		return pNode + 1;
	}

	void* Alloc(size_t cb, int fnZero)
	{
		return Alloc(cb);
	}

#if 0
	void* AllocArray(size_t cbType, size_t count, FnDestructor fn)
	{
		ArrayDestructHeader* pHeader = (ArrayDestructHeader*)
			Alloc(sizeof(ArrayDestructHeader) + cbType*count, fn);

		pHeader->count = count;

		return pHeader + 1;
	}

	void* AllocArray(size_t cbType, size_t count, int fnZero = 0)
	{
		return Alloc(cbType*count);
	}
#endif

#if defined(_DEBUG)
	void* Alloc(size_t cb, FnDestructor fn, const char* szFile, int nLine)
	{
		return Alloc(cb, fn);
	}

	void* Alloc(size_t cb, int fnZero, const char* szFile, int nLine)
	{
		return Alloc(cb);
	}

	void* AllocArray(
		size_t cbType, size_t count, FnDestructor fn,
		const char* szFile, int nLine)
	{
		return AllocArray(cbType, count, fn);
	}

	void* AllocArray(
		size_t cbType, size_t count, int fnZero,
		const char* szFile, int nLine)
	{
		return AllocArray(cbType, count, fnZero);
	}
#endif // defined(_DEBUG)
};

#ifndef _KfcNoDestructor
#define _KfcNoDestructor(type)	DeclareNoDestructor(type)
#endif

#ifndef _KfcDestructorOf
#define _KfcDestructorOf(type)	DestructorTraits< type >::Destruct
#endif

#ifndef _AutoFreeAlloc
#define _AutoFreeAlloc(alloc, type)	\
	( (type*) (alloc)->Alloc(sizeof(type)) )
#endif

#ifndef _AutoFreeAllocArray
#define _AutoFreeAllocArray(alloc, type, count)	\
	( (type*) (alloc)->Alloc((count)*sizeof(type)) )
#endif

#ifndef _AutoFreeNew
#define _AutoFreeNew(alloc, type)	\
	new( (alloc)->Alloc(sizeof(type), _KfcDestructorOf(type)) ) type
#endif

#ifndef _AutoFreeNew1Arg
#define _AutoFreeNew1Arg(alloc, type, arg)	\
	_AutoFreeNew(alloc, type)(arg)
#endif

#ifndef _AutoFreeNew2Arg
#define _AutoFreeNew2Arg(alloc, type, arg1, arg2)	\
	_AutoFreeNew(alloc, type)(arg1, arg2)
#endif

#ifndef _AutoFreeNew3Arg
#define _AutoFreeNew3Arg(alloc, type, arg1, arg2, arg3)	\
	_AutoFreeNew(alloc, type)(arg1, arg2, arg3)
#endif

#ifndef _AutoFreeNew4Arg
#define _AutoFreeNew4Arg(alloc, type, arg1, arg2, arg3, arg4)	\
	_AutoFreeNew(alloc, type)(arg1, arg2, arg3, arg4)
#endif

#ifndef _AutoFreeNew5Arg
#define _AutoFreeNew5Arg(alloc, type, arg1, arg2, arg3, arg4, arg5)	\
	_AutoFreeNew(alloc, type)(arg1, arg2, arg3, arg4, arg5)
#endif

#ifndef _MsoNoDestructor
#define _MsoNoDestructor(type)				_KfcNoDestructor(type)
#endif

#ifndef _MsoDestructorOf
#define _MsoDestructorOf(type)				_KfcDestructorOf(type)
#endif

#ifndef _MsoAlloc
#define _MsoAlloc(alloc, type)				_AutoFreeAlloc(alloc, type)
#endif

#ifndef _MsoAllocArray
#define _MsoAllocArray(alloc, type, count)	_AutoFreeAllocArray(alloc, type, count)
#endif

#ifndef _MsoNew
#define _MsoNew(alloc, type)				_AutoFreeNew(alloc, type)
#endif

#ifndef _MsoNew1Arg
#define _MsoNew1Arg(alloc, type, arg)		_AutoFreeNew1Arg(alloc, type, arg)
#endif

#ifndef _MsoNew2Arg
#define _MsoNew2Arg(alloc, type, arg1, arg2)	\
	_AutoFreeNew2Arg(alloc, type, arg1, arg2)
#endif

typedef TAutoFreeAlloc<DefaultStaticAlloc> AutoFreeAlloc;

// -------------------------------------------------------------------------
// --> support:
//	Type* o = new(alloc) Type;
//	Type* o = new(alloc) Type[nCount];

#ifdef MEMORY_SUPPORT_NEW
#define NEW_BY_AUTOFREE_ALLOC(Type)											\
public:																		\
	void* operator new(size_t, void* p)		{ return p; }					\
	void operator delete(void*, void*)		{}								\
																			\
	template <class AllocType>												\
	void* operator new(size_t cb, AllocType& alloc)							\
	{																		\
		MEMORY_STATIC_ASSERT(AllocType::IsAutoFreeAlloctor);				\
		MEMORY_ASSERT(sizeof(Type) == cb);									\
		return alloc.Alloc(													\
			sizeof(Type),													\
			std::DestructorTraits<Type>::Destruct);							\
	}																		\
																			\
	template <class AllocType>												\
	void* operator new[](size_t cb, AllocType& alloc)						\
	{																		\
		cb -= sizeof(size_t);												\
		MEMORY_STATIC_ASSERT(AllocType::IsAutoFreeAlloctor);				\
		MEMORY_ASSERT(cb % sizeof(Type) == 0);								\
		return alloc.AllocArray(											\
			sizeof(Type), cb/sizeof(Type),									\
			std::DestructorTraits<Type>::ArrayDestruct);					\
	}																		\
																			\
	template <class AllocType>												\
	void operator delete(void* p, AllocType& alloc)							\
	{																		\
		MEMORY_STATIC_ASSERT(AllocType::IsAutoFreeAlloctor);				\
		MEMORY_ASSERT(!"don't call me!");									\
	}																		\
																			\
	template <class AllocType>												\
	void operator delete[](void* p, AllocType& alloc)						\
	{																		\
		MEMORY_STATIC_ASSERT(AllocType::IsAutoFreeAlloctor);				\
		MEMORY_ASSERT(!"don't call me!");									\
	}																		\
																			\
private:																	\
	void operator delete(void* p)	{ MEMORY_ASSERT(!"don't call me!"); }	\
	void operator delete[](void* p)	{ MEMORY_ASSERT(!"don't call me!"); }

#endif

// -------------------------------------------------------------------------
//	$Log: autofree.h,v $
//	Revision 1.13  2006/03/15 02:39:50  xushiwei
//	增加memory leak checker
//
//	Revision 1.12  2006/03/14 03:30:06  xushiwei
//	引入STDCALL
//
//	Revision 1.11  2006/03/14 03:05:05  xushiwei
//	引入: MEMORY_BLOCK_SIZE
//
//	Revision 1.10  2006/02/28 01:01:50  xushiwei
//	1、memory支持VC++ 6.0
//	2、std::New支持两个参数的构造
//
//	Revision 1.9  2005/12/06 05:49:50  xushiwei
//	1、public MemBlockSize
//	2、增加构造: TAutoFreeAlloc(_Alloc alloc);
//
//	Revision 1.6  2005/11/29 09:13:06  xushiwei
//	vs2005警告
//
//	Revision 1.5  2005/09/09 09:03:38  wanghui
//	分配内存没有记录，导致内存泄漏
//
//	Revision 1.4  2005/07/28 01:20:21  xushiwei
//	考虑pHeader为NULL的情况。
//
//	Revision 1.2  2005/07/22 07:18:23  xushiwei
//	定义MEMORY_NO_TASKALLOC，允许没有TaskAlloc。
//
//	Revision 1.1  2005/07/19 04:59:57  xushiwei
//	AutoFreeAlloc
//

}

#endif /* __MEMORY_AUTOFREE_H__ */

