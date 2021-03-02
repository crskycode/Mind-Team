#ifndef GAMECONSOLE_H
#define GAMECONSOLE_H

#include <interface.h>
#include "GameConsoleDialog.h"

class CGameConsole : public IBaseInterface
{
public:
	CGameConsole();
	~CGameConsole();

	virtual void Activate(void);
	virtual void Initialize(void);
	virtual void Hide(void);
	virtual void Clear(void);
	virtual bool IsConsoleVisible(void);
	virtual void Printf(const char *format, ...);
	virtual void DPrintf(const char *format, ...);
	virtual void SetParent(int parent);

public:
	bool m_bInitialized;
	CGameConsoleDialog *m_pConsole;
};

#define GAMECONSOLE_INTERFACE_VERSION "GameConsole003"

CGameConsole &GameConsole();

#endif