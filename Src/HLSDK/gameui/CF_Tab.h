#ifndef CF_TAB_H
#define CF_TAB_H

#include <vgui_controls/Panel.h>
#include <utlvector.h>

namespace vgui
{

class CFTab
{
public:
	CFTab();

	// Attach control to TabCtrl
	void AddCtrl(int index, Panel *panel);
	void AddCtrl(int index, CFTab *tab);

	// Visibility
	void SetVisible(bool state);

	// Change visible group
	void Switch(int index);

	// Return current tab index
	int GetIndex(void);

private:
	typedef struct
	{
		int index;
		Panel *panel;
		CFTab *tab;
	} control_t;

	CUtlVector<control_t> _controls;
	int _current;
	bool _visible;
};

}

#endif