#include <metahook.h>

#include <IBaseUI.h>
#include <ikeyvaluessystem.h>

IBaseUI *g_pBaseUI = 0;

void (__fastcall *g_pfnCBaseUI_Initialize)(void *pthis, int edx, CreateInterfaceFn *factories, int count) = 0;
void (__fastcall *g_pfnCBaseUI_Start)(void *pthis, int edx, struct cl_enginefuncs_s *engineFuncs, int interfaceVersion) = 0;
void (__fastcall *g_pfnCBaseUI_Shutdown)(void *pthis, int edx) = 0;
int (__fastcall *g_pfnCBaseUI_Key_Event)(void *pthis, int edx, int down, int keynum, const char *pszCurrentBinding) = 0;
void (__fastcall *g_pfnCBaseUI_CallEngineSurfaceProc)(void *pthis, int edx, void *hwnd, unsigned int msg, unsigned int wparam, long lparam) = 0;
void (__fastcall *g_pfnCBaseUI_Paint)(void *pthis, int edx, int x, int y, int right, int bottom) = 0;
void (__fastcall *g_pfnCBaseUI_HideGameUI)(void *pthis, int edx) = 0;
void (__fastcall *g_pfnCBaseUI_ActivateGameUI)(void *pthis, int edx) = 0;
bool (__fastcall *g_pfnCBaseUI_IsGameUIVisible)(void *pthis, int edx) = 0;
void (__fastcall *g_pfnCBaseUI_HideConsole)(void *pthis, int edx) = 0;
void (__fastcall *g_pfnCBaseUI_ShowConsole)(void *pthis, int edx) = 0;

class CBaseUI : public IBaseUI
{
public:
	virtual void Initialize(CreateInterfaceFn *factories, int count);
	virtual void Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion);
	virtual void Shutdown(void);
	virtual int Key_Event(int down, int keynum, const char *pszCurrentBinding);
	virtual void CallEngineSurfaceProc(void *hwnd, unsigned int msg, unsigned int wparam, long lparam);
	virtual void Paint(int x, int y, int right, int bottom);
	virtual void HideGameUI(void);
	virtual void ActivateGameUI(void);
	virtual bool IsGameUIVisible(void);
	virtual void HideConsole(void);
	virtual void ShowConsole(void);
};

void KeyValuesSystem_InstallHook(IKeyValuesSystem *pKeyValuesSystem);

void CBaseUI::Initialize(CreateInterfaceFn *factories, int count)
{
	HINTERFACEMODULE m_hVGuiModule = Sys_LoadModule("vgui2.dll");
	CreateInterfaceFn vguiFactory = Sys_GetFactory(m_hVGuiModule);

	IKeyValuesSystem *pKeyValuesSystem = (IKeyValuesSystem *)vguiFactory(KEYVALUESSYSTEM_INTERFACE_VERSION, NULL);
	KeyValuesSystem_InstallHook(pKeyValuesSystem);

	return g_pfnCBaseUI_Initialize(this, 0, factories, count);
}

void CBaseUI::Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion)
{
	return g_pfnCBaseUI_Start(this, 0, engineFuncs, interfaceVersion);
}

void CBaseUI::Shutdown(void)
{
	return g_pfnCBaseUI_Shutdown(this, 0);
}

int CBaseUI::Key_Event(int down, int keynum, const char *pszCurrentBinding)
{
	return g_pfnCBaseUI_Key_Event(this, 0, down, keynum, pszCurrentBinding);
}

void CBaseUI::CallEngineSurfaceProc(void *hwnd, unsigned int msg, unsigned int wparam, long lparam)
{
	return g_pfnCBaseUI_CallEngineSurfaceProc(this, 0, hwnd, msg, wparam, lparam);
}

void CBaseUI::Paint(int x, int y, int right, int bottom)
{
	return g_pfnCBaseUI_Paint(this, 0, x, y, right, bottom);
}

void CBaseUI::HideGameUI(void)
{
	return g_pfnCBaseUI_HideGameUI(this, 0);
}

void CBaseUI::ActivateGameUI(void)
{
	return g_pfnCBaseUI_ActivateGameUI(this, 0);
}

bool CBaseUI::IsGameUIVisible(void)
{
	return g_pfnCBaseUI_IsGameUIVisible(this, 0);
}

void CBaseUI::HideConsole(void)
{
	return g_pfnCBaseUI_HideConsole(this, 0);
}

void CBaseUI::ShowConsole(void)
{
	return g_pfnCBaseUI_ShowConsole(this, 0);
}

void BaseUI_InstallHook(void)
{
	CreateInterfaceFn EngineCreateInterface = g_pMetaHookAPI->GetEngineFactory();
	g_pBaseUI = (IBaseUI *)EngineCreateInterface(BASEUI_INTERFACE_VERSION, 0);

	CBaseUI BaseUI;
	DWORD *pVFTable = *(DWORD **)&BaseUI;

	g_pMetaHookAPI->VFTHook(g_pBaseUI, 0, 1, (void *)pVFTable[1], (void *&)g_pfnCBaseUI_Initialize);
	//g_pMetaHookAPI->VFTHook(g_pBaseUI, 0, 2, (void *)pVFTable[2], (void *&)g_pfnCBaseUI_Start);
	//g_pMetaHookAPI->VFTHook(g_pBaseUI, 0, 3, (void *)pVFTable[3], (void *&)g_pfnCBaseUI_Shutdown);
	//g_pMetaHookAPI->VFTHook(g_pBaseUI, 0, 4, (void *)pVFTable[4], (void *&)g_pfnCBaseUI_Key_Event);
	//g_pMetaHookAPI->VFTHook(g_pBaseUI, 0, 5, (void *)pVFTable[5], (void *&)g_pfnCBaseUI_CallEngineSurfaceProc);
	//g_pMetaHookAPI->VFTHook(g_pBaseUI, 0, 6, (void *)pVFTable[6], (void *&)g_pfnCBaseUI_Paint);
	//g_pMetaHookAPI->VFTHook(g_pBaseUI, 0, 7, (void *)pVFTable[7], (void *&)g_pfnCBaseUI_HideGameUI);
	//g_pMetaHookAPI->VFTHook(g_pBaseUI, 0, 8, (void *)pVFTable[8], (void *&)g_pfnCBaseUI_ActivateGameUI);
	//g_pMetaHookAPI->VFTHook(g_pBaseUI, 0, 9, (void *)pVFTable[9], (void *&)g_pfnCBaseUI_IsGameUIVisible);
	//g_pMetaHookAPI->VFTHook(g_pBaseUI, 0, 10, (void *)pVFTable[10], (void *&)g_pfnCBaseUI_HideConsole);
	//g_pMetaHookAPI->VFTHook(g_pBaseUI, 0, 11, (void *)pVFTable[11], (void *&)g_pfnCBaseUI_ShowConsole);
}