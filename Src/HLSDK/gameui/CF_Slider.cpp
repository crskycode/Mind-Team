#include "CF_Slider.h"
#include "IMTEngine_Internal.h"
#include <KeyValues.h>

using namespace vgui;

DECLARE_BUILD_FACTORY(CFSlider);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CFSlider::CFSlider(Panel *parent, const char *panelName) : Panel(parent, panelName)
{
	_vertical = false;
	_sliderPos[0] = 0;
	_sliderPos[1] = 0;
	_sliderSize[0] = 0;
	_sliderSize[0] = 0;
	_dragging = false;
	_dragStartPos[0] = 0;
	_dragStartPos[1] = 0;
	_range[0] = 0;
	_range[1] = 1;
	_value = 0;
	_aligned = false;
	_sliderUpImage = NULL;
	_sliderFocusImage = NULL;
	_sliderDownImage = NULL;

	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);
}

//-----------------------------------------------------------------------------
// Purpose: Set the value of the slider.
//-----------------------------------------------------------------------------
void CFSlider::SetValue(int value)
{
	int newValue = clamp(value, _range[0], _range[1]);

	if (_value != newValue)
	{
		_value = newValue;
		PostActionSignal(new KeyValues("SliderMoved", "value", newValue));
	}

	RecomputePosFromValue();
}

//-----------------------------------------------------------------------------
// Purpose: Get the value of the slider.
//-----------------------------------------------------------------------------
int CFSlider::GetValue(void)
{
	return _value;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFSlider::MoveUp(void)
{
	int newValue = clamp(_value - 1, _range[0], _range[1]);

	if (_value != newValue)
	{
		SetValue(newValue);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFSlider::MoveDown(void)
{
	int newValue = clamp(_value + 1, _range[0], _range[1]);

	if (_value != newValue)
	{
		SetValue(newValue);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Set the range of the slider.
// This the range of numbers the slider can scroll through.
//-----------------------------------------------------------------------------
void CFSlider::SetRange(int min, int max)
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
// Purpose: Gets the range of the slider.
// This the range of numbers the slider can scroll through.
//-----------------------------------------------------------------------------
void CFSlider::GetRange(int &min, int &max)
{
	min = _range[0];
	max = _range[1];
}

//-----------------------------------------------------------------------------
// Purpose: Set the slider is vertical (true) or horizontal (false)
//-----------------------------------------------------------------------------
void CFSlider::SetVertical(bool vertical)
{
	_vertical = vertical;
}

//-----------------------------------------------------------------------------
// Purpose: Check if the slider is vertical (true) or horizontal (false)
//-----------------------------------------------------------------------------
bool CFSlider::IsVertical(void)
{
	return _vertical;
}

//-----------------------------------------------------------------------------
// Purpose: Set the slider snapto location of value
//-----------------------------------------------------------------------------
void CFSlider::SetAligned(bool state)
{
	_aligned = state;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFSlider::SetSliderSize(int wide, int tall)
{
	_sliderSize[0] = wide;
	_sliderSize[1] = tall;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFSlider::GetSliderSize(int &wide, int &tall)
{
	wide = _sliderSize[0];
	tall = _sliderSize[1];
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFSlider::SetUpImage(const char *fileName)
{
	_sliderUpImage = gTexAPI.Load(fileName);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFSlider::SetFocusImage(const char *fileName)
{
	_sliderFocusImage = gTexAPI.Load(fileName);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFSlider::SetDownImage(const char *fileName)
{
	_sliderDownImage = gTexAPI.Load(fileName);
}

#define PROPORT ( g_pVGuiSchemeManager->GetProportionalScaledValue )

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFSlider::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	_sliderSize[0] = PROPORT(inResourceData->GetInt("sliderWide"));
	_sliderSize[1] = PROPORT(inResourceData->GetInt("sliderTall"));

	SetRange(inResourceData->GetInt("rangeMin"), inResourceData->GetInt("rangeMax"));
	SetVertical(inResourceData->GetInt("vertical"));
	SetAligned(inResourceData->GetInt("aligned"));
	SetUpImage(inResourceData->GetString("upImage"));
	SetFocusImage(inResourceData->GetString("focusImage"));
	SetDownImage(inResourceData->GetString("downImage"));
}

//-----------------------------------------------------------------------------
// Purpose: Respond to mouse clicks on the bar
//-----------------------------------------------------------------------------
void CFSlider::OnMousePressed(MouseCode code)
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

	if (x < _sliderPos[0] || x > _sliderPos[0] + _sliderSize[0])
		return;

	if (y < _sliderPos[1] || y > _sliderPos[1] + _sliderSize[1])
		return;

	_dragStartPos[0] = x - _sliderPos[0];
	_dragStartPos[1] = y - _sliderPos[1];

	input()->SetMouseCapture(GetVPanel());
	_dragging = true;
}

//-----------------------------------------------------------------------------
// Purpose: Treat double clicks as single clicks
//-----------------------------------------------------------------------------
void CFSlider::OnMouseDoublePressed(MouseCode code)
{
	OnMousePressed(code);
}

//-----------------------------------------------------------------------------
// Purpose: Stop looking for mouse events when mouse is up.
//-----------------------------------------------------------------------------
void CFSlider::OnMouseReleased(MouseCode code)
{
	if (code != MOUSE_LEFT)
		return;

	if (!_dragging)
		return;

	input()->SetMouseCapture(NULL);
	_dragging = false;

	if (_aligned)
	{
		RecomputePosFromValue();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Respond to cursor movements, we only care about clicking and dragging
//-----------------------------------------------------------------------------
void CFSlider::OnCursorMoved(int x, int y)
{
	if (!_dragging)
		return;

	int wide, tall;
	GetSize(wide, tall);

	_sliderPos[0] = clamp(x - _dragStartPos[0], 0, wide - _sliderSize[0]);
	_sliderPos[1] = clamp(y - _dragStartPos[1], 0, tall - _sliderSize[1]);

	RecomputeValueFromPos();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFSlider::Paint(void)
{
	int wide, tall;
	GetSize(wide, tall);

//	surface()->DrawSetColor(80, 80, 80, 80);
//	surface()->DrawFilledRect(0, 0, wide, tall);

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 255);

	tex_t *drawingImage = NULL;

	int x, y;

	input()->GetCursorPosition(x, y);
	ScreenToLocal(x, y);

	if (IsEnabled())
	{
		if (_dragging)
			drawingImage = _sliderDownImage;
		else if ((x >= _sliderPos[0] && x <= _sliderPos[0] + _sliderSize[0]) && (y >= _sliderPos[1] && y <= _sliderPos[1] + _sliderSize[1]))
			drawingImage = _sliderFocusImage;
		else
			drawingImage = _sliderUpImage;
	}
	else
		drawingImage = _sliderUpImage;

	if (_vertical)
	{
		if (drawingImage && drawingImage->id)
		{
			gTexAPI.DrawSetTexture(drawingImage);
			gTexAPI.DrawTexturedRect(0, _sliderPos[1], _sliderSize[0], _sliderSize[1]);
		}
	}
	else
	{
		if (drawingImage && drawingImage->id)
		{
			gTexAPI.DrawSetTexture(drawingImage);
			gTexAPI.DrawTexturedRect(_sliderPos[0], 0, _sliderSize[0], _sliderSize[1]);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Adjust the bar if panel size has been change
//-----------------------------------------------------------------------------
void CFSlider::OnSizeChanged(int newWide, int newTall)
{
	BaseClass::OnSizeChanged(newWide, newTall);
	RecomputePosFromValue();
}

//-----------------------------------------------------------------------------
// Purpose: Given the value of the slider, adjust the location
//-----------------------------------------------------------------------------
void CFSlider::RecomputePosFromValue(void)
{
	int wide, tall;
	GetSize(wide, tall);

	if (_vertical)
	{
		if (_value == _range[0])
			_sliderPos[1] = 0;
		else if (_value == _range[1])
			_sliderPos[1] = tall - _sliderSize[1];
		else
			_sliderPos[1] = (tall - _sliderSize[1]) / (float)(_range[1] - _range[0]) * (_value - _range[0]);
	}
	else
	{
		if (_value == _range[0])
			_sliderPos[0] = 0;
		else if (_value == _range[1])
			_sliderPos[0] = wide - _sliderSize[0];
		else
			_sliderPos[0] = (wide - _sliderSize[0]) / (float)(_range[1] - _range[0]) * (_value - _range[0]);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Get the slider value using the location
//-----------------------------------------------------------------------------
void CFSlider::RecomputeValueFromPos(void)
{
	int wide, tall;
	GetSize(wide, tall);

	int newValue = _value;

	if (_vertical)
	{
		float rangeLen = tall - _sliderSize[1];
		float tickLen = rangeLen / (float)(_range[1] - _range[0]);
		float sliderPos = _sliderPos[1];

		if (sliderPos <= 0)
			newValue = _range[0];
		else if (sliderPos >= rangeLen)
			newValue = _range[1];
		else
		{
			for (int tick = 0; tick < _range[1] - _range[0]; tick++)
			{
				float tickPos = tick * tickLen;
				float nextTickPos = (tick + 1) * tickLen;
				float centerOfTick = tickPos + (nextTickPos - tickPos) / 2.0f;

				if (sliderPos >= tickPos && sliderPos < nextTickPos)
				{
					if (sliderPos < centerOfTick)
						newValue = _range[0] + tick;
					else if (sliderPos >= centerOfTick)
						newValue = _range[0] + tick + 1;

					break;
				}
			}
		}
	}
	else
	{
		float rangeLen = wide - _sliderSize[0];
		float tickLen = rangeLen / (float)(_range[1] - _range[0]);
		float sliderPos = _sliderPos[0];

		if (sliderPos <= 0)
			newValue = _range[0];
		else if (sliderPos >= rangeLen)
			newValue = _range[1];
		else
		{
			for (int tick = 0; tick < _range[1] - _range[0]; tick++)
			{
				float tickPos = tick * tickLen;
				float nextTickPos = (tick + 1) * tickLen;
				float centerOfTick = tickPos + (nextTickPos - tickPos) / 2.0f;

				if (sliderPos >= tickPos && sliderPos < nextTickPos)
				{
					if (sliderPos < centerOfTick)
						newValue = _range[0] + tick;
					else if (sliderPos >= centerOfTick)
						newValue = _range[0] + tick + 1;

					break;
				}
			}
		}
	}

	if (_value != newValue)
	{
		_value = newValue;
		PostActionSignal(new KeyValues("SliderMoved", "value", newValue));
	}
}