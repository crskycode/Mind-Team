#ifndef CF_SCROLLBAR_H
#define CF_SCROLLBAR_H

#include <vgui_controls/Panel.h>
#include <vgui_controls/Button.h>

// ITextureAPI.h
typedef struct tex_s tex_t;

namespace vgui
{

class CFScrollBar : public Panel
{
	DECLARE_CLASS_SIMPLE(CFScrollBar, Panel);

public:
	CFScrollBar(Panel *parent, const char *panelName);

	// Drawing state
	virtual void SetEnabled(bool state);

	// panel visibility
	virtual void SetVisible(bool state);

	// Set/Get the value of the scroll bar slider
	virtual void SetValue(int value);
	virtual int GetValue(void);

	// Set/Get the rangeof numbers the slider can scroll through
	virtual void SetRange(int min, int max);
	virtual void GetRange(int &min, int &max);

	// Check if the scrollbar is vertical or horizontal
	virtual void SetVertical(bool vertical);
	virtual bool IsVertical(void);

	// Set/Get scrollbar slider size
	virtual void SetSliderSize(int wide, int tall);
	virtual void GetSliderSize(int &wide, int &tall);

	// Set scrollbar images
	virtual void SetSliderImage(const char *fileName);

	// Set/Get scrollbar button location
	virtual void SetDecPos(int x, int y);
	virtual void GetDecPos(int &x, int &y);
	virtual void SetDecSize(int wide, int tall);
	virtual void GetDecSize(int &wide, int &tall);
	virtual void SetIncPos(int x, int y);
	virtual void GetIncPos(int &x, int &y);
	virtual void SetIncSize(int wide, int tall);
	virtual void GetIncSize(int &wide, int &tall);

	// Set scrollbar button images
	virtual void SetDecUpImage(const char *fileName);
	virtual void SetDecFocusImage(const char *fileName);
	virtual void SetDecDownImage(const char *fileName);
	virtual void SetIncUpImage(const char *fileName);
	virtual void SetIncFocusImage(const char *fileName);
	virtual void SetIncDownImage(const char *fileName);

	// Sets the scrollbar buttons visible or not
	virtual void SetScrollbarButtonsVisible(bool state);

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

	// Handle ScrollBarButton message
	virtual void OnCommand(const char *command);

private:
	// Adjust the bar of value
	void RecomputePosFromValue(void);
	// Get the value by bar location
	void RecomputeValueFromPos(void);

	bool		_vertical;
	int			_barPos[2];
	int			_barSize[2];
	bool		_dragging;
	int			_dragStartPos[2];
	int			_range[2];
	int			_value;
	Panel		*_decButton;
	Panel		*_incButton;
	tex_t		*_barImage;
};

}

#endif