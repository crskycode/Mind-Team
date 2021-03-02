#include "CF_Button.h"
#include "IMTEngine_Internal.h"
#include <KeyValues.h>

using namespace vgui;

// Force build it !
DECLARE_BUILD_FACTORY(CFButton);

CFButton::CFButton(Panel *parent, const char *panelName) : Button(parent, panelName, L"")
{
	_upImage = NULL;
	_focusImage = NULL;
	_downImage = NULL;
	_disableImage = NULL;
}

void CFButton::SetUpImage(const char *fileName)
{
	_upImage = gTexAPI.Load(fileName);
}

void CFButton::SetFocusImage(const char *fileName)
{
	_focusImage = gTexAPI.Load(fileName);
}

void CFButton::SetDownImage(const char *fileName)
{
	_downImage = gTexAPI.Load(fileName);
}

void CFButton::SetDisableImage(const char *fileName)
{
	_disableImage = gTexAPI.Load(fileName);
}

//void CFButton::SetCommand(const char *command, ...)
//{
//	va_list arg;
//	static char string[256];
//
//	va_start(arg, command);
//	vsnprintf(string, sizeof(string), command, arg);
//	va_end(arg);
//
//	BaseClass::SetCommand(string);
//}

void CFButton::OnMouseDoublePressed(MouseCode code)
{
	// Get the action message from button setting
	KeyValues *msg = GetCommand();

	if ( msg )
	{
		// Make a copy for this
		KeyValues *sig = msg->MakeCopy();

		// Mark the action is double pressed
		sig->SetInt("IsDouble", 1);

		// Fire button action signal
		PostActionSignal(sig);
	}

	// Call click function
	BaseClass::OnMousePressed(code);
}

void CFButton::PaintBackground(void)
{
	// Don't draw background
}

void CFButton::Paint(void)
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
			if (_upImage && _upImage->id)
			{
				gTexAPI.DrawSetTexture(_upImage);
				gTexAPI.DrawTexturedRect(0, 0, iWidth, iHeight);
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

void CFButton::PaintBorder(void)
{
	// Don't draw border
}

void CFButton::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

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

void CFButton::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	// Unset button border
	SetDefaultBorder(NULL);
	SetDepressedBorder(NULL);
	SetKeyFocusBorder(NULL);
}