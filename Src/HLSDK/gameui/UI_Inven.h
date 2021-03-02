#ifndef UI_INVEN_H
#define UI_INVEN_H

#include "CF_Dialog.h"
#include "CF_Tab.h"

#include "ShopItemManager.h"
#include "UserSystem.h"

namespace vgui
{
	class CFButton;
	class CFStatic;
	class CFImage;
	class CFRadioButton;
	class CFScrollBar;
}

class UIFlowTop;

class UIInven : public vgui::CFDialog
{
	DECLARE_CLASS_SIMPLE(UIInven, vgui::CFDialog);

public:
	UIInven(vgui::Panel *parent);

	virtual void Activate(void);

protected:
	MESSAGE_FUNC_PARAMS(OnCommandFull, "Command", params);
	MESSAGE_FUNC_PTR_INT(OnRadioButtonCheck, "RadioButtonChecked", panel, tabposition);
	MESSAGE_FUNC_INT(OnScrollBarMove, "ScrollBarMoved", value);

	virtual void OnMouseWheeled(int delta);
	virtual void OnKeyCodePressed(vgui::KeyCode code);

public:
	// if click the BACK button we go back to it
	vgui::Panel			*m_pOwnerPanel;

private:
	void TabWeapon(void);
	void TabChar(void);
	void TabItem(void);
	void TabPresent(void);
	void TabBuyList(void);
	void TabAIItem(void);

	void TabMyBag(void);
	void TabMyChar(void);
	void TabMyAcce(void);

	//
	bool m_bHasBombPackage;

	void RefBagTab(int iBagNum);
	int m_iCurrentBag;

	void RefWeaponList(int iCategory, int iCategory2);
	void RefCharList(int iCategory);
	void RefItemList(void);
	void RefPresentList(void);
	void RefBuyListList(void);
	void RefAIItemList(void);

	// list
	CUtlVector<CInvenItem*>	m_ItemBuffer;
	// called on the scrollbar moved
	void (UIInven::*m_pfnOnScrollBarMove)(int value);

	// this function to handle the scrollbar message
	void OnScrollBarMove_Weapon(int value);
	void OnScrollBarMove_Char(int value);
	void OnScrollBarMove_Item(int value);
	void OnScrollBarMove_Present(int value);
	void OnScrollBarMove_BuyList(int value);
	void OnScrollBarMove_AIItem(int value);

	vgui::CFTab				m_cItemTab;
	vgui::CFTab				m_cUniformInfoTab;

	UIFlowTop				*m_pUI_FlowTop;

	vgui::CFStatic			*m_cStaticCaption;

	vgui::CFImage			*m_cImageWeaponTab;
	vgui::CFRadioButton		*m_cRadioWeapon;
	vgui::CFRadioButton		*m_cRadioChar;
	vgui::CFRadioButton		*m_cRadioItem;
	vgui::CFRadioButton		*m_cRadioPresent;
	vgui::CFRadioButton		*m_cRadioAIItemTab;
	vgui::CFRadioButton		*m_cRadioBuyListSet;

	vgui::CFRadioButton		*m_cButtonAll;
	vgui::CFRadioButton		*m_cButtonPrimary;
	vgui::CFRadioButton		*m_cButtonSecondary;
	vgui::CFRadioButton		*m_cButtonKnife;
	vgui::CFRadioButton		*m_cButtonHE;
	vgui::CFRadioButton		*m_cButtonSpecialized;

	vgui::CFRadioButton		*m_cRadioAll;
	vgui::CFRadioButton		*m_cRadioCharacter;
	vgui::CFRadioButton		*m_cRadioHead;
	vgui::CFRadioButton		*m_cRadioUpper;
	vgui::CFRadioButton		*m_cRadioLower;
	vgui::CFRadioButton		*m_cRadioAccessory;

	vgui::CFButton			*m_cButtonIDCard;
	vgui::CFStatic			*m_cStaticWhoseInfo;
	vgui::CFStatic			*m_cStaticEP;
	vgui::CFStatic			*m_cStaticGP;
	vgui::CFStatic			*m_cStaticCP;
	vgui::CFImage			*m_cImageEPGauge;
	vgui::CFImage			*m_cImageMyCurClass;
	vgui::CFImage			*m_cImageMyNextClass;

	vgui::CFButton			*m_cButtonRepairAll;
	vgui::CFButton			*m_cButtonReplay;

	vgui::CFImage			*m_cImageNosel[10];
	vgui::CFImage			*m_cImageItem[10];
	vgui::CFImage			*m_cImageBagnumber[10];
	vgui::CFImage			*m_cImageSelup[10];
	vgui::CFButton			*m_cButtonSel[10];
	vgui::CFStatic			*m_cStaticWeaponname[10];
	vgui::CFImage			*m_cImageDurabilityBack[10];
	vgui::CFImage			*m_cImageDurability[10];
	vgui::CFStatic			*m_cStaticExpire[10];
	vgui::CFButton			*m_cButtonRepair[10];
	vgui::CFButton			*m_cButtonDelete[10];
	vgui::CFButton			*m_cButtonPeriodExtention[10];

	vgui::CFScrollBar		*m_cScrollBarInven;

	vgui::CFRadioButton		*m_cRadioMyBag;
	vgui::CFRadioButton		*m_cRadioMyChar;
	vgui::CFRadioButton		*m_cRadioMyAccessory;

	vgui::CFImage			*m_cImageCharLeftTab;
	vgui::CFImage			*m_cImageMyCharBG;
	vgui::CFButton			*m_cButtonEquipCharLeft;
	vgui::CFButton			*m_cButtonEquipCharRight;
	vgui::CFRadioButton		*m_cButtonBL;
	vgui::CFRadioButton		*m_cButtonGR;
	vgui::CFImage			*m_cImageMainChar;
	vgui::CFButton			*m_cButtonSetMainChar;
	vgui::CFButton			*m_cButtonPriorChar;
	vgui::CFButton			*m_cButtonNextChar;
	vgui::CFButton			*m_cButtonEquipSel[7];
	vgui::CFButton			*m_cCOMBO_Char;

	vgui::CFImage			*m_cImageLeftWeaponTab;
	vgui::CFRadioButton		*m_cRadioBag[3];			// 背包按钮
	vgui::CFImage			*m_cImageBagItem[6];		// 道具图标
	vgui::CFButton			*m_cButtonBagslot[6];		// 道具按钮
	vgui::CFButton			*m_cButtonExtract[6];		// 解除按钮
	vgui::CFStatic			*m_cStaticBagItemName[3];	// 道具名称
	vgui::CFButton			*m_cButtonRepairSlot[2];	// 维修按钮
	vgui::CFImage			*m_cImageWeaponGaugeBG[2];	// 道具损坏度背景
	vgui::CFImage			*m_cImageWeaponGauge[2];	// 道具损坏度
	vgui::CFImage			*m_cImageBagGrenadeX[2];	// ThrowWeaponSlotClose

	vgui::CFImage			*m_cImageAccTab;
	vgui::CFImage			*m_cImageMyAccBG;
};

#endif