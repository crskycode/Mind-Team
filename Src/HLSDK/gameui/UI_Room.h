#ifndef UI_ROOM_H
#define UI_ROOM_H

#include "CF_Dialog.h"
#include "CF_Tab.h"

namespace vgui
{
	class CFButton;
	class CFStatic;
	class CFRadioButton;
	class CFImage;
}

class UIFlowTop;

class UIRoom : public vgui::CFDialog
{
	DECLARE_CLASS_SIMPLE(UIRoom, vgui::CFDialog);

public:
	UIRoom(vgui::Panel *parent);

	virtual void Activate(void);

protected:
	MESSAGE_FUNC_PARAMS(OnCommandFull, "Command", params);
	MESSAGE_FUNC_PTR_INT(OnRadioButtonCheck, "RadioButtonChecked", panel, tabposition);
	virtual void OnCursorMoved(int x, int y);
	virtual void OnKeyCodePressed(vgui::KeyCode code);

private:
	void RefBagInfo(int iBagNumber);
	void RefBagInfoExtra(void);

	int m_iCurrentBag;

	//
	bool m_bHasBombPackage;

	vgui::CFTab			m_cInfoTab;

	UIFlowTop			*m_pUI_FlowTop;

	vgui::CFStatic		*m_cStaticNotice;
	vgui::CFImage		*m_cImageBagBG[2];
	vgui::CFImage		*m_cImageCashWP[2];
	vgui::CFImage		*m_cImageSlotRed[2];
	vgui::CFImage		*m_cImageWP[6];
	vgui::CFRadioButton	*m_cRoomRadioBag[3];
	vgui::CFImage		*m_cImageWPGuageBG[2];
	vgui::CFImage		*m_cImageWPGuage[2];
	vgui::CFButton		*m_cButtonRepair[2];
	vgui::CFStatic		*m_cStaticWPName[3];
	vgui::CFStatic		*m_cStaticExpireWP[3];
	vgui::CFImage		*m_cImageHEDisable[2];

	vgui::CFImage		*m_cImageMyInfoBG;
	vgui::CFStatic		*m_cStaticCallName;
	vgui::CFImage		*m_cImageMyCurClass;
	vgui::CFImage		*m_cImageMyNextClass;
	vgui::CFImage		*m_cImageEPGauge;
	vgui::CFStatic		*m_cStaticEP;
	vgui::CFStatic		*m_cStaticMyClanName;
	vgui::CFStatic		*m_cStaticKilldeath;
	vgui::CFStatic		*m_cStaticWinLose;
	vgui::CFStatic		*m_cStaticManner;
	vgui::CFStatic		*m_cStaticGP;
	vgui::CFStatic		*m_cStaticCP;
	vgui::CFButton		*m_cButtonIDCard;

	vgui::CFRadioButton	*m_cButtonBag;
	vgui::CFRadioButton	*m_cButtonMyInfo;

	vgui::CFButton		*m_cButtonCLeave;		// 踢人
	vgui::CFButton		*m_cButtonHostinvite;	// 邀请
	vgui::CFButton		*m_cButtonStart;		// 开始
	vgui::CFButton		*m_cButtonNanipstart;	// 加入
};

#endif