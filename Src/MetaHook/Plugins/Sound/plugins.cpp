#include <metahook.h>
#include "engfuncs.h"
#include "exportfuncs.h"

cl_exportfuncs_t gExportfuncs;
mh_interface_t *g_pInterface;
metahook_api_t *g_pMetaHookAPI;
mh_enginesave_t *g_pMetaSave;

// engine module version
DWORD g_dwEngineBuildnum;

// engine module address space
void* g_dwEngineBase;
DWORD g_dwEngineSize;

IFileSystem *g_pFileSystem = NULL;

void IPlugins::Init(metahook_api_t *pAPI, mh_interface_t *pInterface, mh_enginesave_t *pSave)
{
	g_pInterface = pInterface;
	g_pMetaHookAPI = pAPI;
	g_pMetaSave = pSave;
}

void IPlugins::Shutdown(void)
{
}

void IPlugins::LoadEngine(void)
{
	g_dwEngineBuildnum = g_pMetaHookAPI->GetEngineBuildnum();

	g_dwEngineBase = (void*)g_pMetaHookAPI->GetEngineBase();
	g_dwEngineSize = g_pMetaHookAPI->GetEngineSize();

	g_pFileSystem = g_pInterface->FileSystem;

	// get all engine functions
	EngFuncs_Init();
}

void IPlugins::LoadClient(cl_exportfuncs_t *pExportFunc)
{
	memcpy(&gExportfuncs, pExportFunc, sizeof(gExportfuncs));

	pExportFunc->Initialize = Initialize;
}

void IPlugins::ExitGame(int iResult)
{
}

EXPOSE_SINGLE_INTERFACE(IPlugins, IPlugins, METAHOOK_PLUGIN_API_VERSION);