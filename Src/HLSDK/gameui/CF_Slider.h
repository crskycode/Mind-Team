#ifndef CF_SLIDER_H
#define CF_SLIDER_H

#include <vgui_controls/Panel.h>

// ITextureAPI.h
typedef struct tex_s tex_t;

namespace vgui
{

class CFSlider : public Panel
{
	DECLARE_CLASS_SIMPLE(CFSlider, Panel);

public:
	CFSlider(Panel *parent, const char *panelName);

	// Set/Get the value of the scroll bar slider
	virtual void SetValue(int value);
	virtual int GetValue(void);

	// Move the sliber
	virtual void MoveUp(void);
	virtual void MoveDown(void);

	// Set/Get the rangeof numbers the slider can scroll through
	virtual void SetRange(int min, int max);
	virtual void GetRange(int &min, int &max);

	// Check if the slider is vertical or horizontal
	virtual void SetVertical(bool vertical);
	virtual bool IsVertical(void);

	// Set the slider snapto location of value
	virtual void SetAligned(bool state);

	// Set/Get the slider size
	virtual void SetSliderSize(int wide, int tall);
	virtual void GetSliderSize(int &wide, int &tall);

	// Set the slider images
	virtual void SetUpImage(const char *fileName);
	virtual void SetFocusImage(const char *fileName);
	virtual void SetDownImage(const char *fileName);

protected:
	// Interface to build settings
	virtual void ApplySettings(KeyValues *inResourceData);

	// Input messages
	virtual void OnMousePressed(MouseCode code);
	virtual void OnMouseDoublePressed(MouseCode code);
	virtual void OnMouseReleased(MouseCode code);
	virtual void OnCursorMoved(int x, int y);

	// Paint messages
	virtual void Paint(void);

	// Adjust the bar if panel size has been change
	virtual void OnSizeChanged(int newWide, int newTall);

private:
	// Adjust the locaiton by the slider value
	void RecomputePosFromValue(void);
	// Get slider value by the location
	void RecomputeValueFromPos(void);

	bool		_vertical;
	int			_sliderPos[2];
	int			_sliderSize[2];
	bool		_dragging;
	int			_dragStartPos[2];
	int			_range[2];
	int			_value;
	bool		_aligned;
	tex_t		*_sliderUpImage;
	tex_t		*_sliderFocusImage;
	tex_t		*_sliderDownImage;
};

}

#endif