#include <filesystem.h>
#include <IKeyValuesSystem.h>
#include "vgui_internal.h"
#include "IVGuiDLL.h"

vgui::IPanel *g_pIPanel = NULL;
vgui::ISurfaceV26 *g_pSurfaceV26 = NULL;
vgui::ISurface *g_pSurface = NULL;
vgui::ISystem *g_pSystem = NULL;

static bool initialized = false;

class CVGuiDLL : public IVGuiDLL
{
public:
	virtual void Init(CreateInterfaceFn *factoryList, int numFactories)
	{
		if (initialized)
			return;

		initialized = true;

		g_pFullFileSystem = (IFileSystem *)factoryList[2](FILESYSTEM_INTERFACE_VERSION, NULL);
		g_pKeyValuesSystem = (IKeyValuesSystem *)factoryList[1](KEYVALUESSYSTEM_INTERFACE_VERSION, NULL);

		g_pIPanel = (vgui::IPanel *)factoryList[1](VGUI_PANEL_INTERFACE_VERSION, NULL);
		g_pSurfaceV26 = (vgui::ISurfaceV26 *)factoryList[0](VGUI_SURFACEV26_INTERFACE_VERSION, NULL);
		g_pSystem = (vgui::ISystem *)factoryList[1](VGUI_SYSTEM_INTERFACE_VERSION, NULL);
	}
};

EXPOSE_SINGLE_INTERFACE(CVGuiDLL, IVGuiDLL, VGUI_IVGUIDLL_INTERFACE_VERSION);