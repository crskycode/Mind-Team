#include "CF_Image.h"
#include "IMTEngine_Internal.h"
#include <KeyValues.h>

using namespace vgui;

// Force build it !
DECLARE_BUILD_FACTORY(CFImage);

CFImage::CFImage(Panel *parent, const char *panelName) : Panel(parent, panelName)
{
	// Igonre mouse input
	DisableMouseInputForThisPanel(true);

	_image = NULL;
	_alpha = 255;
	_clip[0] = -1;
}

void CFImage::SetImage(const char *fileName, ...)
{
	if (!fileName || !*fileName)
	{
		_image = NULL;
		return;
	}

	va_list arg;
	static char string[64];
	
	va_start(arg, fileName);
	vsnprintf(string, sizeof(string), fileName, arg);
	va_end(arg);

	// Load the image, PNG/DTX/TGA file is supported
	_image = gTexAPI.Load(string);
}

void CFImage::SetClip(int x, int y, int wide, int tall)
{
	_clip[0] = x;
	_clip[1] = y;
	_clip[2] = wide;
	_clip[3] = tall;
}

void CFImage::GetClip(int &x, int &y, int &wide, int &tall)
{
	x = _clip[0];
	y = _clip[1];
	wide = _clip[2];
	tall = _clip[3];
}

void CFImage::PaintBackground(void)
{
}

void CFImage::Paint(void)
{
	// Image is not set, so don't do anything
	if (!_image || !_image->id)
		return;

	// Get the panel size for draw
	int iWidth = GetWide();
	int iHeight = GetTall();

	// Draw the render style
	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, _alpha);

	// Setup texture
	gTexAPI.DrawSetTexture(_image);

	// Use range cliping
	if (_clip[0] != -1)
	{
		gTexAPI.TextureEnableScissor(_clip[0], _clip[1], _clip[2], _clip[3]);
	}

	// Draw image
	gTexAPI.DrawTexturedRect(0, 0, iWidth, iHeight);
}

void CFImage::PaintBorder(void)
{
	// Dont' draw border
}

void CFImage::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	// Get image file from setting file
	const char *fileName = inResourceData->GetString("image");

	if (fileName && *fileName)
	{
		// We got the image file, setup the image
		SetImage(fileName);
	}

	// Get image clip rect option
	const char *option = inResourceData->GetString("clip");

	if (option && *option)
	{
		// Parse option string
		sscanf(option, "%d %d %d %d", &_clip[0], &_clip[1], &_clip[2], &_clip[3]);
	}
}

void CFImage::ApplySchemeSettings(IScheme *pScheme)
{
	// Unset border for panel
	SetBorder(NULL);
}