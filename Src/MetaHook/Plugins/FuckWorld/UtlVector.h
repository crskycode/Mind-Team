#ifndef UTLVECTOR_H
#define UTLVECTOR_H

#include <string.h>
#include "UtlMemory.h"

template< class T >
class CUtlVector
{
public:
	typedef T ElemType_t;

	CUtlVector(int growSize = 0, int initSize = 0);
	CUtlVector(T* pMemory, int numElements);
	~CUtlVector();

	CUtlVector<T>& operator=(const CUtlVector<T> &other);

	T& operator[](int i);
	T const& operator[](int i) const;
	T& Element(int i);
	T const& Element(int i) const;

	T* Base();
	T const* Base() const;

	int Count() const;
	int Size() const;

	bool IsValidIndex(int i) const;
	static int InvalidIndex(void);

	int AddToHead();
	int AddToTail();
	int InsertBefore(int elem);
	int InsertAfter(int elem);

	int AddToHead(T const& src);
	int AddToTail(T const& src);
	int InsertBefore(int elem, T const& src);
	int InsertAfter(int elem, T const& src);

	int AddMultipleToHead(int num);
	int AddMultipleToTail(int num, const T *pToCopy = NULL);
	int InsertMultipleBefore(int elem, int num, const T *pToCopy = NULL);
	int InsertMultipleAfter(int elem, int num);

	void SetSize(int size);
	void SetCount(int count);

	void CopyArray(T const *pArray, int size);

	int AddVectorToTail(CUtlVector<T> const &src);

	int Find(T const& src) const;

	bool HasElement(T const& src);

	void EnsureCapacity(int num);

	void EnsureCount(int num);

	void FastRemove(int elem);
	void Remove(int elem);
	void FindAndRemove(T const& src);
	void RemoveMultiple(int elem, int num);
	void RemoveAll();

	void Purge();

	void PurgeAndDeleteElements();

	void SetGrowSize(int size);

protected:
	CUtlVector(CUtlVector const& vec) { return; }

	void GrowVector(int num = 1);

	void ShiftElementsRight(int elem, int num = 1);
	void ShiftElementsLeft(int elem, int num = 1);

	void ResetDbgInfo();

	CUtlMemory<T> m_Memory;
	int m_Size;

	T *m_pElements;
};

template< class T >
inline void CUtlVector<T>::ResetDbgInfo()
{
	m_pElements = m_Memory.Base();
}

template< class T >
inline CUtlVector<T>::CUtlVector(int growSize, int initSize) :
m_Memory(growSize, initSize), m_Size(0)
{
	ResetDbgInfo();
}

template< class T >
inline CUtlVector<T>::CUtlVector(T* pMemory, int numElements) :
m_Memory(pMemory, numElements), m_Size(0)
{
	ResetDbgInfo();
}

template< class T >
inline CUtlVector<T>::~CUtlVector()
{
	Purge();
}

template<class T>
inline CUtlVector<T>& CUtlVector<T>::operator=(const CUtlVector<T> &other)
{
	CopyArray(other.Base(), other.Count());
	return *this;
}

template< class T >
inline T& CUtlVector<T>::operator[](int i)
{
	return m_Memory[i];
}

template< class T >
inline T const& CUtlVector<T>::operator[](int i) const
{
	return m_Memory[i];
}

template< class T >
inline T& CUtlVector<T>::Element(int i)
{
	return m_Memory[i];
}

template< class T >
inline T const& CUtlVector<T>::Element(int i) const
{
	return m_Memory[i];
}

template< class T >
inline T* CUtlVector<T>::Base()
{
	return m_Memory.Base();
}

template< class T >
inline T const* CUtlVector<T>::Base() const
{
	return m_Memory.Base();
}

template< class T >
inline int CUtlVector<T>::Size() const
{
	return m_Size;
}

template< class T >
inline int CUtlVector<T>::Count() const
{
	return m_Size;
}

template< class T >
inline bool CUtlVector<T>::IsValidIndex(int i) const
{
	return (i >= 0) && (i < m_Size);
}

template< class T >
inline int CUtlVector<T>::InvalidIndex(void)
{
	return -1;
}

template< class T >
void CUtlVector<T>::GrowVector(int num)
{
	if (m_Size + num - 1 >= m_Memory.NumAllocated())
	{
		m_Memory.Grow(m_Size + num - m_Memory.NumAllocated());
	}

	m_Size += num;
	ResetDbgInfo();
}

template< class T >
void CUtlVector<T>::EnsureCapacity(int num)
{
	m_Memory.EnsureCapacity(num);
	ResetDbgInfo();
}

template< class T >
void CUtlVector<T>::EnsureCount(int num)
{
	if (Count() < num)
		AddMultipleToTail(num - Count());
}

template< class T >
void CUtlVector<T>::ShiftElementsRight(int elem, int num)
{
	int numToMove = m_Size - elem - num;
	if ((numToMove > 0) && (num > 0))
		memmove(&Element(elem + num), &Element(elem), numToMove * sizeof(T));
}

template< class T >
void CUtlVector<T>::ShiftElementsLeft(int elem, int num)
{
	int numToMove = m_Size - elem - num;
	if ((numToMove > 0) && (num > 0))
	{
		memmove(&Element(elem), &Element(elem + num), numToMove * sizeof(T));

#ifdef _DEBUG
		memset(&Element(m_Size - num), 0xDD, num * sizeof(T));
#endif
	}
}

template< class T >
inline int CUtlVector<T>::AddToHead()
{
	return InsertBefore(0);
}

template< class T >
inline int CUtlVector<T>::AddToTail()
{
	return InsertBefore(m_Size);
}

template< class T >
inline int CUtlVector<T>::InsertAfter(int elem)
{
	return InsertBefore(elem + 1);
}

template< class T >
int CUtlVector<T>::InsertBefore(int elem)
{
	GrowVector();
	ShiftElementsRight(elem);
	Construct(&Element(elem));
	return elem;
}

template< class T >
inline int CUtlVector<T>::AddToHead(T const& src)
{
	return InsertBefore(0, src);
}

template< class T >
inline int CUtlVector<T>::AddToTail(T const& src)
{
	return InsertBefore(m_Size, src);
}

template< class T >
inline int CUtlVector<T>::InsertAfter(int elem, T const& src)
{
	return InsertBefore(elem + 1, src);
}

template< class T >
int CUtlVector<T>::InsertBefore(int elem, T const& src)
{
	GrowVector();
	ShiftElementsRight(elem);
	CopyConstruct(&Element(elem), src);
	return elem;
}

template< class T >
inline int CUtlVector<T>::AddMultipleToHead(int num)
{
	return InsertMultipleBefore(0, num);
}

template< class T >
inline int CUtlVector<T>::AddMultipleToTail(int num, const T *pToCopy)
{
	return InsertMultipleBefore(m_Size, num, pToCopy);
}

template< class T >
int CUtlVector<T>::InsertMultipleAfter(int elem, int num)
{
	return InsertMultipleBefore(elem + 1, num);
}


template< class T >
void CUtlVector<T>::SetCount(int count)
{
	RemoveAll();
	AddMultipleToTail(count);
}

template< class T >
inline void CUtlVector<T>::SetSize(int size)
{
	SetCount(size);
}

template< class T >
void CUtlVector<T>::CopyArray(T const *pArray, int size)
{
	SetSize(size);
	for (int i = 0; i < size; i++)
		(*this)[i] = pArray[i];
}

template< class T >
int CUtlVector<T>::AddVectorToTail(CUtlVector const &src)
{
	int base = Count();

	AddMultipleToTail(src.Count());

	for (int i = 0; i < src.Count(); i++)
		(*this)[base + i] = src[i];

	return base;
}

template< class T >
inline int CUtlVector<T>::InsertMultipleBefore(int elem, int num, const T *pToInsert)
{
	if (num == 0)
		return elem;

	GrowVector(num);
	ShiftElementsRight(elem, num);

	for (int i = 0; i < num; ++i)
		Construct(&Element(elem + i));

	if (pToInsert)
	{
		for (int i = 0; i < num; i++)
		{
			Element(elem + i) = pToInsert[i];
		}
	}

	return elem;
}

template< class T >
int CUtlVector<T>::Find(T const& src) const
{
	for (int i = 0; i < Count(); ++i)
	{
		if (Element(i) == src)
			return i;
	}
	return -1;
}

template< class T >
bool CUtlVector<T>::HasElement(T const& src)
{
	return (Find(src) >= 0);
}

template< class T >
void CUtlVector<T>::FastRemove(int elem)
{
	Destruct(&Element(elem));
	if (m_Size > 0)
	{
		memcpy(&Element(elem), &Element(m_Size - 1), sizeof(T));
		--m_Size;
	}
}

template< class T >
void CUtlVector<T>::Remove(int elem)
{
	Destruct(&Element(elem));
	ShiftElementsLeft(elem);
	--m_Size;
}

template< class T >
void CUtlVector<T>::FindAndRemove(T const& src)
{
	int elem = Find(src);
	if (elem != -1)
	{
		Remove(elem);
	}
}

template< class T >
void CUtlVector<T>::RemoveMultiple(int elem, int num)
{
	for (int i = elem + num; --i >= elem;)
		Destruct(&Element(i));

	ShiftElementsLeft(elem, num);
	m_Size -= num;
}

template< class T >
void CUtlVector<T>::RemoveAll()
{
	for (int i = m_Size; --i >= 0;)
		Destruct(&Element(i));

	m_Size = 0;
}

template< class T >
void CUtlVector<T>::Purge()
{
	RemoveAll();
	m_Memory.Purge();
	ResetDbgInfo();
}


template<class T>
inline void CUtlVector<T>::PurgeAndDeleteElements()
{
	for (int i = 0; i < m_Size; i++)
		delete Element(i);

	Purge();
}


template< class T >
void CUtlVector<T>::SetGrowSize(int size)
{
	m_Memory.SetGrowSize(size);
}

#endif