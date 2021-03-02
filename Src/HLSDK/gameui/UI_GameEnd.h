#ifndef UI_GAMEEND_H
#define UI_GAMEEND_H

#include "CF_Dialog.h"

namespace vgui
{
	class CFButton;
	class CFStatic;
}

class UIGameEnd : public vgui::CFDialog
{
	DECLARE_CLASS_SIMPLE(UIGameEnd, vgui::CFDialog);

public:
	UIGameEnd(vgui::Panel *parent);

	virtual void Activate(vgui::Panel *owner);

protected:
	virtual void OnCommand(const char *command);
	virtual void OnKeyCodePressed(vgui::KeyCode code);

private:
	// a parent panel to handle MoveToServer button command
	vgui::Panel			*m_pOwnerPanel;

	vgui::CFButton		*m_cButtonGameEnd;
	vgui::CFButton		*m_cButtonMoveServer;
	vgui::CFButton		*m_cButtonCancel;
	vgui::CFStatic		*m_cStaticNameComment;
	vgui::CFStatic		*m_cStaticMoreGameComment;
	vgui::CFStatic		*m_cStaticMoreRound;
	vgui::CFStatic		*m_cStaticNextClass;
	vgui::CFStatic		*m_cStaticNoEP;
	vgui::CFStatic		*m_cStaticTodayEP;
	vgui::CFStatic		*m_cStaticTotalEP;
	vgui::CFStatic		*m_cStaticTodayGP;
	vgui::CFStatic		*m_cStaticTotalGP;
	vgui::CFStatic		*m_cStaticWinLose;
	vgui::CFStatic		*m_cStaticKill;
	vgui::CFStatic		*m_cStaticDeath;
	vgui::CFStatic		*m_cStaticKillDeath;
	vgui::CFStatic		*m_cStaticHeadShot;
};

#endif