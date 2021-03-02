#include <metahook.h>

#include "DrawFonts.h"
#include "DrawTextures.h"

class CDrawAPI : public IBaseInterface
{
public:
	virtual void GetAPI(font_api_t *pfontapi, texture_api_t *ptextureapi)
	{
		memcpy(pfontapi, &gFontAPI, sizeof(gFontAPI));
		memcpy(ptextureapi, &gTexAPI, sizeof(gTexAPI));
	}
};

#define METAHOOK_DRAWAPI_VERSION "METAHOOK_DRAWAPI_VERSION001"

EXPOSE_SINGLE_INTERFACE(CDrawAPI, CDrawAPI, METAHOOK_DRAWAPI_VERSION);