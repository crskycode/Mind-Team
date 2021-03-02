/***
 * 
 * Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 * 
 * This product contains software technology licensed from Id
 * Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 * All Rights Reserved.
 * 
 * Use, distribution, and modification of this source code and/or resulting
 * object code is restricted to non-commercial enhancements to products from
 * Valve LLC. All other use, distribution, or modification is prohibited
 * without written permission from Valve LLC.
 * 
***/

#include "EngineInterface.h"
#include "IMTEngine_Internal.h"
#include "GameUI_Interface.h"
#include <IVGuiDLL.h>
#include <IEngineVGui.h>
#include <vgui/KeyCode.h>
#include <IGameUIFuncs.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/Panel.h>

#include "BasePanel.h"
#include "CDKeyEntryDialog.h"
#include "OptionsDialog.h"
#include "CreateMultiplayerGameDialog.h"

#include "UserSystem.h"
#include "ShopItemManager.h"
#include "UI_Globals.h"

cl_enginefunc_t gEngfuncs;

IGameUIFuncs *gameuifuncs = NULL;
vgui::IEngineVGui *enginevguifuncs = NULL;
vgui::ISurface *enginesurfacefuncs = NULL;
cl_enginefunc_t *engine = NULL;

static CBasePanel *staticPanel = NULL;

IMTEngine *gpMTEngine;
font_api_t gFontAPI;
texture_api_t gTexAPI;

CCDKeyEntryDialog *g_pCDKeyEntryDialog = NULL;

vgui::DHANDLE<COptionsDialog> g_OptionsDialog;
vgui::DHANDLE<CCreateMultiplayerGameDialog> g_CreateMultiplayerGameDialog;

void DisplayOptionsDialog(void)
{
	if (!g_OptionsDialog.Get())
	{
		g_OptionsDialog = new COptionsDialog(staticPanel);
	}

	g_OptionsDialog->SetPos( 40, 40 );
	g_OptionsDialog->Activate();
}

void DisplayCreateMultiplayerGameDialog(void)
{
	if (!g_CreateMultiplayerGameDialog.Get())
	{
		g_CreateMultiplayerGameDialog = new CCreateMultiplayerGameDialog(staticPanel);
	}

	g_CreateMultiplayerGameDialog->SetPos( 40, 40 );
	g_CreateMultiplayerGameDialog->Activate();
}

CGameUI::CGameUI()
{
	m_bActivatedUI = false;
}

CGameUI::~CGameUI()
{
}

void CGameUI::Initialize(CreateInterfaceFn *factories, int count)
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
	// load the MT engine library
	fs->GetLocalPath("metahook\\plugins\\mt.dll", szDllName, sizeof(szDllName));
	HINTERFACEMODULE hMTEngineModule = Sys_LoadModule(szDllName);

	CreateInterfaceFn vguiDllFactory = Sys_GetFactory(hVGuiDllModule);
	CreateInterfaceFn mtEngineFactory = Sys_GetFactory(hMTEngineModule);

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

	// setup vgui controls
	vgui::VGui_InitInterfacesList("GameUI", factories, count);

	// setup MT engine interface
	gpMTEngine = (IMTEngine *)mtEngineFactory(MTENGINE_VERSION, NULL);
	gpMTEngine->GetTexAPI(&gTexAPI);
	gpMTEngine->GetFontAPI(&gFontAPI);

	// load localization file
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "Resource/vgui_%language%.txt");
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "Resource/gameui_%language%.txt");

	enginevguifuncs = (vgui::IEngineVGui *)engineFactory(VENGINE_VGUI_VERSION, NULL);
	enginesurfacefuncs = (vgui::ISurface *)vguiDllFactory(VGUI_SURFACE_INTERFACE_VERSION, NULL);
	gameuifuncs = (IGameUIFuncs *)engineFactory(VENGINE_GAMEUIFUNCS_VERSION, NULL);

	if (!enginesurfacefuncs || !gameuifuncs || !enginevguifuncs)
	{
		Sys_Error("CGameUI::Initialize() failed to get necessary interfaces\n");
	}

	int swide, stall;
	g_pVGuiSurface->GetScreenSize(swide, stall);

	// setup base panel
	staticPanel = new CBasePanel();
	staticPanel->SetBounds(0, 0, swide, stall);
	staticPanel->SetPaintBorderEnabled(false);
	staticPanel->SetPaintBackgroundEnabled(true);
	staticPanel->SetPaintEnabled(false);
	staticPanel->SetVisible(true);
	staticPanel->SetMouseInputEnabled(false);
	staticPanel->SetKeyBoardInputEnabled(false);

	vgui::VPANEL rootpanel = enginevguifuncs->GetPanel(vgui::PANEL_GAMEUIDLL);
	staticPanel->SetParent(rootpanel);
}

void CGameUI::Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, void *system)
{
	memcpy(&gEngfuncs, engineFuncs, sizeof(gEngfuncs));
	engine = &gEngfuncs;

	vgui::scheme()->LoadSchemeFromFile( "Resource/SourceScheme.res", "SourceScheme" );

	char cdkey[256];

	//vgui::system()->GetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Half-Life\\Settings\\ValveKey", cdkey, sizeof(cdkey) - 1);
	//
	//if (!strlen(cdkey))
	//{
	//	vgui::system()->GetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Half-Life\\Settings\\yeK1", cdkey, sizeof(cdkey) - 1);
	//
	//	if (!strlen(cdkey))
	//	{
	//		vgui::system()->GetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Half-Life\\Settings\\yeK2", cdkey, sizeof(cdkey) - 1);
	//
	//		if (!strlen(cdkey))
	//		{
	//			if (!g_pCDKeyEntryDialog)
	//			{
	//				g_pCDKeyEntryDialog = new CCDKeyEntryDialog(staticPanel, false);
	//			}
	//
	//			g_pCDKeyEntryDialog->Activate();
	//		}
	//	}
	//}

	g_ShopItemManager.Initialize();
	g_User.Initialize();

	gUI.Initialize( staticPanel );
	gUI.Start();
}

void CGameUI::Shutdown(void)
{
	gUI.Shutdown();
	g_User.Shutdown();
	g_ShopItemManager.Shutdown();
}

int CGameUI::ActivateGameUI(void)
{
	if ( IsGameUIActive() )
		return 1;

	m_bActivatedUI = true;

	staticPanel->SetVisible( true );
	return 1;
}

int CGameUI::ActivateDemoUI(void)
{
	return 1;
}

int CGameUI::HasExclusiveInput(void)
{
	return IsGameUIActive();
}

void CGameUI::RunFrame(void)
{
	staticPanel->RunFrame();
}

void CGameUI::ConnectToServer(const char *game, int IP, int port)
{
}

void CGameUI::DisconnectFromServer(void)
{
}

void CGameUI::HideGameUI(void)
{
	const char *level = gEngfuncs.pfnGetLevelName();

	if (level && *level)
	{
		staticPanel->SetVisible( false );
	}
}

bool CGameUI::IsGameUIActive(void)
{
	if ( m_bActivatedUI )
	{
		return staticPanel->IsVisible();
	}

	return false;
}

void CGameUI::LoadingStarted(const char *resourceType, const char *resourceName)
{
	//gEngfuncs.Con_Printf("LoadingStarted: Type:%s Name:%s\n", resourceType, resourceName);

	if (!Q_strcmp(resourceType, "level"))
	{
		if (gUI.GetLoading())
		{
			gUI.GetLoading()->SetMapBg( "tex/UI/Loading/LOADINGTD_CQB_GR.dtx" );
		}
	}
}

void CGameUI::LoadingFinished(const char *resourceType, const char *resourceName)
{
	//gEngfuncs.Con_Printf("LoadingFinished: Type:%s Name:%s\n", resourceType, resourceName);
}

static bool g_skipfirstset = false;
void CGameUI::StartProgressBar(const char *progressType, int progressSteps)
{
	//gEngfuncs.Con_Printf("StartProgressBar: Type:%s Step:%d\n", progressType, progressSteps);

	if (!Q_strcmp(progressType, "Connecting"))
	{
		if (gUI.GetRoom())
		{
			gUI.GetRoom()->Close();
		}

		if (gUI.GetMsgWnd())
		{
			gUI.GetMsgWnd()->Close();
		}

		if (gUI.GetLoading())
		{
			gUI.GetLoading()->SetProgress( 1 );
			gUI.GetLoading()->Activate();
		}

		g_skipfirstset = true;
	}
}

int CGameUI::ContinueProgressBar(int progressPoint, float progressFraction)
{
	//gEngfuncs.Con_Printf("ContinueProgressBar: Point:%d Fraction:%f\n", progressPoint, progressFraction);

	if (g_skipfirstset && progressPoint == 11)
	{
		g_skipfirstset = false;
		return 1;
	}

	if (gUI.GetLoading())
	{
		gUI.GetLoading()->SetProgress( progressPoint / 11.0 * 100 );
	}

	return 1;
}

void CGameUI::StopProgressBar(bool bError, const char *failureReason, const char *extendedReason)
{
	//gEngfuncs.Con_Printf("StopProgressBar: Error:%d failureReason:%s extendedReason:%s\n", bError, failureReason, extendedReason);

	if (gUI.GetLoading())
	{
		gUI.GetLoading()->Close();
	}
}

int CGameUI::SetProgressBarStatusText(const char *statusText)
{
	return 1;
}

void CGameUI::SetSecondaryProgressBar(float progress)
{
}

void CGameUI::SetSecondaryProgressBarText(const char *statusText)
{
}

void CGameUI::OnDisconnectFromServer(int eSteamLoginFailure, const char *username)
{
}

EXPOSE_SINGLE_INTERFACE(CGameUI, CGameUI, GAMEUI_INTERFACE_VERSION);