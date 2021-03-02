#include "EngineInterface.h"
#include "CF_ScrollBar.h"
#include "IMTEngine_Internal.h"
#include <KeyValues.h>

using namespace vgui;

DECLARE_BUILD_FACTORY(CFScrollBar);


//-----------------------------------------------------------------------------
// Purpose: Scroll bar button-the arrow button that moves the slider up and down.
//-----------------------------------------------------------------------------
class CFScrollBarButton : public Button
{
	DECLARE_CLASS_SIMPLE(CFScrollBarButton, Button);

public:
	CFScrollBarButton(Panel *parent, const char *panelName, Panel *pActionSignalTarget, const char *pCmd) : Button(parent, panelName, L"", pActionSignalTarget, pCmd)
	{
		_upImage = NULL;
		_focusImage = NULL;
		_downImage = NULL;

		SetPaintEnabled(false);
		SetPaintBorderEnabled(false);
	}

	void SetUpImage(const char *fileName)
	{
		_upImage = gTexAPI.Load(fileName);
	}

	void SetFocusImage(const char *fileName)
	{
		_focusImage = gTexAPI.Load(fileName);
	}

	void SetDownImage(const char *fileName)
	{
		_downImage = gTexAPI.Load(fileName);
	}

protected:
	void PaintBackground(void)
	{
		gTexAPI.RenderMode(RenderNormal);
		gTexAPI.DrawSetColor(255, 255, 255, 255);

		int wide, tall;
		GetSize(wide, tall);

		if (IsEnabled())
		{
			if (IsDepressed())
			{
				if (_downImage && _downImage->id)
				{
					gTexAPI.DrawSetTexture(_downImage);
					gTexAPI.DrawTexturedRect(0, 0, wide, tall);
				}
			}
			else if (IsArmed())
			{
				if (_focusImage && _focusImage->id)
				{
					gTexAPI.DrawSetTexture(_focusImage);
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
			if (_upImage && _upImage)
			{
				gTexAPI.DrawSetTexture(_upImage);
				gTexAPI.DrawTexturedRect(0, 0, wide, tall);
			}
		}
	}

private:
	tex_t	*_upImage;
	tex_t	*_focusImage;
	tex_t	*_downImage;
};

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CFScrollBar::CFScrollBar(Panel *parent, const char *panelName) : Panel(parent, panelName)
{
	_vertical = true;
	_barPos[0] = 0;
	_barPos[1] = 0;
	_barSize[0] = 0;
	_barSize[1] = 0;
	_dragging = false;
	_range[0] = 0;
	_range[1] = 1;
	_value = 0;
	_decButton = NULL;
	_incButton = NULL;
	_barImage = NULL;

	_decButton = new CFScrollBarButton(parent, "dec_button", this, "_scrollbar_dec");
	_incButton = new CFScrollBarButton(parent, "inc_button", this, "_scrollbar_inc");

	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::SetEnabled(bool state)
{
	BaseClass::SetEnabled(state);

	_decButton->SetEnabled(state);
	_incButton->SetEnabled(state);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::SetVisible(bool state)
{
	BaseClass::SetVisible(state);
	SetScrollbarButtonsVisible(state);
}

//-----------------------------------------------------------------------------
// Purpose: Set the value of the scroll bar slider.
//-----------------------------------------------------------------------------
void CFScrollBar::SetValue(int value)
{
	int newValue = clamp(value, _range[0], _range[1]);

	if (_value != newValue)
	{
		_value = newValue;
		PostActionSignal(new KeyValues("ScrollBarMoved", "value", newValue));
	}

	RecomputePosFromValue();
}

//-----------------------------------------------------------------------------
// Purpose: Get the value of the scroll bar slider.
//-----------------------------------------------------------------------------
int CFScrollBar::GetValue(void)
{
	return _value;
}

//-----------------------------------------------------------------------------
// Purpose: Set the range of the scroll bar slider.
// This the range of numbers the slider can scroll through.
//-----------------------------------------------------------------------------
void CFScrollBar::SetRange(int min, int max)
{
	if (max <= min)
		return;

	_range[0] = min;
	_range[1] = max;

	if (_value < min || _value > max)
	{
		int newValue = clamp(_value, min, max);
		SetValue(newValue);
	}

	RecomputePosFromValue();
}

//-----------------------------------------------------------------------------
// Purpose: Gets the range of the scroll bar slider.
// This the range of numbers the slider can scroll through.
//-----------------------------------------------------------------------------
void CFScrollBar::GetRange(int &min, int &max)
{
	min = _range[0];
	max = _range[1];
}

//-----------------------------------------------------------------------------
// Purpose: Set the scrollbar is vertical (true) or horizontal (false)
//-----------------------------------------------------------------------------
void CFScrollBar::SetVertical(bool vertical)
{
	_vertical = vertical;
}

//-----------------------------------------------------------------------------
// Purpose: Check if the scrollbar is vertical (true) or horizontal (false)
//-----------------------------------------------------------------------------
bool CFScrollBar::IsVertical(void)
{
	return _vertical;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::SetSliderSize(int wide, int tall)
{
	_barSize[0] = wide;
	_barSize[1] = tall;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::GetSliderSize(int &wide, int &tall)
{
	wide = _barSize[0];
	tall = _barSize[1];
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::SetSliderImage(const char *fileName)
{
	_barImage = gTexAPI.Load(fileName);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::SetDecPos(int x, int y)
{
	((CFScrollBarButton *)_decButton)->SetPos(x, y);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::GetDecPos(int &x, int &y)
{
	((CFScrollBarButton *)_decButton)->GetPos(x, y);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::SetDecSize(int wide, int tall)
{
	((CFScrollBarButton *)_decButton)->SetSize(wide, tall);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::GetDecSize(int &wide, int &tall)
{
	((CFScrollBarButton *)_decButton)->GetSize(wide, tall);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::SetIncPos(int x, int y)
{
	((CFScrollBarButton *)_incButton)->SetPos(x, y);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::GetIncPos(int &x, int &y)
{
	((CFScrollBarButton *)_incButton)->GetPos(x, y);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::SetIncSize(int wide, int tall)
{
	((CFScrollBarButton *)_incButton)->SetSize(wide, tall);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::GetIncSize(int &wide, int &tall)
{
	((CFScrollBarButton *)_incButton)->GetSize(wide, tall);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::SetDecUpImage(const char *fileName)
{
	((CFScrollBarButton *)_decButton)->SetUpImage(fileName);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::SetDecFocusImage(const char *fileName)
{
	((CFScrollBarButton *)_decButton)->SetFocusImage(fileName);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::SetDecDownImage(const char *fileName)
{
	((CFScrollBarButton *)_decButton)->SetDownImage(fileName);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::SetIncUpImage(const char *fileName)
{
	((CFScrollBarButton *)_incButton)->SetUpImage(fileName);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::SetIncFocusImage(const char *fileName)
{
	((CFScrollBarButton *)_incButton)->SetFocusImage(fileName);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::SetIncDownImage(const char *fileName)
{
	((CFScrollBarButton *)_incButton)->SetDownImage(fileName);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::SetScrollbarButtonsVisible(bool state)
{
	_decButton->SetVisible(state);
	_incButton->SetVisible(state);
}

#define PROPORT ( g_pVGuiSchemeManager->GetProportionalScaledValue )

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	SetSliderSize(PROPORT(inResourceData->GetInt("barWide")), PROPORT(inResourceData->GetInt("barTall")));
	SetSliderImage(inResourceData->GetString("barImage"));

	SetRange(inResourceData->GetInt("rangeMin"), inResourceData->GetInt("rangeMax"));
	SetVertical(inResourceData->GetInt("vertical"));

	_decButton->SetParent(GetParent());
	_incButton->SetParent(GetParent());

	SetDecPos(PROPORT(inResourceData->GetInt("decXPos")), PROPORT(inResourceData->GetInt("decYPos")));
	SetDecSize(PROPORT(inResourceData->GetInt("decWide")), PROPORT(inResourceData->GetInt("decTall")));
	SetDecUpImage(inResourceData->GetString("decUpImage"));
	SetDecFocusImage(inResourceData->GetString("decFocusImage"));
	SetDecDownImage(inResourceData->GetString("decDownImage"));

	SetIncPos(PROPORT(inResourceData->GetInt("incXPos")), PROPORT(inResourceData->GetInt("incYPos")));
	SetIncSize(PROPORT(inResourceData->GetInt("incWide")), PROPORT(inResourceData->GetInt("incTall")));
	SetIncUpImage(inResourceData->GetString("incUpImage"));
	SetIncFocusImage(inResourceData->GetString("incFocusImage"));
	SetIncDownImage(inResourceData->GetString("incDownImage"));
}

//-----------------------------------------------------------------------------
// Purpose: Respond to mouse clicks on the bar
//-----------------------------------------------------------------------------
void CFScrollBar::OnMousePressed(MouseCode code)
{
	if (!IsEnabled())
		return;

	if (code != MOUSE_LEFT)
		return;

	if (_dragging)
		return;

	int x, y;

	input()->GetCursorPosition(x, y);
	ScreenToLocal(x, y);

	if (x < _barPos[0] || x > _barPos[0] + _barSize[0])
		return;

	if (y < _barPos[1] || y > _barPos[1] + _barSize[1])
		return;

	_dragStartPos[0] = x - _barPos[0];
	_dragStartPos[1] = y - _barPos[1];

	input()->SetMouseCapture(GetVPanel());
	_dragging = true;
}

//-----------------------------------------------------------------------------
// Purpose: Treat double clicks as single clicks
//-----------------------------------------------------------------------------
void CFScrollBar::OnMouseDoublePressed(MouseCode code)
{
	OnMousePressed(code);
}

//-----------------------------------------------------------------------------
// Purpose: Stop looking for mouse events when mouse is up.
//-----------------------------------------------------------------------------
void CFScrollBar::OnMouseReleased(MouseCode code)
{
	if (code != MOUSE_LEFT)
		return;

	if (!_dragging)
		return;

	RecomputePosFromValue();

	input()->SetMouseCapture(NULL);
	_dragging = false;
}

//-----------------------------------------------------------------------------
// Purpose: Respond to cursor movements, we only care about clicking and dragging
//-----------------------------------------------------------------------------
void CFScrollBar::OnCursorMoved(int x, int y)
{
	if (!_dragging)
		return;

	int wide, tall;
	GetSize(wide, tall);

	_barPos[0] = clamp(x - _dragStartPos[0], 0, wide - _barSize[0]);
	_barPos[1] = clamp(y - _dragStartPos[1], 0, tall - _barSize[1]);

	RecomputeValueFromPos();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::Paint(void)
{
	int wide, tall;
	GetSize(wide, tall);

//	surface()->DrawSetColor(255, 255, 255, 100);
//	surface()->DrawFilledRect(0, 0, wide, tall);

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 255);

	if (_vertical)
	{
		if (_barImage && _barImage->id)
		{
			gTexAPI.DrawSetTexture(_barImage);
			gTexAPI.DrawTexturedRect(0, _barPos[1], _barSize[0], _barSize[1]);
		}
	}
	else
	{
		if (_barImage && _barImage->id)
		{
			gTexAPI.DrawSetTexture(_barImage);
			gTexAPI.DrawTexturedRect(_barPos[0], 0, _barSize[0], _barSize[1]);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Adjust the bar if panel size has been change
//-----------------------------------------------------------------------------
void CFScrollBar::OnSizeChanged(int newWide, int newTall)
{
	BaseClass::OnSizeChanged(newWide, newTall);
	RecomputePosFromValue();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFScrollBar::OnCommand(const char *command)
{
	if (!IsEnabled())
		return;

	if (!strcmp(command, "_scrollbar_dec"))
	{
		int newValue = clamp(_value - 1, _range[0], _range[1]);
		SetValue(newValue);
	}
	else if (!strcmp(command, "_scrollbar_inc"))
	{
		int newValue = clamp(_value + 1, _range[0], _range[1]);
		SetValue(newValue);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Given the value of the bar, adjust the location
//-----------------------------------------------------------------------------
void CFScrollBar::RecomputePosFromValue(void)
{
	int wide, tall;
	GetSize(wide, tall);

	if (_vertical)
	{
		if (_value == _range[0])
			_barPos[1] = 0;
		else if (_value == _range[1])
			_barPos[1] = tall - _barSize[1];
		else
			_barPos[1] = (tall - _barSize[1]) / (float)(_range[1] - _range[0]) * (_value - _range[0]);
	}
	else
	{
		if (_value == _range[0])
			_barPos[0] = 0;
		else if (_value == _range[1])
			_barPos[0] = wide - _barSize[0];
		else
			_barPos[0] = (wide - _barSize[0]) / (float)(_range[1] - _range[0]) * (_value - _range[0]);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Get the bar value using the location
//-----------------------------------------------------------------------------
void CFScrollBar::RecomputeValueFromPos(void)
{
	int wide, tall;
	GetSize(wide, tall);

	int newValue = 0;

	if (_vertical)
		newValue = _range[0] + _barPos[1] / (float)(tall - _barSize[1]) * (_range[1] - _range[0]);
	else
		newValue = _range[0] + _barPos[0] / (float)(wide - _barSize[0]) * (_range[1] - _range[0]);

	if (_value != newValue)
	{
		_value = newValue;
		PostActionSignal(new KeyValues("ScrollBarMoved", "value", newValue));
	}
}