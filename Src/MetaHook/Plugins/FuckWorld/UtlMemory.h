#ifndef UTLMEMORY_H
#define UTLMEMORY_H

#include <malloc.h>
#include <string.h>
#include <new.h>

template< class T >
inline void Construct(T* pMemory)
{
	new(pMemory)T;
}

template< class T >
inline void CopyConstruct(T* pMemory, T const& src)
{
	new(pMemory)T(src);
}

template< class T >
inline void Destruct(T* pMemory)
{
	pMemory->~T();
}

template< class T >
class CUtlMemory
{
public:
	CUtlMemory(int nGrowSize = 0, int nInitSize = 0);
	CUtlMemory(T* pMemory, int numElements);
	~CUtlMemory();

	T& operator[](int i);
	T const& operator[](int i) const;
	T& Element(int i);
	T const& Element(int i) const;

	bool IsIdxValid(int i) const;

	T* Base();
	T const* Base() const;

	void SetExternalBuffer(T* pMemory, int numElements);
	int NumAllocated() const;
	int Count() const;
	void Grow(int num = 1);
	void EnsureCapacity(int num);
	void Purge();
	bool IsExternallyAllocated() const;
	void SetGrowSize(int size);

private:
	enum
	{
		EXTERNAL_BUFFER_MARKER = -1,
	};

	T* m_pMemory;
	int m_nAllocationCount;
	int m_nGrowSize;
};

template< class T >
CUtlMemory<T>::CUtlMemory(int nGrowSize, int nInitAllocationCount) : m_pMemory(0),
m_nAllocationCount(nInitAllocationCount), m_nGrowSize(nGrowSize)
{
	if (m_nAllocationCount)
	{
		m_pMemory = (T*)malloc(m_nAllocationCount * sizeof(T));
	}
}

template< class T >
CUtlMemory<T>::CUtlMemory(T* pMemory, int numElements) : m_pMemory(pMemory),
m_nAllocationCount(numElements)
{
	m_nGrowSize = EXTERNAL_BUFFER_MARKER;
}

template< class T >
CUtlMemory<T>::~CUtlMemory()
{
	Purge();
}

template< class T >
void CUtlMemory<T>::SetExternalBuffer(T* pMemory, int numElements)
{
	Purge();

	m_pMemory = pMemory;
	m_nAllocationCount = numElements;
	m_nGrowSize = EXTERNAL_BUFFER_MARKER;
}


template< class T >
inline T& CUtlMemory<T>::operator[](int i)
{
	return m_pMemory[i];
}

template< class T >
inline T const& CUtlMemory<T>::operator[](int i) const
{
	return m_pMemory[i];
}

template< class T >
inline T& CUtlMemory<T>::Element(int i)
{
	return m_pMemory[i];
}

template< class T >
inline T const& CUtlMemory<T>::Element(int i) const
{
	return m_pMemory[i];
}

template< class T >
bool CUtlMemory<T>::IsExternallyAllocated() const
{
	return m_nGrowSize == EXTERNAL_BUFFER_MARKER;
}


template< class T >
void CUtlMemory<T>::SetGrowSize(int nSize)
{
	m_nGrowSize = nSize;
}

template< class T >
inline T* CUtlMemory<T>::Base()
{
	return m_pMemory;
}

template< class T >
inline T const* CUtlMemory<T>::Base() const
{
	return m_pMemory;
}

template< class T >
inline int CUtlMemory<T>::NumAllocated() const
{
	return m_nAllocationCount;
}

template< class T >
inline int CUtlMemory<T>::Count() const
{
	return m_nAllocationCount;
}

template< class T >
inline bool CUtlMemory<T>::IsIdxValid(int i) const
{
	return (i >= 0) && (i < m_nAllocationCount);
}

template< class T >
void CUtlMemory<T>::Grow(int num)
{
	if (IsExternallyAllocated())
	{
		return;
	}

	int nAllocationRequested = m_nAllocationCount + num;
	while (m_nAllocationCount < nAllocationRequested)
	{
		if (m_nAllocationCount != 0)
		{
			if (m_nGrowSize)
			{
				m_nAllocationCount += m_nGrowSize;
			}
			else
			{
				m_nAllocationCount += m_nAllocationCount;
			}
		}
		else
		{
			m_nAllocationCount = (31 + sizeof(T)) / sizeof(T);
		}
	}

	if (m_pMemory)
	{
		m_pMemory = (T*)realloc(m_pMemory, m_nAllocationCount * sizeof(T));
	}
	else
	{
		m_pMemory = (T*)malloc(m_nAllocationCount * sizeof(T));
	}
}

template< class T >
inline void CUtlMemory<T>::EnsureCapacity(int num)
{
	if (m_nAllocationCount >= num)
		return;

	if (IsExternallyAllocated())
	{
		return;
	}

	m_nAllocationCount = num;
	if (m_pMemory)
	{
		m_pMemory = (T*)realloc(m_pMemory, m_nAllocationCount * sizeof(T));
	}
	else
	{
		m_pMemory = (T*)malloc(m_nAllocationCount * sizeof(T));
	}
}

template< class T >
void CUtlMemory<T>::Purge()
{
	if (!IsExternallyAllocated())
	{
		if (m_pMemory)
		{
			free((void*)m_pMemory);
			m_pMemory = 0;
		}
		m_nAllocationCount = 0;
	}
}

#endif