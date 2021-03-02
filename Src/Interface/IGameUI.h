#ifndef IGAMEUI_H
#define IGAMEUI_H

#include "interface.h"

class IGameUI : public IBaseInterface
{
public:
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
};

#define GAMEUI_INTERFACE_VERSION "GameUI007"

#endif