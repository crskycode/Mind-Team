#ifndef CEVALUE_H
#define CEVALUE_H

typedef unsigned char BYTE;

#ifdef NDEBUG

template <typename T>
class CEValue
{
public:
	CEValue()
	{
		for (int i = 0; i < sizeof(T); i++)
		{
			m_Memory[i] = 0 ^ (i + 73);
		}
	}

	void operator =(T src)
	{
		for (int i = 0; i < sizeof(T); i++)
		{
			m_Memory[i] = ((BYTE *)&src)[i] ^ (i + 73);
		}
	}

	operator T()
	{
		BYTE temp[sizeof(T)];

		for (int i = 0; i < sizeof(T); i++)
		{
			temp[i] = m_Memory[i] ^ (i + 73);
		}

		return *(T *)temp;
	}

	T operator ->()
	{
		BYTE temp[sizeof(T)];

		for (int i = 0; i < sizeof(T); i++)
		{
			temp[i] = m_Memory[i] ^ (i + 73);
		}

		return *(T *)temp;
	}

	BYTE m_Memory[sizeof(T)];
};

#else

template <typename T>
class CEValue
{
public:
	CEValue()
	{
	}

	void operator =(T src)
	{
		m_Memory = src;
	}

	operator T()
	{
		return m_Memory;
	}

	T operator ->()
	{
		return m_Memory;
	}

	T m_Memory;
};

#endif

#endif