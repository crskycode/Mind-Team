#define NOVECTOR
#include "cl_dll.h"
#include <filesystem.h>
#include <vgui_controls/Controls.h>
#include <IVGuiDLL.h>
#include <IClientVGui.h>
#include <IEngineVGui.h>
#include "Viewport.h"
#include "input.h"

// include dialogs
#include "vgui_gamemenudialog.h"
#include "vgui_buysetupdialog.h"
#include "vgui_chatboxdialog.h"

vgui::CViewPort *g_pViewport = NULL;


class CClientVGUI : public IClientVGUI
{
public:
	/*virtual */void Initialize(CreateInterfaceFn *factories, int count);
	/*virtual */void Start(void);
	/*virtual */void SetParent(vgui::VPANEL parent);
	/*virtual */bool UseVGUI1(void);
	/*virtual */void HideScoreBoard(void);
	/*virtual */void HideAllVGUIMenu(void);
	/*virtual */void ActivateClientUI(void);
	/*virtual */void HideClientUI(void);
};

EXPOSE_SINGLE_INTERFACE(CClientVGUI, IClientVGUI, CLIENTVGUI_INTERFACE_VERSION);

void CClientVGUI::Initialize(CreateInterfaceFn *factories, int count)
{
	char szDllName[ 512 ];

	// load and initialize vgui
	CreateInterfaceFn thisFactory = Sys_GetFactoryThis();
	CreateInterfaceFn engineFactory = factories[ 0 ];
	CreateInterfaceFn vguiFactory = factories[ 1 ];
	CreateInterfaceFn fileSystemFactory = factories[ 2 ];

	IFileSystem *fs = (IFileSystem *)fileSystemFactory(FILESYSTEM_INTERFACE_VERSION, NULL);

	// load the VGUI library
	fs->GetLocalPath("cl_dlls\\vgui_dll.dll", szDllName, sizeof(szDllName));
	HINTERFACEMODULE hVGuiDllModule = Sys_LoadModule(szDllName);

	CreateInterfaceFn vguiDllFactory = Sys_GetFactory(hVGuiDllModule);

	// setup the factory list
	CreateInterfaceFn factoryList[ 5 ] = 
	{
		thisFactory,
		engineFactory,
		vguiFactory,
		fileSystemFactory,
		vguiDllFactory,
	};

	// setup VGUI library
	IVGuiDLL *staticVGuiDllFuncs = (IVGuiDLL *)vguiDllFactory(VGUI_IVGUIDLL_INTERFACE_VERSION, NULL);
	staticVGuiDllFuncs->Init(factories, count);

	ConnectTier3Libraries(factoryList, 5);

	vgui::VGui_InitInterfacesList("ClientUI", factories, count);
}

void CClientVGUI::Start(void)
{
	vgui::scheme()->LoadSchemeFromFile("Resource/SourceScheme.res", "SourceScheme");

	int swide, stall;
	g_pVGuiSurface->GetScreenSize(swide, stall);

	g_pViewport = new vgui::CViewPort();
	g_pViewport->SetBounds(0, 0, swide, stall);
	g_pViewport->SetPaintBorderEnabled(false);
	g_pViewport->SetPaintBackgroundEnabled(true);
	g_pViewport->SetPaintEnabled(false);
	g_pViewport->SetVisible(false);
	g_pViewport->SetMouseInputEnabled(false);
	g_pViewport->SetKeyBoardInputEnabled(false);

	// initailize dialogs
	GameMenuDialog_Init();
	BuySetupDialog_Init();
	ChatBoxDialog_Init();
}

void CClientVGUI::SetParent(vgui::VPANEL parent)
{
	if (g_pViewport)
	{
		g_pViewport->SetParent(parent);
	}
}

bool CClientVGUI::UseVGUI1(void)
{
	return false;
}

void CClientVGUI::HideScoreBoard(void)
{
}

void CClientVGUI::HideAllVGUIMenu(void)
{
}

void CClientVGUI::ActivateClientUI(void)
{
	if (g_pViewport)
	{
		g_pViewport->SetVisible(true);
	}

	IN_ActivateMouse();
}

void CClientVGUI::HideClientUI(void)
{
	if (g_pViewport)
	{
		g_pViewport->SetVisible(false);
	}

	IN_DeactivateMouse();
}