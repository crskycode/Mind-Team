#ifndef KEYVALUES_H
#define KEYVALUES_H

class CBuffer;

class NewKeyValues
{
public:
	NewKeyValues(const char *setName);

	// set/get name
	virtual const char *GetName(void);
	virtual void SetName(const char *setName);

	// load/save file
	virtual bool LoadFromFile(const char *resourceName);
	virtual bool SaveToFile(const char *resourceName);

	// load from text buffer
	virtual bool LoadFromBuffer(const char *pBuffer);
	virtual bool SaveToBuffer(char *pBuffer, size_t *iSize);

	// find a subkey
	virtual NewKeyValues *FindKey(const char *keyName, bool bCreate = false);

	// craete a subkey
	virtual NewKeyValues *CreateNewKey(void);
	virtual NewKeyValues *CreateKey(const char *keyName);

	// add/remove sub key
	virtual void AddSubKey(NewKeyValues *subKey);
	virtual void RemoveSubKey(NewKeyValues *subKey);

	virtual NewKeyValues *GetNextKey(void);
	virtual void SetNextKey(NewKeyValues *dat);

	// get key
	virtual NewKeyValues *GetFirstSubKey(void);
	virtual NewKeyValues *GetNextSubKey(void);
	virtual NewKeyValues *GetFirstValue(void);
	virtual NewKeyValues *GetNextValue(void);

	// if not subkey return true
	virtual bool IsEmpty(const char *keyName = NULL);

	// set value
	virtual const char *GetString(const char *keyName = NULL, const char *defaultValue = "");
	virtual int GetInt(const char *keyName = NULL, int defaultValue = 0);
	virtual float GetFloat(const char *keyName = NULL, float defaultValue = 0);

	// get value
	virtual void SetString(const char *keyName, const char *value);
	virtual void SetInt(const char *keyName, int value);
	virtual void SetFloat(const char *keyName, float value);

	// remove all key/value
	virtual void Clear(void);
	virtual void deleteThis(void);

private:
	~NewKeyValues(void);

	void RemoveEverything(void);

	void RecursiveLoadFromBuffer(CBuffer &buf);
	void RecursiveSaveToBuffer(CBuffer &buf, int indentLevel);

	void WriteIndents(CBuffer &buf, int indentLevel);

	void Init(void);
	bool ReadToken(char *token, CBuffer &buf);

	char *m_szName;

	char *m_szValue;
	int m_iValue;
	float m_flValue;

	NewKeyValues *m_pPeer;
	NewKeyValues *m_pSub;
};

#endif