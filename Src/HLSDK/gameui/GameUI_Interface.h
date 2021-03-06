#ifndef GAMEUI_INTERFACE_H
#define GAMEUI_INTERFACE_H

#include <interface.h>

class CGameUI : public IBaseInterface
{
public:
	CGameUI();
	~CGameUI();

	virtual void Initialize(CreateInterfaceFn *factories, int count);
	virtual void Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, void *system);
	virtual void Shutdown(void);
	virtual int ActivateGameUI(void);
	virtual int ActivateDemoUI(void);
	virtual int HasExclusiveInput(void);
	virtual void RunFrame(void);
	virtual void ConnectToServer(const char *game, int IP, int port);
	virtual void DisconnectFromServer(void);
	virtual void HideGameUI(void);
	virtual bool IsGameUIActive(void);
	virtual void LoadingStarted(const char *resourceType, const char *resourceName);
	virtual void LoadingFinished(const char *resourceType, const char *resourceName);
	virtual void StartProgressBar(const char *progressType, int progressSteps);
	virtual int ContinueProgressBar(int progressPoint, float progressFraction);
	virtual void StopProgressBar(bool bError, const char *failureReason, const char *extendedReason);
	virtual int SetProgressBarStatusText(const char *statusText);
	virtual void SetSecondaryProgressBar(float progress);
	virtual void SetSecondaryProgressBarText(const char *statusText);
	virtual void PaddingFunction1(int param1, int param2) {}
	virtual void PaddingFunction2(int param1, int param2) {}
	virtual void OnDisconnectFromServer(int eSteamLoginFailure, const char *username);
private:
	bool m_bActivatedUI;
};

#define GAMEUI_INTERFACE_VERSION "GameUI007"

#endif