#ifndef BUFFER_H
#define BUFFER_H

#include <malloc.h>
#include <memory.h>

typedef unsigned char byte;

enum seek_t
{
	seek_set,
	seek_cur,
	seek_end,
};

#pragma warning(push)
#pragma warning(disable:4018)

class CBuffer
{
public:
	CBuffer()
	{
		m_pbuf = NULL;
		m_size = 0;
		m_seek = 0;
	}

	CBuffer(size_t size)
	{
		m_pbuf = (byte *)malloc(size);
		m_size = size;
		m_seek = 0;
	}

	~CBuffer()
	{
		if (m_pbuf)
		{
			free(m_pbuf);
			m_pbuf = NULL;
		}

		m_size = 0;
		m_seek = 0;
	}

private:
	byte *GetSpace(size_t size)
	{
		if (!m_pbuf)
		{
			m_pbuf = (byte *)malloc(size);
			m_size = size;
		}

		if (m_seek + size > m_size)
		{
			m_size += size;
			m_pbuf = (byte *)realloc(m_pbuf, m_size);
		}

		byte *p = m_pbuf + m_seek;
		m_seek += size;
		
		return p;
	}

public:
	void Write(const void *data, size_t size)
	{
		byte *p = GetSpace(size);
		memcpy(p, data, size);
	}

	bool Read(void *pbuf, size_t size)
	{
		if (m_pbuf && m_seek < m_size)
		{
			size_t len = size;

			if (len > m_seek + m_size)
				len = m_size - m_seek;

			byte *p = m_pbuf + m_seek;
			m_seek += size;

			memcpy(pbuf, p, len);

			return true;
		}

		return false;
	}

	char getc(void)
	{
		if (m_pbuf && m_seek < m_size)
		{
			byte *p = m_pbuf + m_seek;
			m_seek += 1;

			return (char)*p;
		}

		return 0;
	}

	bool Seek(seek_t pos, int ofs)
	{
		switch (pos)
		{
		case seek_set:
			if (ofs >= 0 && ofs <= m_size)
			{
				m_seek = ofs;
				return true;
			}
			break;
		case seek_cur:
			if (m_seek + ofs >= 0 && m_seek + ofs <= m_size)
			{
				m_seek += ofs;
				return true;
			}
			break;
		case seek_end:
			if (m_size + ofs >= 0 && m_size + ofs <= m_size)
			{
				m_seek = m_size + ofs;
				return true;
			}
			break;
		}

		return false;
	}

	void *Get(void)
	{
		return m_pbuf;
	}

	size_t Size(void)
	{
		return m_size;
	}

	size_t Tell(void)
	{
		return m_seek;
	}

private:
	byte *m_pbuf;

	size_t m_size;
	size_t m_seek;
};

#pragma warning(pop)

#endif