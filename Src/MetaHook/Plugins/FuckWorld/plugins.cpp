#include <metahook.h>
#include "exportfuncs.h"
#include "engine_addr.h"

cl_exportfuncs_t gExportfuncs;
mh_interface_t *g_pInterface;
metahook_api_t *g_pMetaHookAPI;
mh_enginesave_t *g_pMetaSave;

IFileSystem *g_pFileSystem;

DWORD g_dwEngineBase, g_dwEngineSize;

// Hooks
void WindowMode_InstallHook(void);
void BaseUI_InstallHook(void);
void StartupGraphic_Init(void);
void Command_Init(void);
void Keys_InstallHook(void);
void StudioModelSkin_InstallHook(void);
void GLTexture_Init(void);
void Sky_InstallHook(void);
void BspTexture_Init(void);
void Studio_InstallHook(void);
void WAD_InstallHook(void);
void Registry_InstallHook(void);

// Patchs
void SkyPatch_Attach(void);

// Hook OutputDebugString
void Log_Init(void);

void MHP_LogFile(const char *format, ...)
{
#if 1
	va_list arg;
	static char string[1024];

	va_start(arg, format);
	vsnprintf(string, sizeof(string) - 1, format, arg);
	va_end(arg);

	OutputDebugStringA(string);
#endif
}

void IPlugins::Init(metahook_api_t *pAPI, mh_interface_t *pInterface, mh_enginesave_t *pSave)
{
	g_pInterface = pInterface;
	g_pMetaHookAPI = pAPI;
	g_pMetaSave = pSave;

	g_pInterface->CommandLine->AppendParm("-forcevalve", NULL);
	g_pInterface->CommandLine->AppendParm("-nomaster", NULL);

	Log_Init();

	MHP_LogFile("MetaHook plugins initailized !\n");
}

void IPlugins::Shutdown(void)
{
}

#include "sys.h"

void IPlugins::LoadEngine(void)
{
	if (g_pMetaHookAPI->GetEngineBuildnum() != 3266)
	{
		Sys_Error("MT.dll cannot run in this version of engine");
	}

	g_pFileSystem = g_pInterface->FileSystem;

	g_dwEngineBase = g_pMetaHookAPI->GetEngineBase();
	g_dwEngineSize = g_pMetaHookAPI->GetEngineSize();

	Engine_Init();

	//SkyPatch_Attach();

	//Fuck the texnum for vgui
	g_pMetaHookAPI->WriteDWORD( (void *)0x01EDEB74, 10000 );

	// SV_ClipToLinks @pfnShouldCollide
	g_pMetaHookAPI->WriteDWORD( (void *)( 0x01DCE986 + 0x2 ), 0x000001E7 );

	WindowMode_InstallHook();
	BaseUI_InstallHook();
	StartupGraphic_Init();
	Command_Init();
	Keys_InstallHook();
	StudioModelSkin_InstallHook();
	GLTexture_Init();
	Sky_InstallHook();
	BspTexture_Init();
	Studio_InstallHook();
	WAD_InstallHook();
	Registry_InstallHook();

}

void IPlugins::LoadClient(cl_exportfuncs_t *pExportFunc)
{
	memcpy(&gExportfuncs, pExportFunc, sizeof(gExportfuncs));

	pExportFunc->Initialize = Initialize;
	pExportFunc->HUD_Init = HUD_Init;
	pExportFunc->HUD_Redraw = HUD_Redraw;
	//pExportFunc->V_CalcRefdef = V_CalcRefdef;
	pExportFunc->HUD_DrawTransparentTriangles = HUD_DrawTransparentTriangles;
	pExportFunc->HUD_Shutdown = HUD_Shutdown;
	//pExportFunc->HUD_Frame = HUD_Frame;
	//pExportFunc->HUD_GetStudioModelInterface = HUD_GetStudioModelInterface;
}

void IPlugins::ExitGame(int iResult)
{
}

EXPOSE_SINGLE_INTERFACE(IPlugins, IPlugins, METAHOOK_PLUGIN_API_VERSION);