#ifndef UI_CHANNELSELECT_H
#define UI_CHANNELSELECT_H

#include "CF_Dialog.h"
#include <UtlVector.h>

namespace vgui
{
	class CFButton;
	class CFStatic;
	class CFImage;
}

class UIFlowTop;
class CShopItem;

class UIChannelSelect : public vgui::CFDialog
{
	DECLARE_CLASS_SIMPLE(UIChannelSelect, vgui::CFDialog);

public:
	UIChannelSelect(vgui::Panel *parent);

	virtual void Activate(void);

protected:
	MESSAGE_FUNC_PARAMS(OnCommandFull, "Command", params);
	virtual void OnKeyCodePressed(vgui::KeyCode code);

private:
	void RefHotItem(int step);

	CUtlVector<CShopItem*>	m_ChannelItem;
	int						m_iHotlItemStep;

	UIFlowTop	*m_pUI_FlowTop;

	vgui::CFButton			*m_cButtonChannel[13];
	vgui::CFImage			*m_cImageGaugeBack[13];
	vgui::CFImage			*m_cImageServerGauge[13];
	vgui::CFStatic			*m_cStaticChannelName[13];
	vgui::CFStatic			*m_cStaticNotice;
	vgui::CFButton			*m_cButtonRefresh;
	vgui::CFButton			*m_cButtonChannelEnter;
	vgui::CFStatic			*m_cStaticMyCallName;
	vgui::CFStatic			*m_cStaticMyClanName;
	vgui::CFStatic			*m_cStaticMyGP;
	vgui::CFStatic			*m_cStaticMyCP;
	vgui::CFStatic			*m_cStaticMyEP;
	vgui::CFImage			*m_cImageClassGauge;
	vgui::CFImage			*m_cImageMyCurClass;
	vgui::CFImage			*m_cImageMyNextClass;
	vgui::CFButton			*m_cButtonIDCard;
	vgui::CFStatic			*m_cStaticKillDeath;
	vgui::CFStatic			*m_cStaticWinLose;
	vgui::CFStatic			*m_cStaticManner;
	vgui::CFImage			*m_cImageChannelItem[2];
	vgui::CFImage			*m_cImageChannelItemTag[2];
	vgui::CFStatic			*m_cStaticItemName[2];
	vgui::CFStatic			*m_cStaticItemGP[2];
	vgui::CFButton			*m_cButtonItem[2];
	vgui::CFButton			*m_cButtonPrev;
	vgui::CFButton			*m_cButtonNext;
};

#endif