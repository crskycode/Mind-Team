#ifndef UI_MSGWND_H
#define UI_MSGWND_H

#include "CF_Dialog.h"

namespace vgui
{
	class CFButton;
	class CFStatic;
};

class UIMsgWnd : public vgui::CFDialog
{
	DECLARE_CLASS_SIMPLE(UIMsgWnd, vgui::CFDialog);

public:
	UIMsgWnd(vgui::Panel *parent);

	void Create(int type, const wchar_t *msg);
	void Destory(void);

protected:
	virtual void OnCommand(const char *command);

private:
	vgui::CFStatic		*m_cStaticMsg;
};

#endif