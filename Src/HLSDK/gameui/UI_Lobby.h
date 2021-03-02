#ifndef UI_LOBBY_H
#define UI_LOBBY_H

#include "CF_Dialog.h"
#include "CF_Tab.h"
#include <utlvector.h>

namespace vgui
{
	class CFButton;
	class CFRadioButton;
	class CFStatic;
	class CFImage;
	class CFScrollBar;
}

class UIFlowTop;

class UILobby : public vgui::CFDialog
{
	DECLARE_CLASS_SIMPLE(UILobby, vgui::CFDialog);

public:
	UILobby(vgui::Panel *parent);

	virtual void Activate(void);

protected:
	MESSAGE_FUNC_PARAMS(OnCommandFull, "Command", params);
	MESSAGE_FUNC_PTR_INT(OnRadioButtonCheck, "RadioButtonChecked", panel, tabposition);
	virtual void OnKeyCodePressed(vgui::KeyCode code);

private:
	void TabRoomInfo(void);
	void TabMyInfo(void);

	vgui::CFTab			m_cInfoTab;

	UIFlowTop			*m_pUI_FlowTop;

	vgui::CFStatic		*m_cStaticNotice;

	vgui::CFButton		*m_cButtonRoomlist[13];
	vgui::CFStatic		*m_cStaticRoomNumberList[13];
	vgui::CFStatic		*m_cStaticRoomTitleList[13];
	vgui::CFStatic		*m_cStaticRoomRuleList[13];
	vgui::CFStatic		*m_cStaticRoomMapList[13];
	vgui::CFStatic		*m_cStaticRoomPersonList[13];
	vgui::CFStatic		*m_cStaticRoomStateList[13];
	vgui::CFImage		*m_cImageWeaponTypeIconList[13];

	vgui::CFScrollBar	*m_cScrollBarRoomList;
	vgui::Button		*m_cButtonMakeRoom;
	vgui::Button		*m_cButtonRoomEnter;

	vgui::CFImage		*m_cImageRoomInfoBG;
	vgui::CFImage		*m_cImageMapInfo;
	vgui::CFImage		*m_cImageSubRule;
	vgui::CFStatic		*m_cStaticRoomMap;
	vgui::CFStatic		*m_cStaticRoomRule;
	vgui::CFStatic		*m_cStaticRoomGoal;
	vgui::CFStatic		*m_cStaticRoomState;
	vgui::CFStatic		*m_cStaticRoomOwner;

	vgui::CFImage		*m_cImageMyInfoBG;
	vgui::CFStatic		*m_cStaticCallName;
	vgui::CFImage		*m_cImageMyCurClass;
	vgui::CFImage		*m_cImageMyNextClass;
	vgui::CFImage		*m_cImageEPGauge;
	vgui::CFStatic		*m_cStaticEP;
	vgui::CFStatic		*m_cStaticMyClanName;
	vgui::CFStatic		*m_cStaticKillDeath;
	vgui::CFStatic		*m_cStaticWinLose;
	vgui::CFStatic		*m_cStaticManner;
	vgui::CFStatic		*m_cStaticGP;
	vgui::CFStatic		*m_cStaticCP;
	vgui::CFButton		*m_cButtonIDCard;

	vgui::CFRadioButton	*m_cButtonRoomInfo;
	vgui::CFRadioButton	*m_cButtonMyInfo;
};

#endif