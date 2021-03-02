#include "CF_Tab.h"

using namespace vgui;

CFTab::CFTab()
{
	_current = 0;
	_visible = true;
}

void CFTab::AddCtrl(int index, Panel *panel)
{
	if (!panel)
		return;

	int i = _controls.AddToTail();

	_controls[i].index = index;
	_controls[i].panel = panel;
	_controls[i].tab = NULL;
}

void CFTab::AddCtrl(int index, CFTab *tab)
{
	if (!tab)
		return;

	int i = _controls.AddToTail();

	_controls[i].index = index;
	_controls[i].panel = NULL;
	_controls[i].tab = tab;
}

void CFTab::SetVisible(bool state)
{
	_visible = state;

	for (int i = 0; i < _controls.Count(); i++)
	{
		if (_controls[i].panel)
			_controls[i].panel->SetVisible(state);
		
		if (_controls[i].tab)
			_controls[i].tab->SetVisible(state);
	}
}

void CFTab::Switch(int index)
{
	if (_visible)
	{
		for (int i = 0; i < _controls.Count(); i++)
		{
			if (_controls[i].index != index)
			{
				if (_controls[i].panel)
					_controls[i].panel->SetVisible(false);

				if (_controls[i].tab)
					_controls[i].tab->SetVisible(false);
			}
			else
			{
				if (_controls[i].panel)
					_controls[i].panel->SetVisible(true);

				if (_controls[i].tab)
					_controls[i].tab->SetVisible(true);
			}
		}
	}

	_current = index;
}

int CFTab::GetIndex(void)
{
	return _current;
}