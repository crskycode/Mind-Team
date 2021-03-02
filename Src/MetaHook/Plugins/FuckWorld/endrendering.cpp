#include <metahook.h>
#include "plugins.h"


void (*g_pfnSCR_NetGraph)(void) = NULL;

#define SCR_NETGRAPH_SIG "\x83\xEC\x64\x53\x55\x56\x57\x33\xFF\x89\x7C\x24\x3C\x89\x7C\x24\x28\x89\x7C\x24\x24\x89\x7C\x24\x40\x89\x7C\x24\x44\xE8\x2A\x2A\x2A\x2A"


typedef void (*ENDRENDERINGPROC)(void);

ENDRENDERINGPROC g_pfnGL_EndRenderingCallBack = NULL;


void SCR_NetGraph(void)
{
	if (g_pfnGL_EndRenderingCallBack)
	{
		g_pfnGL_EndRenderingCallBack();
	}

	g_pfnSCR_NetGraph();
}

void EndRendering_InstallHook(void)
{
	*(void **)&g_pfnSCR_NetGraph = SearchPattern(g_dwEngineBase, g_dwEngineSize, SCR_NETGRAPH_SIG, sizeof(SCR_NETGRAPH_SIG) - 1);

	if (g_pfnSCR_NetGraph)
		InlineHook(g_pfnSCR_NetGraph, SCR_NetGraph, (void *&)g_pfnSCR_NetGraph);
	else
		SigNotFound(SCR_NETGRAPH_SIG);
}

void EndRendering_SetCallBack(ENDRENDERINGPROC pfn)
{
	g_pfnGL_EndRenderingCallBack = pfn;
}