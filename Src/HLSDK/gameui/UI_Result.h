#ifndef UI_RESULT_H
#define UI_RESULT_H

#include "CF_Dialog.h"

namespace vgui
{
	class CFImage;
	class CFButton;
	class CFStatic;
}

class UIResult : public vgui::CFDialog
{
	DECLARE_CLASS_SIMPLE(UIResult, vgui::CFDialog);

public:
	UIResult(vgui::Panel *parent);

protected:

private:
};

#endif