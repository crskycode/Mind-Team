#ifndef UI_MAKEROOM_H
#define UI_MAKEROOM_H

#include "CF_Dialog.h"

namespace vgui
{
	class CFImage;
	class CFStatic;
	class CFButton;
};

class UIMakeRoom : public vgui::CFDialog
{
	DECLARE_CLASS_SIMPLE(UIMakeRoom, vgui::CFDialog);

public:
	UIMakeRoom(vgui::Panel *parent);

protected:
	MESSAGE_FUNC_PARAMS(OnCommandFull, "Command", params);

private:
	//
};

#endif