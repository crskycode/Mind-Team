#ifndef UI_SHOP_H
#define UI_SHOP_H

#include "CF_Dialog.h"

namespace vgui
{
	class CFImage;
	class CFStatic;
	class CFButton;
	class CFRadioButton;
	class CFScrollBar;
}

class UIFlowTop;

class UIShop : public vgui::CFDialog
{
	DECLARE_CLASS_SIMPLE(UIShop, vgui::CFDialog);

public:
	UIShop(vgui::Panel *parent);

protected:
	MESSAGE_FUNC_PARAMS(OnCommandFull, "Command", params);

private:
	//
};

#endif