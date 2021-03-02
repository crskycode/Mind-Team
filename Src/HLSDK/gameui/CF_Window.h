#ifndef CF_WINDOW_H
#define CF_WINDOW_H

#include <vgui_controls/EditablePanel.h>

namespace vgui
{

class CFWindow : public EditablePanel
{
	DECLARE_CLASS_SIMPLE(CFWindow, EditablePanel);

public:
	CFWindow(Panel *parent, const char *panelName);

	void SetProportionalBase(int width, int height);
	void GetProportionalBase(int &width, int &height);

	virtual void LoadControlSettings(const char *dialogResourceName, const char *pathID = NULL, KeyValues *pPreloadedKeyValues = NULL);

protected:
	virtual void OnSizeChanged(int wide, int tall);
	virtual void PaintBackground(void);
	virtual void Paint(void);
	virtual void PaintBorder(void);

private:
	void LayoutProportional(void);

	int	m_iProportionalWidth;
	int	m_iProportionalHeight;
};

}

#endif