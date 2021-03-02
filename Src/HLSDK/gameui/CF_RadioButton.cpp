#include "CF_RadioButton.h"
#include "IMTEngine_Internal.h"
#include <KeyValues.h>

using namespace vgui;

// Force build it !
DECLARE_BUILD_FACTORY(CFRadioButton);

CFRadioButton::CFRadioButton(Panel *parent, const char *panelName) : RadioButton(parent, panelName, "")
{
	_upImage = NULL;
	_focusImage = NULL;
	_downImage = NULL;
	_disableImage = NULL;
}

void CFRadioButton::SetRadioGroup(int group)
{
	// RadioButton::_oldTabPosition
	*(reinterpret_cast<int *>(reinterpret_cast<byte *>(this) + 348)) = group;
}

int CFRadioButton::GetRadioGroup(void)
{
	return GetRadioTabPosition();
}

void CFRadioButton::SetUpImage(const char *fileName)
{
	_upImage = gTexAPI.Load(fileName);
}

void CFRadioButton::SetFocusImage(const char *fileName)
{
	_focusImage = gTexAPI.Load(fileName);
}

void CFRadioButton::SetDownImage(const char *fileName)
{
	_downImage = gTexAPI.Load(fileName);
}

void CFRadioButton::SetDisableImage(const char *fileName)
{
	_disableImage = gTexAPI.Load(fileName);
}

void CFRadioButton::Paint(void)
{
	if (!ShouldPaint())
		return;

	// Get size for draw
	int iWidth = GetWide();
	int iHeight = GetTall();

	// Setup render
	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 255);

	// Draw button image
	if (IsEnabled())
	{
		if (IsDepressed())
		{
			// Button pressed
			if (_downImage && _downImage->id)
			{
				gTexAPI.DrawSetTexture(_downImage);
				gTexAPI.DrawTexturedRect(0, 0, iWidth, iHeight);
			}
		}
		else if (IsArmed())
		{
			// Button actived
			if (_focusImage && _focusImage->id)
			{
				gTexAPI.DrawSetTexture(_focusImage);
				gTexAPI.DrawTexturedRect(0, 0, iWidth, iHeight);
			}
		}
		else
		{
			if (IsSelected())
			{
				if (_downImage && _downImage->id)
				{
					gTexAPI.DrawSetTexture(_downImage);
					gTexAPI.DrawTexturedRect(0, 0, iWidth, iHeight);
				}
			}
			else
			{
				if (_upImage && _upImage->id)
				{
					gTexAPI.DrawSetTexture(_upImage);
					gTexAPI.DrawTexturedRect(0, 0, iWidth, iHeight);
				}
			}
		}
	}
	else
	{
		if (_disableImage && _disableImage->id)
		{
			gTexAPI.DrawSetTexture(_disableImage);
			gTexAPI.DrawTexturedRect(0, 0, iWidth, iHeight);
		}
		else if (_upImage && _upImage->id)
		{
			// If disable image is not set, we draw the up image
			gTexAPI.DrawSetTexture(_upImage);
			gTexAPI.DrawTexturedRect(0, 0, iWidth, iHeight);
		}
	}
}

void CFRadioButton::PaintBackground(void)
{
	// Don't draw background
}

void CFRadioButton::PaintBorder(void)
{
	// Don't draw border
}

void CFRadioButton::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	// Set radio group ID
	SetRadioGroup(inResourceData->GetInt("group"));

	const char *fileName;

	fileName = inResourceData->GetString("upImage");

	if (fileName && *fileName)
	{
		SetUpImage(fileName);
	}

	fileName = inResourceData->GetString("focusImage");

	if (fileName && *fileName)
	{
		SetFocusImage(fileName);
	}

	fileName = inResourceData->GetString("downImage");

	if (fileName && *fileName)
	{
		SetDownImage(fileName);
	}

	fileName = inResourceData->GetString("disableImage");

	if (fileName && *fileName)
	{
		SetDisableImage(fileName);
	}
}

void CFRadioButton::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	// Unset button border
	SetDefaultBorder(NULL);
	SetDepressedBorder(NULL);
	SetKeyFocusBorder(NULL);
}