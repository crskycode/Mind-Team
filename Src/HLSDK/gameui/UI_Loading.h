#ifndef UI_LOADING_H
#define UI_LOADING_H

#include "CF_Dialog.h"

namespace vgui
{
	class CFImage;
}

class UILoading : public vgui::CFDialog
{
	DECLARE_CLASS_SIMPLE(UILoading, vgui::CFDialog);

public:
	UILoading(vgui::Panel *parent);

	void SetMapBg(const char *fileName);
	void SetProgress(int value);

private:
	float		m_iBarWidth;

	vgui::CFImage	*m_cImageLoadingMapBg;
	vgui::CFImage	*m_cImageLoadingProgressBar;
	vgui::CFImage	*m_cImageLoadingWaitIcon;
};

#endif