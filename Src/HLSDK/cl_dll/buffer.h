#ifndef BUFFER_H
#define BUFFER_H

typedef unsigned char byte;

class CBufferWriter
{
public:
	CBufferWriter(byte *pBuffer, int iSize);

	void BeginWrite(void);
	void WriteChar(char value);
	void WriteUChar(unsigned char value);
	void WriteShort(short value);
	void WriteUShort(unsigned short value);
	void WriteInt(int value);
	void WriteUInt(unsigned int value);
	void WriteFloat(float value);
	void WriteDouble(double value);
	void WriteString(const char *value);
	void WriteData(byte *value, int size);

	int GetDataSize(void);

private:
	byte *GetWriteSpace(int iSize);

	byte *m_pBuffer;
	int m_iSize;
	int m_iWritePos;
};

class CBufferReader
{
public:
	CBufferReader(byte *pBuffer, int iSize);

	void BeginRead(void);
	char ReadChar(void);
	unsigned char ReadUChar(void);
	short ReadShort(void);
	unsigned short ReadUShort(void);
	int ReadInt(void);
	unsigned int ReadUInt(void);
	float ReadFloat(void);
	double ReadDouble(void);
	const char *ReadString(void);

private:
	byte *GetReadSpace(int iSize);

	byte *m_pBuffer;
	int m_iSize;
	int m_iReadPos;
};

#endif