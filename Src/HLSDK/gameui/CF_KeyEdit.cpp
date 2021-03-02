#include "CF_KeyEdit.h"
#include "IMTEngine_Internal.h"
#include <KeyValues.h>

using namespace vgui;

DECLARE_BUILD_FACTORY(CFKeyEdit);

static char *GetKeyName(KeyCode code)
{
	switch (code)
	{
		case KEY_0: return "0";
		case KEY_1: return "1";
		case KEY_2: return "2";
		case KEY_3: return "3";
		case KEY_4: return "4";
		case KEY_5: return "5";
		case KEY_6: return "6";
		case KEY_7: return "7";
		case KEY_8: return "8";
		case KEY_9: return "9";
		case KEY_A: return "A";
		case KEY_B: return "B";
		case KEY_C: return "C";
		case KEY_D: return "D";
		case KEY_E: return "E";
		case KEY_F: return "F";
		case KEY_G: return "G";
		case KEY_H: return "H";
		case KEY_I: return "I";
		case KEY_J: return "J";
		case KEY_K: return "K";
		case KEY_L: return "L";
		case KEY_M: return "M";
		case KEY_N: return "N";
		case KEY_O: return "O";
		case KEY_P: return "P";
		case KEY_Q: return "Q";
		case KEY_R: return "R";
		case KEY_S: return "S";
		case KEY_T: return "T";
		case KEY_U: return "U";
		case KEY_V: return "V";
		case KEY_W: return "W";
		case KEY_X: return "X";
		case KEY_Y: return "Y";
		case KEY_Z: return "Z";
		case KEY_PAD_0: return "PAD_0";
		case KEY_PAD_1: return "PAD_1";
		case KEY_PAD_2: return "PAD_2";
		case KEY_PAD_3: return "PAD_3";
		case KEY_PAD_4: return "PAD_4";
		case KEY_PAD_5: return "PAD_5";
		case KEY_PAD_6: return "PAD_6";
		case KEY_PAD_7: return "PAD_7";
		case KEY_PAD_8: return "PAD_8";
		case KEY_PAD_9: return "PAD_9";
		case KEY_PAD_DIVIDE: return "PAD_DIVIDE";
		case KEY_PAD_MULTIPLY: return "PAD_MULTIPLY";
		case KEY_PAD_MINUS: return "PAD_MINUS";
		case KEY_PAD_PLUS: return "PAD_PLUS";
		case KEY_PAD_ENTER: return "PAD_ENTER";
		case KEY_PAD_DECIMAL: return "PAD_DECIMAL";
		case KEY_LBRACKET: return "LBRACKET";
		case KEY_RBRACKET: return "RBRACKET";
		case KEY_SEMICOLON: return "SEMICOLON";
		case KEY_APOSTROPHE: return "APOSTROPHE";
		case KEY_BACKQUOTE: return "BACKQUOTE";
		case KEY_COMMA: return "COMMA";
		case KEY_PERIOD: return "PERIOD";
		case KEY_SLASH: return "SLASH";
		case KEY_BACKSLASH: return "BACKSLASH";
		case KEY_MINUS: return "MINUS";
		case KEY_EQUAL: return "EQUAL";
		case KEY_ENTER: return "ENTER";
		case KEY_SPACE: return "SPACE";
		case KEY_BACKSPACE: return "BACKSPACE";
		case KEY_TAB: return "TAB";
		case KEY_CAPSLOCK: return "CAPSLOCK";
		case KEY_NUMLOCK: return "NUMLOCK";
		case KEY_ESCAPE: return "ESCAPE";
		case KEY_SCROLLLOCK: return "SCROLLLOCK";
		case KEY_INSERT: return "INSERT";
		case KEY_DELETE: return "DELETE";
		case KEY_HOME: return "HOME";
		case KEY_END: return "END";
		case KEY_PAGEUP: return "PAGEUP";
		case KEY_PAGEDOWN: return "PAGEDOWN";
		case KEY_BREAK: return "BREAK";
		case KEY_LSHIFT: return "LSHIFT";
		case KEY_RSHIFT: return "RSHIFT";
		case KEY_LALT: return "LALT";
		case KEY_RALT: return "RALT";
		case KEY_LCONTROL: return "LCONTROL";
		case KEY_RCONTROL: return "RCONTROL";
		case KEY_LWIN: return "LWIN";
		case KEY_RWIN: return "RWIN";
		case KEY_APP: return "APP";
		case KEY_UP: return "UP";
		case KEY_LEFT: return "LEFT";
		case KEY_DOWN: return "DOWN";
		case KEY_RIGHT: return "RIGHT";
		case KEY_F1: return "F1";
		case KEY_F2: return "F2";
		case KEY_F3: return "F3";
		case KEY_F4: return "F4";
		case KEY_F5: return "F5";
		case KEY_F6: return "F6";
		case KEY_F7: return "F7";
		case KEY_F8: return "F8";
		case KEY_F9: return "F9";
		case KEY_F10: return "F10";
		case KEY_F11: return "F11";
		case KEY_F12: return "F12";
		case KEY_CAPSLOCKTOGGLE: return "CAPSLOCKTOGGLE";
		case KEY_NUMLOCKTOGGLE: return "NUMLOCKTOGGLE";
		case KEY_SCROLLLOCKTOGGLE: return "SCROLLLOCKTOGGLE";
	}
	
	return "<UNKNOWN KEYNUM>";
}

static char *GetMouseKeyName(MouseCode code)
{
	switch (code)
	{
		case MOUSE_LEFT: return "MOUSELEFT";
		case MOUSE_RIGHT: return "MOUSERIGHT";
		case MOUSE_MIDDLE: return "MOUSEMIDDLE";
		case MOUSE_4: return "MOUSE4";
		case MOUSE_5: return "MOUSE5";
	}
	
	return "<UNKNOWN KEYNUM>";
}

static char *GetWheelKeyName(int code)
{
	if (code < 0)
		return "WHEELDOWN";
	if (code > 0)
		return "WHEELUP";

	return "<UNKNOWN KEYNUM>";
}

CFKeyEdit::CFKeyEdit(Panel *parent, const char *panelName) : Panel(parent, panelName)
{
	_actively = 0;
	_capturing = false;
	_captureKeyboard = true;
	_captureMouse = true;
	_keyName[0] = '\0';
	_upImage = NULL;
	_focusImage = NULL;
	_downImage = NULL;

	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);
	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);
}

void CFKeyEdit::SetVisible(bool state)
{
	if (!state)
	{
		if (_capturing)
		{
			EndCapture((KeyCode)-1, (MouseCode)-1, 0);
		}
	}

	BaseClass::SetVisible(state);
}

void CFKeyEdit::SetEnabled(bool state)
{
	if (!state)
	{
		if (_capturing)
		{
			EndCapture((KeyCode)-1, (MouseCode)-1, 0);
		}
	}

	BaseClass::SetEnabled(state);
}

void CFKeyEdit::SetCaptureKeyboardEnable(bool state)
{
	_captureKeyboard = state;
}

void CFKeyEdit::SetCaptureMouseEnable(bool state)
{
	_captureMouse = state;
}

void CFKeyEdit::SetKey(const char *name)
{
	strcpy(_keyName, name);
}

char *CFKeyEdit::GetKey(void)
{
	return _keyName;
}

void CFKeyEdit::SetUpImage(const char *fileName)
{
	_upImage = gTexAPI.Load(fileName);
}

void CFKeyEdit::SetFocusImage(const char *fileName)
{
	_focusImage = gTexAPI.Load(fileName);
}

void CFKeyEdit::SetDownImage(const char *fileName)
{
	_downImage = gTexAPI.Load(fileName);
}

void CFKeyEdit::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	SetCaptureKeyboardEnable(inResourceData->GetInt("allowKeyboard"));
	SetCaptureMouseEnable(inResourceData->GetInt("allowMouse"));
	SetUpImage(inResourceData->GetString("upImage"));
	SetFocusImage(inResourceData->GetString("focusImage"));
	SetDownImage(inResourceData->GetString("downImage"));
}

void CFKeyEdit::OnCursorEntered(void)
{
	if (!IsEnabled())
		return;

	if (_capturing)
		return;

	if (_actively == 0)
		_actively = 1;
}

void CFKeyEdit::OnCursorExited(void)
{
	if (!IsEnabled())
		return;

	if (_capturing)
		return;

	if (_actively == 1)
		_actively = 0;
}

void CFKeyEdit::OnKeyCodePressed(KeyCode code)
{
	if (!IsEnabled())
		return;

	if (_capturing)
	{
		if (_captureKeyboard)
		{
			EndCapture(code, (MouseCode)-1, 0);
		}
	}

	BaseClass::OnKeyCodePressed(code);
}

void CFKeyEdit::OnMousePressed(MouseCode code)
{
	if (!IsEnabled())
		return;

	if (_capturing)
	{
		if (_captureMouse)
		{
			EndCapture((KeyCode)-1, code, 0);
		}
	}
	else
	{
		if (code == MOUSE_LEFT)
		{
			if (_actively == 1)
			{
				_actively = 2;
				input()->SetMouseCapture(GetVPanel());
			}
		}
	}
}

void CFKeyEdit::OnMouseReleased(MouseCode code)
{
	if (!IsEnabled())
		return;

	if (_capturing)
		return;

	if (_actively == 2)
	{
		input()->SetMouseCapture(NULL);

		if (IsCursorOver())
		{
			_actively = 1;
			StartCapture();
		}
		else
		{
			_actively = 0;
		}
	}
}

void CFKeyEdit::OnMouseWheeled(int delta)
{
	if (!IsEnabled())
		return;

	if (_capturing)
	{
		if (_captureMouse)
		{
			EndCapture((KeyCode)-1, (MouseCode)-1, delta);
		}
	}

	BaseClass::OnMouseWheeled(delta);
}

void CFKeyEdit::Paint(void)
{
	int wide, tall;
	GetSize(wide, tall);

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 255);

	if (IsEnabled())
	{
		if (_actively == 1 || _capturing)
		{
			if (_focusImage && _focusImage->id)
			{
				gTexAPI.DrawSetTexture(_focusImage);
				gTexAPI.DrawTexturedRect(0, 0, wide, tall);
			}
		}
		else if (_actively == 2)
		{
			if (_downImage && _downImage->id)
			{
				gTexAPI.DrawSetTexture(_downImage);
				gTexAPI.DrawTexturedRect(0, 0, wide, tall);
			}
		}
		else
		{
			if (_upImage && _upImage->id)
			{
				gTexAPI.DrawSetTexture(_upImage);
				gTexAPI.DrawTexturedRect(0, 0, wide, tall);
			}
		}
	}
	else
	{
		if (_upImage && _upImage->id)
		{
			gTexAPI.DrawSetTexture(_upImage);
			gTexAPI.DrawTexturedRect(0, 0, wide, tall);
		}
	}
}

void CFKeyEdit::StartCapture(void)
{
	if (_capturing)
		return;

	if (!_captureKeyboard && !_captureMouse)
		return;

	RequestFocus();
	input()->SetMouseCapture(GetVPanel());
	input()->SetCursorOveride(dc_hand);

	_capturing = true;
	_keyName[0] = '\0';
}

void vgui::CFKeyEdit::EndCapture(KeyCode keyCode, MouseCode mouseCode, int wheelCode)
{
	if (!_capturing)
		return;

	RequestFocus();
	input()->SetMouseCapture(NULL);
	input()->SetCursorOveride(dc_user);

	_actively = IsCursorOver();
	_capturing = false;

	if (keyCode != -1)
		strcpy(_keyName, GetKeyName(keyCode));
	else if (mouseCode != -1)
		strcpy(_keyName, GetMouseKeyName(mouseCode));
	else if (wheelCode != 0)
		strcpy(_keyName, GetWheelKeyName(wheelCode));
	else
		return;

	KeyValues *signal = new KeyValues("KeyCaptured");
	signal->SetPtr("panel", this);
	PostActionSignal(signal);
}