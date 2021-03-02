
#include <stddef.h>
#include <string.h>
#include "buffer.h"

CBufferWriter::CBufferWriter(byte *pBuffer, int iSize)
{
	m_pBuffer = pBuffer;
	m_iSize = iSize;
	m_iWritePos = 0;
}

void CBufferWriter::BeginWrite(void)
{
	m_iWritePos = 0;
}

void CBufferWriter::WriteChar(char value)
{
	byte *p = GetWriteSpace(1);

	if (p != NULL)
	{
		*(char *)p = value;
	}
}

void CBufferWriter::WriteUChar(unsigned char value)
{
	byte *p = GetWriteSpace(1);

	if (p != NULL)
	{
		*(unsigned char *)p = value;
	}
}

void CBufferWriter::WriteShort(short value)
{
	byte *p = GetWriteSpace(2);

	if (p != NULL)
	{
		*(short *)p = value;
	}
}

void CBufferWriter::WriteUShort(unsigned short value)
{
	byte *p = GetWriteSpace(2);

	if (p != NULL)
	{
		*(unsigned short *)p = value;
	}
}

void CBufferWriter::WriteInt(int value)
{
	byte *p = GetWriteSpace(4);

	if (p != NULL)
	{
		*(int *)p = value;
	}
}

void CBufferWriter::WriteUInt(unsigned int value)
{
	byte *p = GetWriteSpace(4);

	if (p != NULL)
	{
		*(unsigned int *)p = value;
	}
}

void CBufferWriter::WriteFloat(float value)
{
	byte *p = GetWriteSpace(4);

	if (p != NULL)
	{
		*(float *)p = value;
	}
}

void CBufferWriter::WriteDouble(double value)
{
	byte *p = GetWriteSpace(8);

	if (p != NULL)
	{
		*(double *)p = value;
	}
}

void CBufferWriter::WriteString(const char *value)
{
	if (value != NULL)
	{
		int size = strlen(value) + 1;

		byte *p = GetWriteSpace(size);

		if (p != NULL)
		{
			memcpy(p, value, size);
		}
	}
}

void CBufferWriter::WriteData(byte *value, int size)
{
	if (value != NULL && size > 0)
	{
		byte *p = GetWriteSpace(size);

		if (p != NULL)
		{
			memcpy(p, value, size);
		}
	}
}

int CBufferWriter::GetDataSize(void)
{
	return m_iWritePos;
}

byte *CBufferWriter::GetWriteSpace(int iSize)
{
	if (m_iWritePos + iSize <= m_iSize)
	{
		byte *p = m_pBuffer + m_iWritePos;
		m_iWritePos += iSize;
		return p;
	}

	return NULL;
}

CBufferReader::CBufferReader(byte *pBuffer, int iSize)
{
	m_pBuffer = pBuffer;
	m_iSize = iSize;
	m_iReadPos = 0;
}

void CBufferReader::BeginRead(void)
{
	m_iReadPos = 0;
}

char CBufferReader::ReadChar(void)
{
	byte *p = GetReadSpace(1);

	if (p != NULL)
	{
		return *(char *)p;
	}

	return 0;
}

unsigned char CBufferReader::ReadUChar(void)
{
	byte *p = GetReadSpace(1);

	if (p != NULL)
	{
		return *(unsigned char *)p;
	}

	return 0;
}

short CBufferReader::ReadShort(void)
{
	byte *p = GetReadSpace(2);

	if (p != NULL)
	{
		return *(short *)p;
	}

	return 0;
}

unsigned short CBufferReader::ReadUShort(void)
{
	byte *p = GetReadSpace(2);

	if (p != NULL)
	{
		return *(unsigned short *)p;
	}

	return 0;
}

int CBufferReader::ReadInt(void)
{
	byte *p = GetReadSpace(4);

	if (p != NULL)
	{
		return *(int *)p;
	}

	return 0;
}

unsigned int CBufferReader::ReadUInt(void)
{
	byte *p = GetReadSpace(4);

	if (p != NULL)
	{
		return *(unsigned int *)p;
	}

	return 0;
}

float CBufferReader::ReadFloat(void)
{
	byte *p = GetReadSpace(4);

	if (p != NULL)
	{
		return *(float *)p;
	}

	return 0;
}

double CBufferReader::ReadDouble(void)
{
	byte *p = GetReadSpace(8);

	if (p != NULL)
	{
		return *(double *)p;
	}

	return 0;
}

const char *CBufferReader::ReadString(void)
{
	static char buf[1024];

	char *p = buf;

	do {
		*p = ReadChar();
	}
	while (*p++);

	return buf;
}

byte *CBufferReader::GetReadSpace(int iSize)
{
	if (m_iReadPos + iSize <= m_iSize)
	{
		byte *p = m_pBuffer + m_iReadPos;
		m_iReadPos += iSize;
		return p;
	}

	return NULL;
}