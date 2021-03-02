#include "CF_Sprite.h"
#include "IMTEngine_Internal.h"
#include <KeyValues.h>
#include "EngineInterface.h"

using namespace vgui;

// Force build it !
DECLARE_BUILD_FACTORY(CFSprite);

CFSprite::CFSprite(Panel *parent, const char *panelName) : Panel(parent, panelName)
{
	_image.RemoveAll();
	_alpha = 255;
	_count = 0;
	_startTime = 0;
	_frame = 0;
}

void CFSprite::SetSprite(const char *fileName, int count, float updateTime)
{
	// Must be least 1 image !
	if ( !count )
		return;

	_image.RemoveAll();
	_image.EnsureCount(count);

	for (int i = 0; i < count; i++)
	{
		char path[260];
		
		// Make the full path
		_snprintf(path, sizeof(path), "%s%03d.png", fileName, i + 1);

		// Load the image
		_image[i] = gTexAPI.LoadPNG(path);
	}

	_count = count;
	_startTime = gEngfuncs.GetClientTime();
	_updateTime = updateTime;
	_frame = 0;
}

void CFSprite::PaintBackground(void)
{
	// Don't draw background
}

void CFSprite::Paint(void)
{
	// Not set sprite, don't do anything
	if ( !_count )
		return;

	// Estimate frame
	uint32 frame = (gEngfuncs.GetClientTime() - _startTime) / (_count * _updateTime) * _count;

	// Mod it
	frame = frame % _count;

	// Out of range ?
	if (frame < 0 || frame >= _count)
		return;

	// Get size for draw
	int iWidth = GetWide();
	int iHeight = GetTall();

	// Draw the image
	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, _alpha);
	gTexAPI.DrawSetTexture(_image[frame]);
	gTexAPI.DrawTexturedRect(0, 0, iWidth, iHeight);
}

void CFSprite::PaintBorder(void)
{
	// Don't draw border
}

void CFSprite::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	// Get image file from setting file
	const char *fileName = inResourceData->GetString("image");

	// We got the image file, setup the sprite
	if (fileName && *fileName)
	{
		// Sprite frame count
		int count = inResourceData->GetInt("spriteCount");

		// Sprite update interval
		float updateTime = inResourceData->GetFloat("updateTime");

		// Setup the sprite
		SetSprite(fileName, count, updateTime);
	}
}

void CFSprite::ApplySchemeSettings(IScheme *pScheme)
{
	// Unset border for panel
	SetBorder(NULL);
}