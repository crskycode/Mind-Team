#include <stdio.h>	// fopen fclose fread fwrite
#include <string.h>	// malloc free strlen
#include <stdlib.h>	// atoi atof
#include <ctype.h>	// isspace isdigit

#include "NewKeyValues.h"
#include "buffer.h"

NewKeyValues::NewKeyValues(const char *setName)
{
	m_szName = NULL;
	m_szValue = NULL;

	Init();
	SetName(setName);
}

NewKeyValues::~NewKeyValues(void)
{
	RemoveEverything();
}

const char *NewKeyValues::GetName(void)
{
	if (m_szName)
		return m_szName;

	return "";
}

void NewKeyValues::SetName(const char *setName)
{
	if (!setName)
		setName = "";

	if (m_szName)
	{
		free(m_szName);
		m_szName = NULL;
	}

	size_t len = strlen(setName);
	m_szName = (char *)malloc(len + 1);
	strcpy(m_szName, setName);
}

bool NewKeyValues::LoadFromFile(const char *resourceName)
{
	FILE *f = fopen(resourceName, "rb");
	if (!f)
		return false;

	int fileSize;

	fseek(f, 0, SEEK_END);
	fileSize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *buffer = (char *)malloc(fileSize + 1);

	fread(buffer, fileSize, 1, f);

	fclose(f);

	buffer[fileSize] = 0;
	LoadFromBuffer(buffer);

	free(buffer);

	return true;
}

bool NewKeyValues::SaveToFile(const char *resourceName)
{
	FILE *f = fopen(resourceName, "wb");
	if (!f)
		return false;

	CBuffer buf(0x10000); // 64KB
	RecursiveSaveToBuffer(buf, 0);

	fwrite(buf.Get(), buf.Tell(), 1, f);

	fclose(f);

	return true;
}

bool NewKeyValues::LoadFromBuffer(const char *pBuffer)
{
	CBuffer buf;
	char token[100];
	bool got;

	buf.Write(pBuffer, strlen(pBuffer) + 1);
	buf.Seek(seek_set, 0);

	RemoveEverything();

	NewKeyValues *pPreviousKey = NULL;
	NewKeyValues *pCurrentKey = this;

	while (1)
	{
		// get the key
		got = ReadToken(token, buf);

		// expected 'ident' but end of file
		if (!got)
			break;

		if (!pCurrentKey)
		{
			pCurrentKey = new NewKeyValues(token);

			if (pPreviousKey)
			{
				pPreviousKey->SetNextKey(pCurrentKey);
			}
		}
		else
		{
			// set name for this key
			pCurrentKey->SetName(token);
		}

		// get the value
		got = ReadToken(token, buf);

		// expected 'ident' or '{' but end of file
		if (!got)
			break;

		// get the key
		if (token[0] == '{')
		{
			pCurrentKey->RecursiveLoadFromBuffer(buf);
		}

		pPreviousKey = pCurrentKey;
		pCurrentKey = NULL;
	}

	return true;
}

bool NewKeyValues::SaveToBuffer(char *pBuffer, size_t *iSize)
{
	CBuffer buf(0x10000);
	RecursiveSaveToBuffer(buf, 0);

	memcpy(pBuffer, buf.Get(), buf.Tell());

	*iSize = buf.Tell();

	return true;
}

NewKeyValues *NewKeyValues::FindKey(const char *keyName, bool bCreate)
{
	if (!keyName || !keyName[0])
		return this;

	NewKeyValues *lastItem = NULL;
	NewKeyValues *dat;

	for (dat = m_pSub; dat != NULL; dat = dat->m_pPeer)
	{
		lastItem = dat;

		if (!strcmp(keyName, dat->m_szName))
		{
			break;
		}
	}

	if (!dat)
	{
		if (bCreate)
		{
			dat = new NewKeyValues(keyName);

			if (lastItem)
			{
				lastItem->m_pPeer = dat;
			}
			else
			{
				m_pSub = dat;
			}
		}
		else
		{
			return NULL;
		}
	}

	return dat;
}

NewKeyValues *NewKeyValues::CreateNewKey(void)
{
	int newID = 1;

	for (NewKeyValues *dat = m_pSub; dat != NULL; dat = dat->m_pPeer)
	{
		int val = atoi(dat->GetName());

		if (newID <= val)
		{
			newID = val + 1;
		}
	}

	char buf[12];
	sprintf(buf, "%d", newID);

	return CreateKey(buf);
}

NewKeyValues *NewKeyValues::CreateKey(const char *keyName)
{
	NewKeyValues *dat = new NewKeyValues(keyName);

	AddSubKey(dat);

	return dat;
}

void NewKeyValues::AddSubKey(NewKeyValues *subKey)
{
	if (m_pSub == NULL)
	{
		m_pSub = subKey;
	}
	else
	{
		NewKeyValues *pTempDat = m_pSub;

		while (pTempDat->GetNextKey() != NULL)
		{
			pTempDat = pTempDat->GetNextKey();
		}

		pTempDat->SetNextKey(subKey);
	}
}

void NewKeyValues::RemoveSubKey(NewKeyValues *subKey)
{
	if (!subKey)
		return;

	if (m_pSub == subKey)
	{
		m_pSub = m_pSub->m_pPeer;
	}
	else
	{
		NewKeyValues *dat = m_pSub;

		while (dat->m_pPeer)
		{
			if (dat->m_pPeer == subKey)
			{
				dat->m_pPeer = dat->m_pPeer->m_pPeer;
				break;
			}

			dat = dat->m_pPeer;
		}
	}

	subKey->m_pPeer = NULL;
}

NewKeyValues *NewKeyValues::GetNextKey(void)
{
	return m_pPeer;
}

void NewKeyValues::SetNextKey(NewKeyValues *dat)
{
	m_pPeer = dat;
}

NewKeyValues *NewKeyValues::GetFirstSubKey(void)
{
	NewKeyValues *dat = m_pSub;

	while (dat && !dat->m_pSub)
	{
		dat = dat->m_pPeer;
	}

	return dat;
}

NewKeyValues *NewKeyValues::GetNextSubKey(void)
{
	NewKeyValues *dat = m_pPeer;

	while (dat && !dat->m_pSub)
	{
		dat = dat->m_pPeer;
	}

	return dat;
}

NewKeyValues *NewKeyValues::GetFirstValue(void)
{
	NewKeyValues *dat = m_pSub;

	while (dat && dat->m_pSub)
	{
		dat = dat->m_pPeer;
	}

	return dat;
}

NewKeyValues *NewKeyValues::GetNextValue(void)
{
	NewKeyValues *dat = m_pPeer;

	while (dat && dat->m_pSub)
	{
		dat = dat->m_pPeer;
	}

	return dat;
}

bool NewKeyValues::IsEmpty(const char *keyName)
{
	NewKeyValues *dat = FindKey(keyName);

	if (!dat)
		return true;

	if (!dat->m_pSub)
		return true;

	return false;
}

const char *NewKeyValues::GetString(const char *keyName, const char *defaultValue)
{
	NewKeyValues *dat = FindKey(keyName);

	if (dat)
	{
		return dat->m_szValue;
	}

	return defaultValue;
}

int NewKeyValues::GetInt(const char *keyName, int defaultValue)
{
	NewKeyValues *dat = FindKey(keyName);

	if (dat)
	{
		return dat->m_iValue;
	}

	return defaultValue;
}

float NewKeyValues::GetFloat(const char *keyName, float defaultValue)
{
	NewKeyValues *dat = FindKey(keyName);

	if (dat)
	{
		return dat->m_flValue;
	}

	return defaultValue;
}

void NewKeyValues::SetString(const char *keyName, const char *value)
{
	NewKeyValues *dat = FindKey(keyName, true);

	if (dat)
	{
		if (dat->m_szValue)
		{
			free(dat->m_szValue);
			dat->m_szValue = NULL;
		}

		size_t len = strlen(value);
		dat->m_szValue = (char *)malloc(len + 1);
		strcpy(dat->m_szValue, value);
	}
}

void NewKeyValues::SetInt(const char *keyName, int value)
{
	NewKeyValues *dat = FindKey(keyName, true);

	if (dat)
	{
		if (dat->m_szValue)
		{
			free(dat->m_szValue);
			dat->m_szValue = NULL;
		}

		dat->m_szValue = (char *)malloc(16);
		sprintf(dat->m_szValue, "%d", value);

		dat->m_iValue = value;
		dat->m_flValue = value;
	}
}

void NewKeyValues::SetFloat(const char *keyName, float value)
{
	NewKeyValues *dat = FindKey(keyName, true);

	if (dat)
	{
		if (dat->m_szValue)
		{
			free(dat->m_szValue);
			dat->m_szValue = NULL;
		}

		dat->m_szValue = (char *)malloc(16);
		sprintf(dat->m_szValue, "%.6f", value);

		dat->m_flValue = value;
		dat->m_iValue = value;
	}
}

void NewKeyValues::Clear(void)
{
	delete m_pSub;
	m_pSub = NULL;
}

void NewKeyValues::deleteThis(void)
{
	delete this;
}

void NewKeyValues::RemoveEverything(void)
{
	NewKeyValues *dat;
	NewKeyValues *datNext = NULL;

	for (dat = m_pSub; dat != NULL; dat = datNext)
	{
		datNext = dat->m_pPeer;
		dat->m_pPeer = NULL;
		delete dat;
	}

	for (dat = m_pPeer; dat && dat != this; dat = datNext)
	{
		datNext = dat->m_pPeer;
		dat->m_pPeer = NULL;
		delete dat;
	}

	m_iValue = 0;
	m_flValue = 0;

	if (m_szValue)
	{
		free(m_szValue);
		m_szValue = NULL;
	}
}

static int GetStringType(const char *src)
{
	// is multi char ?
	if (*src <= 0)
		return 0;

	// is '-' or digit ?
	if (*src == '-' || isdigit(*src))
	{
		// "1"
		if (isdigit(*src) && !*(src + 1))
			return 1;

		++src; // next char

		// "-a" or "0a"
		if (!isdigit(*src) && *src != '.')
			return 0;

		while (*src)
		{
			// "1." or "-1."
			if (*src == '.')
			{
				++src; // next char

				// we need a digit, "1." not a float
				if (!*src)
					return 0;

				while (*src)
				{
					// "1.a"
					if (!isdigit(*src))
						return 0;
					++src;
				}
				// float value
				return 2;
			}

			// "10a" not a integer
			if (!isdigit(*src))
				return 0;

			++src; // next char
		}
		// integer value
		return 1;
	}

	return 0;
}

void NewKeyValues::RecursiveLoadFromBuffer(CBuffer &buf)
{
	char token[100];
	bool got;
	int type;

	while (1)
	{
		// get the key
		got = ReadToken(token, buf);

		// expected 'ident' or '}' but end of file
		if (!got)
			break;

		// close the key
		if (token[0] == '}')
			break;

		NewKeyValues *dat = CreateKey(token);

		// get the value
		got = ReadToken(token, buf);

		// expected '{' or 'ident' but end of file
		if (!got)
			break;

		// expected '{' or 'ident' but got '}'
		if (token[0] == '}')
			break;

		if (token[0] == '{')
		{
			dat->RecursiveLoadFromBuffer(buf);
		}
		else
		{
			type = GetStringType(token);

			if (type == 1)
			{
				dat->m_iValue = atoi(token);
				dat->m_flValue = dat->m_iValue;
			}
			else if (type == 2)
				dat->m_flValue = atof(token);

			if (dat->m_szValue)
			{
				free(dat->m_szValue);
				dat->m_szValue = NULL;
			}

			size_t len = strlen(token);
			dat->m_szValue = (char *)malloc(len + 1);
			strcpy(dat->m_szValue, token);
		}
	}
}

void NewKeyValues::RecursiveSaveToBuffer(CBuffer &buf, int indentLevel)
{
	WriteIndents(buf, indentLevel);
	buf.Write("\"", 1);
	buf.Write(m_szName, strlen(m_szName));
	buf.Write("\"\n", 2);
	WriteIndents(buf, indentLevel);
	buf.Write("{\n", 2);

	for (NewKeyValues *dat = m_pSub; dat != NULL; dat = dat->m_pPeer)
	{
		if (dat->m_pSub)
		{
			dat->RecursiveSaveToBuffer(buf, indentLevel + 1);
		}
		else
		{
			WriteIndents(buf, indentLevel + 1);
			buf.Write("\"", 1);
			buf.Write(dat->GetName(), strlen(dat->GetName()));
			buf.Write("\"\t\t\"", 4);
			buf.Write(dat->GetString(), strlen(dat->GetString()));
			buf.Write("\"\n", 2);
		}
	}

	WriteIndents(buf, indentLevel);
	buf.Write("}\n", 2);
}

void NewKeyValues::WriteIndents(CBuffer &buf, int indentLevel)
{
	for (int i = 0; i < indentLevel; ++i)
	{
		buf.Write("\t", 1);
	}
}

void NewKeyValues::Init(void)
{
	if (m_szName)
	{
		free(m_szName);
		m_szName = NULL;
	}

	if (m_szValue)
	{
		free(m_szValue);
		m_szValue = NULL;
	}

	m_iValue = 0;
	m_flValue = 0;

	m_pPeer = NULL;
	m_pSub = NULL;
}

bool NewKeyValues::ReadToken(char *token, CBuffer &buf)
{
	char *pw = token;
	char ch;

	while (1)
	{
skipwhite:
		do { ch = buf.getc(); } while ( isspace(ch) );

		if (ch == '/')
		{
			ch = buf.getc();

			if (ch == '/')
			{
				do { ch = buf.getc(); } while ( ch != '\n' );

				goto skipwhite;
			}
		}

		if (ch == '{' || ch == '}')
		{
			pw[0] = ch;
			pw[1] = 0;

			return true;
		}

		if (ch == '"')
		{
			do { *pw = buf.getc(); } while ( *(pw++) != '"' );

			*(--pw) = 0;

			return true;
		}

		if (!ch)
		{
			*pw = 0;

			return false;
		}
	}
}