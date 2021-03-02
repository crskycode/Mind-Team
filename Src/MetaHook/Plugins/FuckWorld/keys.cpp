#include <metahook.h>
#include "plugins.h"
#include <keydefs.h>

void (*g_pfnKey_Event)(int key, int down) = NULL;
void (*g_pfnKey_SetBinding)(int keynum, char *binding) = NULL;

#define KEY_EVENT_SIG "\x81\xEC\x2A\x2A\x2A\x2A\x8D\x84\x24\x08\x04\x00\x00\x8D\x8C\x24\x04\x04\x00\x00\x56\x57\x50\x51\xFF\x15\x2A\x2A\x2A\x2A\x8B\x84\x24\x14\x04\x00\x00"
#define KEY_SETBINDING_SIG "\x53\x8B\x5C\x24\x08\x83\xFB\xFF\x55\x74\x2A\x8B\x04\x9D\x2A\x2A\x2A\x2A\x8B\x6C\x24\x10\x85\xC0\x74\x2A\x55\x50\xE8\x2A\x2A\x2A\x2A"

void Key_Event(int key, int down)
{
	if (down && key == K_F12)
	{
		gEngfuncs.pfnClientCmd("toggleconsole\n");
		return;
	}

	if (key == K_ESCAPE || key == '`' || key == '~')
	{
		if (!gExportfuncs.HUD_Key_Event(down, key, ""))
			return;
	}

	g_pfnKey_Event(key, down);
}

void Key_SetBinding(int keynum, char *binding)
{
	if (keynum == '`' || keynum == '~')
	{
		// If it starts with '+', that the command is from client.dll
		if (*binding != '+')
			return;
	}

	g_pfnKey_SetBinding(keynum, binding);
}

void Keys_InstallHook(void)
{
	*(void **)&g_pfnKey_Event = SearchPattern(g_dwEngineBase, g_dwEngineSize, KEY_EVENT_SIG, sizeof(KEY_EVENT_SIG) - 1);
	*(void **)&g_pfnKey_SetBinding = SearchPattern(g_dwEngineBase, g_dwEngineSize, KEY_SETBINDING_SIG, sizeof(KEY_SETBINDING_SIG) - 1);

	if (g_pfnKey_Event)
		InlineHook(g_pfnKey_Event, Key_Event, (void *&)g_pfnKey_Event);
	else
		SigNotFound(KEY_EVENT_SIG);

	if (g_pfnKey_SetBinding)
		InlineHook(g_pfnKey_SetBinding, Key_SetBinding, (void *&)g_pfnKey_SetBinding);
	else
		SigNotFound(KEY_SETBINDING_SIG);

	// Change ESC to F12 for toggleconsole
	g_pMetaHookAPI->WriteBYTE( (void *)( 0x01D6182E + 0x2 ), K_F12 );
}