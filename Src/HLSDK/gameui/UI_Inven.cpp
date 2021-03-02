#include "UI_Inven.h"
#include "UI_FlowTop.h"
#include "CF_Base.h"
#include "EngineInterface.h"
#include "UI_Globals.h"
#include "UserSystem.h"
#include <keyvalues.h>
#include "IMTEngine_Internal.h"

studio_render_t gStudioRender;

class CModelView : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CModelView, vgui::Panel);

public:

	HSTUDIO m_hStudioHandler;
	int m_iBufferWide;
	int m_iBufferTall;
	int m_iStudioBuffer;

	CModelView(vgui::Panel *parent) : vgui::Panel(parent, "CharacterViewer")
	{
		SetPaintBackgroundEnabled(false);
		SetPaintBorderEnabled(false);

		m_hStudioHandler = gStudioRender.Create();

		m_iBufferWide = PROPORT(226);
		m_iBufferTall = PROPORT(306);
		gStudioRender.Init(m_hStudioHandler, m_iBufferWide, m_iBufferTall);
		gStudioRender.SetPerspective(m_hStudioHandler, 45.0, (float)m_iBufferWide / (float)m_iBufferTall, 0.01, 1000.0);
		gStudioRender.Load(m_hStudioHandler, "models/character/arctic.mdl");
		gStudioRender.SetSequence(m_hStudioHandler, 1);
		gStudioRender.SetAngles(m_hStudioHandler, -90, 0, -90);

		m_iStudioBuffer = gStudioRender.GetColorBuffer(m_hStudioHandler);

		m_iDragging = 0;
		m_iCurrentPos[0] = -180;
		m_iCurrentPos[1] = 0;
	}

	~CModelView()
	{
		gStudioRender.Release(m_hStudioHandler);
	}

	void Paint(void)
	{
		static float prev = 0;
		float curr = gEngfuncs.GetClientTime();

		gStudioRender.SetOrigin(m_hStudioHandler, 0, 0, -11.0);
		//gStudioRender.SetAngles(m_hStudioHandler, -90, 0, -90);
		gStudioRender.SetScale(m_hStudioHandler, 0.1);
		gStudioRender.AdvanceFrame(m_hStudioHandler, curr - prev);
		gStudioRender.Draw(m_hStudioHandler);

		prev = curr;

		int iWidth, iHeight;
		GetSize(iWidth, iHeight);

		tex_t tempTexture;
		tempTexture.id = m_iStudioBuffer;
		tempTexture.wide = m_iBufferWide;
		tempTexture.tall = m_iBufferTall;

		gTexAPI.RenderMode(RenderNormal);
		gTexAPI.DrawSetColor(255, 255, 255, 255);
		gTexAPI.DrawSetTexture(&tempTexture);
		gTexAPI.DrawTexturedRectEx(0, iHeight, iWidth, 0);//up to bottom
	}

	void OnMousePressed(vgui::MouseCode code)
	{
		if (code != vgui::MOUSE_LEFT)
			return;

		if (m_iDragging)
			return;

		int x, y;

		vgui::input()->GetCursorPosition(x, y);
		ScreenToLocal(x, y);

		m_iStartPos[0] = x - m_iCurrentPos[0];
		m_iStartPos[1] = y - m_iCurrentPos[1];

		vgui::input()->SetMouseCapture(GetVPanel());
		m_iDragging = 1;
	}

	void OnMouseReleased(vgui::MouseCode code)
	{
		if (code != vgui::MOUSE_LEFT)
			return;

		if (!m_iDragging)
			return;

		vgui::input()->SetMouseCapture(NULL);
		m_iDragging = 0;
	}

	void OnCursorMoved( int x, int y )
	{
		if (!m_iDragging)
			return;

		m_iCurrentPos[0] = x - m_iStartPos[0];
		m_iCurrentPos[1] = y - m_iStartPos[1];

		gStudioRender.SetAngles(m_hStudioHandler, -90, 0, m_iCurrentPos[0] * 0.5);
	}

	int m_iDragging;
	int m_iStartPos[2];
	int m_iCurrentPos[2];
};

UIInven::UIInven(vgui::Panel *parent) : vgui::CFDialog(parent, "Inven")
{
	// Must be enable
	SetProportional(true);

	// Create top window
	m_pUI_FlowTop = new UIFlowTop(this);

	// Load layout setting
	LoadControlSettings("UI/Scripts/CharacterSetting/Inven_BaseNew.txt");
	LoadControlSettings("UI/Scripts/CharacterSetting/Inven_InvenMenu.txt");
	LoadControlSettings("UI/Scripts/CharacterUniform/Uniform_UniformMenu.txt");
	LoadControlSettings("UI/Scripts/CharacterSetting/Inven_Scroll.txt");
	LoadControlSettings("UI/Scripts/CharacterSetting/Inven_UniformInfo.txt");
	LoadControlSettings("UI/Scripts/Common/CampChangeButtons_Inven.txt");
	LoadControlSettings("UI/Scripts/CharacterSetting/Inven_Equip.txt");
	LoadControlSettings("UI/Scripts/CharacterSetting/Inven_ComboChar.txt");

	//CharacterViewer
	gpMTEngine->GetStudioRenderAPI(&gStudioRender);
	CModelView *pModelView = new CModelView(this);
	LoadControlSettings("UI/Scripts/CharacterUniform/Uniform_CharacterViewer.txt");

	// Load buddy window
	LoadControlSettings("UI/Scripts/Buddy_CF20/FlowTop_Buddy.txt");

	m_cStaticCaption = FIND_CONTROL_STATIC("StaticCaption");

	//ItemTab::Control
	m_cImageWeaponTab = FIND_CONTROL_IMAGE("ImgWeaponTab");
	m_cRadioWeapon = FIND_CONTROL_RADIOBUTTON("RadioWeapon");
	m_cRadioChar = FIND_CONTROL_RADIOBUTTON("RadioChar");
	m_cRadioItem = FIND_CONTROL_RADIOBUTTON("RadioItem");
	m_cRadioPresent = FIND_CONTROL_RADIOBUTTON("RadioPresent");
	m_cRadioAIItemTab = FIND_CONTROL_RADIOBUTTON("ButtonAIItemTab");
	m_cRadioBuyListSet = FIND_CONTROL_RADIOBUTTON("RadioBuyListSet");

	//ItemTab::Weapon
	m_cButtonAll = FIND_CONTROL_RADIOBUTTON("ButtonAll");
	m_cButtonPrimary = FIND_CONTROL_RADIOBUTTON("ButtonPrimary");
	m_cButtonSecondary = FIND_CONTROL_RADIOBUTTON("ButtonSecondary");
	m_cButtonKnife = FIND_CONTROL_RADIOBUTTON("ButtonKnife");
	m_cButtonHE = FIND_CONTROL_RADIOBUTTON("ButtonHE");
	m_cButtonSpecialized = FIND_CONTROL_RADIOBUTTON("ButtonSpecialized");

	//ItemTab::Character
	m_cRadioAll = FIND_CONTROL_RADIOBUTTON("RadioAll");
	m_cRadioCharacter = FIND_CONTROL_RADIOBUTTON("RadioCharacter");
	m_cRadioHead = FIND_CONTROL_RADIOBUTTON("RadioHead");
	m_cRadioUpper = FIND_CONTROL_RADIOBUTTON("RadioUpper");
	m_cRadioLower = FIND_CONTROL_RADIOBUTTON("RadioLower");
	m_cRadioAccessory = FIND_CONTROL_RADIOBUTTON("RadioAccessory");

	//MyInfo
	m_cButtonIDCard = FIND_CONTROL_BUTTON("ButtonIDCard");
	m_cStaticWhoseInfo = FIND_CONTROL_STATIC("StaticWhoseInfo");
	m_cStaticEP = FIND_CONTROL_STATIC("StaticEP");
	m_cStaticGP = FIND_CONTROL_STATIC("StaticGP");
	m_cStaticCP = FIND_CONTROL_STATIC("StaticCP");
	m_cImageEPGauge = FIND_CONTROL_IMAGE("ImageEPGauge");
	m_cImageMyCurClass = FIND_CONTROL_IMAGE("ImageMyCurClass");
	m_cImageMyNextClass = FIND_CONTROL_IMAGE("ImageMyNextClass");

	m_cButtonRepairAll = FIND_CONTROL_BUTTON("ButtonRepairAll");
	m_cButtonReplay = FIND_CONTROL_BUTTON("ButtonReplay");

	char szOption[32];

	//ItemList
	for (int i = 0; i < 10; i++)
	{
		sprintf(szOption, "ImageNosel%d", i + 1);
		m_cImageNosel[i] = FIND_CONTROL_IMAGE(szOption);
		sprintf(szOption, "ImageItem%d", i + 1);
		m_cImageItem[i] = FIND_CONTROL_IMAGE(szOption);
		sprintf(szOption, "ImageBagnumber%d", i + 1);
		m_cImageBagnumber[i] = FIND_CONTROL_IMAGE(szOption);
		sprintf(szOption, "ImageSelup%d", i + 1);
		m_cImageSelup[i] = FIND_CONTROL_IMAGE(szOption);
		sprintf(szOption, "ButtonSel%d", i + 1);
		m_cButtonSel[i] = FIND_CONTROL_BUTTON(szOption);
		sprintf(szOption, "StaticWeaponname%d", i + 1);
		m_cStaticWeaponname[i] = FIND_CONTROL_STATIC(szOption);
		sprintf(szOption, "durabilityBack%d", i + 1);
		m_cImageDurabilityBack[i] = FIND_CONTROL_IMAGE(szOption);
		sprintf(szOption, "Imagedurability%d", i + 1);
		m_cImageDurability[i] = FIND_CONTROL_IMAGE(szOption);
		sprintf(szOption, "StaticExpire%d", i + 1);
		m_cStaticExpire[i] = FIND_CONTROL_STATIC(szOption);
		sprintf(szOption, "ButtonRepair%d", i + 1);
		m_cButtonRepair[i] = FIND_CONTROL_BUTTON(szOption);
		sprintf(szOption, "ButtonDelete%d", i + 1);
		m_cButtonDelete[i] = FIND_CONTROL_BUTTON(szOption);
		sprintf(szOption, "ButtonPeriodExtention%d", i + 1);
		m_cButtonPeriodExtention[i] = FIND_CONTROL_BUTTON(szOption);
	}

	m_cScrollBarInven = FIND_CONTROL_SCROLLBAR("ScrollBarInven");

	//UniformInfoTab
	m_cRadioMyBag = FIND_CONTROL_RADIOBUTTON("RadioMyBag");
	m_cRadioMyChar = FIND_CONTROL_RADIOBUTTON("RadioMyChar");
	m_cRadioMyAccessory = FIND_CONTROL_RADIOBUTTON("RadioMyAccessory");

	//UniformInfoTab::MyCharInfo
	m_cImageCharLeftTab = FIND_CONTROL_IMAGE("ImageMyCharBG");
	m_cImageMyCharBG = FIND_CONTROL_IMAGE("CharLeftTab");
	m_cButtonEquipCharLeft = FIND_CONTROL_BUTTON("ButtonEquipCharLeft");
	m_cButtonEquipCharRight = FIND_CONTROL_BUTTON("ButtonEquipCharRight");
	m_cButtonBL = FIND_CONTROL_RADIOBUTTON("ButtonBL");
	m_cButtonGR = FIND_CONTROL_RADIOBUTTON("ButtonGR");
	m_cImageMainChar = FIND_CONTROL_IMAGE("ImageMainChar");
	m_cButtonSetMainChar = FIND_CONTROL_BUTTON("ButtonSetMainChar");
	m_cButtonPriorChar = FIND_CONTROL_BUTTON("ButtonPriorChar");
	m_cButtonNextChar = FIND_CONTROL_BUTTON("ButtonNextChar");
	m_cButtonEquipSel[0] = FIND_CONTROL_BUTTON("ButtonEquipSel1");
	m_cButtonEquipSel[1] = FIND_CONTROL_BUTTON("ButtonEquipSel2");
	m_cButtonEquipSel[2] = FIND_CONTROL_BUTTON("ButtonEquipSel3");
	m_cButtonEquipSel[3] = FIND_CONTROL_BUTTON("ButtonEquipSel4");
	m_cButtonEquipSel[4] = FIND_CONTROL_BUTTON("ButtonEquipSel5");
	m_cButtonEquipSel[5] = FIND_CONTROL_BUTTON("ButtonEquipSel6");
	m_cCOMBO_Char = FIND_CONTROL_BUTTON("COMBO_Char");

	//UniformInfoTab::MyBagInfo
	m_cImageLeftWeaponTab = FIND_CONTROL_IMAGE("LeftWeaponTab");

	for (int i = 0; i < 3; i++)
	{
		sprintf(szOption, "RadioBag%d", i + 1);
		m_cRadioBag[i] = FIND_CONTROL_RADIOBUTTON(szOption);
	}

	for (int i = 0; i < 6; i++)
	{
		sprintf(szOption, "ImageBagItem%d", i + 1);
		m_cImageBagItem[i] = FIND_CONTROL_IMAGE(szOption);
		sprintf(szOption, "ButtonBagslot%d", i + 1);
		m_cButtonBagslot[i] = FIND_CONTROL_BUTTON(szOption);
		sprintf(szOption, "ButtonExtract%d", i + 1);
		m_cButtonExtract[i] = FIND_CONTROL_BUTTON(szOption);
	}

	for (int i = 0; i < 3; i++)
	{
		sprintf(szOption, "StaticBagItemName%d", i + 1);
		m_cStaticBagItemName[i] = FIND_CONTROL_STATIC(szOption);
	}

	for (int i = 0; i < 2; i++)
	{
		sprintf(szOption, "ButtonRepairSlot%d", i + 1);
		m_cButtonRepairSlot[i] = FIND_CONTROL_BUTTON(szOption);
		sprintf(szOption, "ImageWeaponGaugeBG%d", i + 1);
		m_cImageWeaponGaugeBG[i] = FIND_CONTROL_IMAGE(szOption);
		sprintf(szOption, "ImageWeaponGauge%d", i + 1);
		m_cImageWeaponGauge[i] = FIND_CONTROL_IMAGE(szOption);
	}

	m_cImageBagGrenadeX[0] = FIND_CONTROL_IMAGE("ImageBagGrenadeX2");
	m_cImageBagGrenadeX[1] = FIND_CONTROL_IMAGE("ImageBagGrenadeX3");

	//UniformInfoTab::AccessInfo
	m_cImageAccTab = FIND_CONTROL_IMAGE("AccTab");
	m_cImageMyAccBG = FIND_CONTROL_IMAGE("ImageMyAccBG");


	//ItemTab::Weapon
	m_cItemTab.AddCtrl(0, m_cButtonAll);
	m_cItemTab.AddCtrl(0, m_cButtonPrimary);
	m_cItemTab.AddCtrl(0, m_cButtonSecondary);
	m_cItemTab.AddCtrl(0, m_cButtonKnife);
	m_cItemTab.AddCtrl(0, m_cButtonHE);
	m_cItemTab.AddCtrl(0, m_cButtonSpecialized);
	m_cItemTab.AddCtrl(0, m_cButtonRepairAll);

	//ItemTab::Character
	m_cItemTab.AddCtrl(1, m_cRadioAll);
	m_cItemTab.AddCtrl(1, m_cRadioCharacter);
	m_cItemTab.AddCtrl(1, m_cRadioHead);
	m_cItemTab.AddCtrl(1, m_cRadioUpper);
	m_cItemTab.AddCtrl(1, m_cRadioLower);
	m_cItemTab.AddCtrl(1, m_cRadioAccessory);


	//UniformInfoTab::MyCharInfo
	m_cUniformInfoTab.AddCtrl(1, m_cImageCharLeftTab);
	m_cUniformInfoTab.AddCtrl(1, m_cImageMyCharBG);
	m_cUniformInfoTab.AddCtrl(1, m_cButtonEquipCharLeft);
	m_cUniformInfoTab.AddCtrl(1, m_cButtonEquipCharRight);
	m_cUniformInfoTab.AddCtrl(1, m_cButtonBL);
	m_cUniformInfoTab.AddCtrl(1, m_cButtonGR);
	m_cUniformInfoTab.AddCtrl(1, m_cImageMainChar);
	m_cUniformInfoTab.AddCtrl(1, m_cButtonSetMainChar);
	m_cUniformInfoTab.AddCtrl(1, m_cButtonPriorChar);
	m_cUniformInfoTab.AddCtrl(1, m_cButtonNextChar);
	m_cUniformInfoTab.AddCtrl(1, m_cButtonEquipSel[0]);
	m_cUniformInfoTab.AddCtrl(1, m_cButtonEquipSel[1]);
	m_cUniformInfoTab.AddCtrl(1, m_cButtonEquipSel[2]);
	m_cUniformInfoTab.AddCtrl(1, m_cButtonEquipSel[3]);
	m_cUniformInfoTab.AddCtrl(1, m_cButtonEquipSel[4]);
	m_cUniformInfoTab.AddCtrl(1, m_cButtonEquipSel[5]);
	m_cUniformInfoTab.AddCtrl(1, m_cCOMBO_Char);
	m_cUniformInfoTab.AddCtrl(1, pModelView);

	//UniformInfoTab::MyBagInfo
	m_cUniformInfoTab.AddCtrl(0, m_cImageLeftWeaponTab);

	for (int i = 0; i < 3; i++)
	{
		m_cUniformInfoTab.AddCtrl(0, m_cRadioBag[i]);
	}

	for (int i = 0; i < 6; i++)
	{
		m_cUniformInfoTab.AddCtrl(0, m_cImageBagItem[i]);
		m_cUniformInfoTab.AddCtrl(0, m_cButtonBagslot[i]);
		m_cUniformInfoTab.AddCtrl(0, m_cButtonExtract[i]);
	}

	for (int i = 0; i < 3; i++)
	{
		m_cUniformInfoTab.AddCtrl(0, m_cStaticBagItemName[i]);
	}

	for (int i = 0; i < 2; i++)
	{
		m_cUniformInfoTab.AddCtrl(0, m_cButtonRepairSlot[i]);
		m_cUniformInfoTab.AddCtrl(0, m_cImageWeaponGaugeBG[i]);
		m_cUniformInfoTab.AddCtrl(0, m_cImageWeaponGauge[i]);
	}

	m_cUniformInfoTab.AddCtrl(0, m_cImageBagGrenadeX[0]);
	m_cUniformInfoTab.AddCtrl(0, m_cImageBagGrenadeX[1]);

	//UniformInfoTab::AccessInfo
	m_cUniformInfoTab.AddCtrl(2, m_cImageAccTab);
	m_cUniformInfoTab.AddCtrl(2, m_cImageMyAccBG);

//
// Initailize button command, allocate KeyValues only once
//

	for (int i = 0; i < 10; i++)
	{
		m_cButtonSel[i]->SetCommand("SelectItem");
		m_cButtonRepair[i]->SetCommand("RepairItem");
		m_cButtonDelete[i]->SetCommand("DeleteItem");
		m_cButtonPeriodExtention[i]->SetCommand("PeriodExtention");
	}

	for (int i = 0; i < 6; i++)
	{
		m_cButtonBagslot[i]->SetCommand("BagItemExtract");
		m_cButtonExtract[i]->SetCommand("BagItemExtract");
	}

	for (int i = 0; i < 2; i++)
	{
		m_cButtonRepairSlot[i]->SetCommand("BagItemRepair");
	}

//
// Setup controls
//

	for (int i = 0; i < 6; i++)
	{
		m_cButtonEquipSel[i]->SetEnabled(false);
	}
}

void UIInven::Activate(void)
{
	BaseClass::Activate();

	int iCurClassEP = g_User.GetLevelEP(g_User.GetCurLevel());
	int iNextClassEP = g_User.GetLevelEP(g_User.GetNextLevel());
	int iCurrentEP = g_User.GetEP();

	int iGaugeWide = g_pVGuiSchemeManager->GetProportionalScaledValue((iCurrentEP - iCurClassEP) / (float)(iNextClassEP - iCurClassEP) * 100);

	m_cStaticWhoseInfo->SetText(g_User.GetName());
	m_cImageMyCurClass->SetImage("ui/class/class_%d.png", g_User.GetCurLevel());
	m_cImageMyNextClass->SetImage("ui/class/class_%d.png", g_User.GetNextLevel());
	m_cImageEPGauge->SetSize(iGaugeWide, 12);
	m_cStaticEP->SetText(L"%d/%d", iCurrentEP, iNextClassEP);
	m_cStaticGP->SetText(L"%d GP", g_User.GetGP());
	m_cStaticCP->SetText(L"%d CF点", g_User.GetCP());
	
	// If don't have BombPackage item, close the throw weapon slot
	m_bHasBombPackage = g_User.FindInvenItem(ITEMBOMBPACKAGEINDEX) != NULL;

	// default show tab MYBAG
	m_cRadioMyBag->SetSelected(true);

	// default show tab WEAPON
	m_cRadioWeapon->SetSelected(true);
}

void UIInven::OnCommandFull(KeyValues *params)
{
	const char *szCommand = params->GetString("command");

	if (!strcmp(szCommand, "GachaShop"))
	{
		return;
	}

	if (!strcmp(szCommand, "Inven"))
	{
		return;
	}

	if (!strcmp(szCommand, "Shop"))
	{
		return;
	}

	if (!strcmp(szCommand, "MPointMall"))
	{
		return;
	}

	if (!strcmp(szCommand, "Back"))
	{
		if (m_pOwnerPanel)
		{
			dynamic_cast<vgui::CFDialog*>(m_pOwnerPanel)->Activate();
			Close();
		}
		return;
	}

	if (!strcmp(szCommand, "InClan"))
	{
		return;
	}

	if (!strcmp(szCommand, "AutoMatchTry"))
	{
		return;
	}

	if (!strcmp(szCommand, "Buddy"))
	{
		return;
	}

	if (!strcmp(szCommand, "Option"))
	{
		gUI.GetOption()->DoModal();
		return;
	}

	if (!strcmp(szCommand, "Exit"))
	{
		gUI.GetGameEnd()->Activate(this);
		return;
	}

	if (!strcmp(szCommand, "ServerList"))
	{
		gUI.GetServerSelect()->Activate();
		Close();
		return;
	}

	if (!strcmp(szCommand, "SelectWeapon"))
	{
		CInvenItem *pItem = m_ItemBuffer[params->GetInt("Item")];

		switch (pItem->m_pShopItem->m_iItemCategory)
		{
			case ITEM_WEAPON_PRIMARY:
			{
				g_User.SetBagItem(m_iCurrentBag, BAG_SLOT_PRIMARY, pItem);
				break;
			}
			case ITEM_WEAPON_SECOND:
			{
				g_User.SetBagItem(m_iCurrentBag, BAG_SLOT_SECOND, pItem);
				break;
			}
			case ITEM_WEAPON_KNIFE:
			{
				g_User.SetBagItem(m_iCurrentBag, BAG_SLOT_KNIFE, pItem);
				break;
			}
			case ITEM_WEAPON_GREANDE:
			{
				if (m_bHasBombPackage)
				{
					for (int iSlot = BAG_SLOT_GRENADE1; iSlot <= BAG_SLOT_GRENADE3; iSlot++)
					{
						CInvenItem *pInside = g_User.GetBagItem(m_iCurrentBag, iSlot);

						//FIXME:
						if ( (pInside == NULL) || (pInside->m_pShopItem->m_iItemCategory2 == pItem->m_pShopItem->m_iItemCategory2) )
						{
							g_User.SetBagItem(m_iCurrentBag, iSlot, pItem);
							break;
						}
					}
				}
				else
				{
					g_User.SetBagItem(m_iCurrentBag, BAG_SLOT_GRENADE1, pItem);
				}
			}
		}

		OnScrollBarMove( m_cScrollBarInven->GetValue() );
		RefBagTab( m_iCurrentBag );

		return;
	}

	if (!strcmp(szCommand, "BagItemExtract"))
	{
		int iBagNumber = params->GetInt("BagNumber");
		int iSlot = params->GetInt("Slot");

		CInvenItem *pItem = g_User.GetBagItem(iBagNumber, iSlot);

		if ( !pItem )
			return;

		if (pItem->m_iShopItemIndex == ITEMKNIFEINDEX)
			// MsgBox("Can not remove base weapon knife");
			return;

		g_User.RemoveBagItem(pItem);

		OnScrollBarMove( m_cScrollBarInven->GetValue() );
		RefBagTab( m_iCurrentBag );
	}
}

void UIInven::OnRadioButtonCheck(vgui::Panel *panel, int tabposition)
{

//
// LEFT TAB RADIOBUTTON
//

	if (panel == m_cRadioMyBag)
	{
		m_cUniformInfoTab.Switch(0);

		if (m_bHasBombPackage)
		{
			m_cImageBagGrenadeX[0]->SetVisible(false);
			m_cImageBagGrenadeX[1]->SetVisible(false);
		}
		else
		{
			m_cImageBagGrenadeX[0]->SetVisible(true);
			m_cImageBagGrenadeX[1]->SetVisible(true);
		}

		m_cRadioBag[0]->SetSelected(true);

		return;
	}

	if (panel == m_cRadioMyChar)
	{
		m_cUniformInfoTab.Switch(1);
		return;
	}

	if (panel == m_cRadioMyAccessory)
	{
		m_cUniformInfoTab.Switch(2);
		return;
	}

//
// BAG NUMBER RADIOBUTTON
//
	
	if (panel == m_cRadioBag[0])
	{
		RefBagTab(1);
		m_iCurrentBag = 1;
		return;
	}

	if (panel == m_cRadioBag[1])
	{
		RefBagTab(2);
		m_iCurrentBag = 2;
		return;
	}

	if (panel == m_cRadioBag[2])
	{
		RefBagTab(3);
		m_iCurrentBag = 3;
		return;
	}

//
// ITEM LIST RADIOBUTTON
//

	if (panel == m_cRadioWeapon)
	{
		TabWeapon();
		return;
	}

	if (panel == m_cRadioChar)
	{
		TabChar();
		return;
	}

	if (panel == m_cRadioItem)
	{
		TabItem();
		return;
	}

	if (panel == m_cRadioPresent)
	{

		TabPresent();
		return;
	}

	if (panel == m_cRadioBuyListSet)
	{
		TabBuyList();
		return;
	}

	if (panel == m_cRadioAIItemTab)
	{
		TabAIItem();
		return;
	}

//
// TAB WEAPON
//

	if (panel == m_cButtonAll)
	{
		RefWeaponList(ITEM_WEAPON_ALL, 0);
		return;
	}
	if (panel == m_cButtonPrimary)
	{
		RefWeaponList(ITEM_WEAPON_PRIMARY, 0);
		return;
	}
	if (panel == m_cButtonSecondary)
	{
		RefWeaponList(ITEM_WEAPON_SECOND, 0);
		return;
	}
	if (panel == m_cButtonKnife)
	{
		RefWeaponList(ITEM_WEAPON_KNIFE, 0);
		return;
	}
	if (panel == m_cButtonHE)
	{
		RefWeaponList(ITEM_WEAPON_GREANDE, 0);
		return;
	}
	if (panel == m_cButtonSpecialized)
	{
		RefWeaponList(ITEM_WEAPON_SPECIAL, 0);
		return;
	}

//
// TAB CHAR
//

	if (panel == m_cRadioAll)
	{
		RefCharList(ITEM_CHARACTER_ALL);
		return;
	}

	if (panel == m_cRadioCharacter)
	{
		RefCharList(ITEM_CHARACTER_CHAR);
		return;
	}

	if (panel == m_cRadioHead)
	{
		RefCharList(ITEM_CHARACTER_HEAD);
		return;
	}

	if (panel == m_cRadioUpper)
	{
		RefCharList(ITEM_CHARACTER_UPPER);
		return;
	}

	if (panel == m_cRadioLower)
	{
		RefCharList(ITEM_CHARACTER_LOWER);
		return;
	}

	if (panel == m_cRadioAccessory)
	{
		RefCharList(ITEM_CHARACTER_ACCES);
		return;
	}
}

void UIInven::TabWeapon(void)
{
	m_cStaticCaption->SetText(L"个人仓库 - 武器");
	m_cImageWeaponTab->SetImage("UI/UI_Character_Setting/WeaponTabIn.png");

	m_cItemTab.Switch(0);
	m_cButtonAll->SetSelected(true);
	m_cRadioMyBag->SetSelected(true);
}

void UIInven::TabChar(void)
{
	m_cStaticCaption->SetText(L"个人仓库 - 角色");
	m_cImageWeaponTab->SetImage("UI/UI_Character_Uniform/CharTabIn.png");

	m_cItemTab.Switch(1);
	m_cRadioAll->SetSelected(true);
	m_cRadioMyChar->SetSelected(true);
}

void UIInven::TabItem(void)
{
	m_cStaticCaption->SetText(L"个人仓库 - 道具");
	m_cImageWeaponTab->SetImage("UI/UI_Character_Setting/ItemTabIn.png");

	m_cItemTab.Switch(2);
	RefItemList();
	m_cRadioMyChar->SetSelected(true);
}

void UIInven::TabPresent(void)
{
	m_cStaticCaption->SetText(L"个人仓库 - 礼物盒");
	m_cImageWeaponTab->SetImage("UI/UI_Character_Present/PresentTabIn.png");

	m_cItemTab.Switch(3);
	RefPresentList();
	m_cRadioMyChar->SetSelected(true);
}

void UIInven::TabBuyList(void)
{
	m_cStaticCaption->SetText(L"个人仓库 - 自定义喷图");
	m_cImageWeaponTab->SetImage("UI/UI_BuyListSet/BuyListTabIn.png");

	m_cItemTab.Switch(4);
	RefBuyListList();
	m_cRadioMyChar->SetSelected(true);
}

void UIInven::TabAIItem(void)
{
	m_cStaticCaption->SetText(L"个人仓库 - 挑战道具卡");
	m_cImageWeaponTab->SetImage("UI/UI_Character_Setting/ChallengeMode.png");

	m_cItemTab.Switch(5);
	RefAIItemList();
	m_cRadioMyChar->SetSelected(true);
}

void UIInven::TabMyBag(void)
{
	m_cUniformInfoTab.Switch(0);
}

void UIInven::TabMyChar(void)
{
	m_cUniformInfoTab.Switch(1);
}

void UIInven::TabMyAcce(void)
{
	m_cUniformInfoTab.Switch(2);
}

void UIInven::RefBagTab(int iBagNum)
{
	// not in MyBag tab, don't draw it
	if (m_cUniformInfoTab.GetIndex() != 0)
		return;

	for (int iSlot = 0; iSlot < 6; iSlot++)
	{
		// return null if no weapon in this slot
		CInvenItem *pItem = g_User.GetBagItem(iBagNum, iSlot);

		// set item button target
		m_cButtonBagslot[iSlot]->GetCommand()->SetInt("BagNumber", iBagNum);
		m_cButtonBagslot[iSlot]->GetCommand()->SetInt("Slot", iSlot);
		// icon button always visible
		m_cButtonBagslot[iSlot]->SetVisible(true);

		if (pItem)
		{
			if (iSlot < 3)
			{
				// big icon for ( primary second knife )
				m_cImageBagItem[iSlot]->SetImage("UI/ItemIcon/ItemIcon_%03d.PNG", (int)pItem->m_iShopItemIndex);
				m_cImageBagItem[iSlot]->SetVisible(true);
			}
			else
			{
				// small icon for greande
				m_cImageBagItem[iSlot]->SetImage("UI/ItemIcon/ItemIcon_%03ds.PNG", (int)pItem->m_iShopItemIndex);
				m_cImageBagItem[iSlot]->SetVisible(true);
			}

			// set extract target
			if (pItem->m_iShopItemIndex != ITEMKNIFEINDEX)
			{
				m_cButtonExtract[iSlot]->GetCommand()->SetInt("BagNumber", iBagNum);
				m_cButtonExtract[iSlot]->GetCommand()->SetInt("Slot", iSlot);
				m_cButtonExtract[iSlot]->SetVisible(true);
			}
			else
			{
				// don't show extract button if handling knife
				m_cButtonExtract[iSlot]->SetVisible(false);
			}

			// weapon name for ( primary second knife )
			if (iSlot < 3)
			{
				m_cStaticBagItemName[iSlot]->SetText(pItem->m_pShopItem->m_szName);
				m_cStaticBagItemName[iSlot]->SetVisible(true);
			}

			// repair info for ( primary second )
			if (iSlot < 2)
			{
				if (pItem->m_pShopItem->m_bRepairItem)
				{
					int iWide = pItem->m_iDurability / 100.0 * PROPORT( 88 );

					m_cButtonRepairSlot[iSlot]->SetVisible(true);
					m_cImageWeaponGaugeBG[iSlot]->SetVisible(true);
					m_cImageWeaponGauge[iSlot]->SetWide(iWide);
					m_cImageWeaponGauge[iSlot]->SetVisible(true);
				}
				else
				{
					m_cButtonRepairSlot[iSlot]->SetVisible(false);
					m_cImageWeaponGaugeBG[iSlot]->SetVisible(false);
					m_cImageWeaponGauge[iSlot]->SetVisible(false);
				}
			}
		}
		else
		{
			// not weapon in slot, hide all controls

			m_cImageBagItem[iSlot]->SetVisible(false);
			m_cButtonExtract[iSlot]->SetVisible(false);

			if (iSlot < 3)
			{
				m_cStaticBagItemName[iSlot]->SetVisible(false);
			}

			if (iSlot < 2)
			{
				m_cButtonRepairSlot[iSlot]->SetVisible(false);
				m_cImageWeaponGaugeBG[iSlot]->SetVisible(false);
				m_cImageWeaponGauge[iSlot]->SetVisible(false);
			}
		}
	}
}

static int _PtWeaponCompare(const void *_Arg1, const void *_Arg2)
{
	CInvenItem *pItem1 = *(CInvenItem **)_Arg1;
	CInvenItem *pItem2 = *(CInvenItem **)_Arg2;

	if (pItem1->m_pShopItem->m_iItemCategory > pItem2->m_pShopItem->m_iItemCategory)
		return 1;
	else if (pItem1->m_pShopItem->m_iItemCategory < pItem2->m_pShopItem->m_iItemCategory)
		return -1;
	else
		return 0;
}

void UIInven::RefWeaponList(int iCategory, int iCategory2)
{
	m_ItemBuffer.RemoveAll();

	int iItemCount = g_User.GetInvenItemCount();

	for (int nItem = 0; nItem < iItemCount; nItem++)
	{
		CInvenItem *pInvenItem = g_User.GetInvenItem(nItem);

		if (pInvenItem->m_pShopItem->m_iItemType != ITEM_WEAPON)
			continue;

		if (pInvenItem->m_pShopItem->m_iItemCategory != iCategory && iCategory != ITEM_WEAPON_ALL)
			continue;

		// don't contain knife
		if (pInvenItem->m_iShopItemIndex == ITEMKNIFEINDEX)
			continue;

		//if (pShopItem->m_iItemCategory2 != ITEM_WEAPON_RIFLE)

		m_ItemBuffer.AddToTail(pInvenItem);
	}

	// Sort item ( primary > second > knife > grenade )
	qsort(m_ItemBuffer.Base(), m_ItemBuffer.Count(), sizeof(CInvenItem *), &_PtWeaponCompare);

	m_pfnOnScrollBarMove = static_cast<void (UIInven::*)(int)>(&UIInven::OnScrollBarMove_Weapon);

	int iCount = m_ItemBuffer.Count();

	if (iCount > 10)
	{
		int iStep = ceil((iCount - 10) / 2.0);

		m_cScrollBarInven->SetEnabled(true);
		m_cScrollBarInven->SetValue(0);
		m_cScrollBarInven->SetRange(0, iStep);
	}
	else
	{
		m_cScrollBarInven->SetEnabled(false);
		m_cScrollBarInven->SetValue(0);
		m_cScrollBarInven->SetRange(0, 1);
	}

	OnScrollBarMove(0);
}

void UIInven::RefCharList(int iCategory)
{
	m_ItemBuffer.RemoveAll();

	int iItemCount = g_User.GetInvenItemCount();

	for (int nItem = 0; nItem < iItemCount; nItem++)
	{
		CInvenItem *pInvenItem = g_User.GetInvenItem(nItem);

		if (pInvenItem->m_pShopItem->m_iItemType != ITEM_CHAR)
			continue;

		if (pInvenItem->m_pShopItem->m_iItemCategory != iCategory && iCategory != ITEM_CHARACTER_ALL)
			continue;

		m_ItemBuffer.AddToTail(pInvenItem);
	}

	m_pfnOnScrollBarMove = static_cast<void (UIInven::*)(int)>(&UIInven::OnScrollBarMove_Char);

	int iCount = m_ItemBuffer.Count();

	if (iCount > 10)
	{
		int iStep = ceil((iCount - 10) / 2.0);

		m_cScrollBarInven->SetEnabled(true);
		m_cScrollBarInven->SetValue(0);
		m_cScrollBarInven->SetRange(0, iStep);
	}
	else
	{
		m_cScrollBarInven->SetEnabled(false);
		m_cScrollBarInven->SetValue(0);
		m_cScrollBarInven->SetRange(0, 1);
	}

	//FIXME: don't call twice
	OnScrollBarMove(0);
}

void UIInven::RefItemList(void)
{
	m_ItemBuffer.RemoveAll();

	int iItemCount = g_User.GetInvenItemCount();

	for (int nItem = 0; nItem < iItemCount; nItem++)
	{
		CInvenItem *pInvenItem = g_User.GetInvenItem(nItem);

		if (pInvenItem->m_pShopItem->m_iItemType != ITEM_ITEM)
			continue;

		m_ItemBuffer.AddToTail(pInvenItem);
	}

	m_pfnOnScrollBarMove = static_cast<void (UIInven::*)(int)>(&UIInven::OnScrollBarMove_Item);

	int iCount = m_ItemBuffer.Count();

	if (iCount > 10)
	{
		int iStep = ceil((iCount - 10) / 2.0);

		m_cScrollBarInven->SetEnabled(true);
		m_cScrollBarInven->SetValue(0);
		m_cScrollBarInven->SetRange(0, iStep);
	}
	else
	{
		m_cScrollBarInven->SetEnabled(false);
		m_cScrollBarInven->SetValue(0);
		m_cScrollBarInven->SetRange(0, 1);
	}

	OnScrollBarMove(0);
}

void UIInven::RefPresentList(void)
{
	for (int i = 0; i < 10; i++)
	{
		m_cImageNosel[i]->SetVisible(false);
		m_cImageItem[i]->SetVisible(false);
		m_cImageBagnumber[i]->SetVisible(false);
		m_cImageSelup[i]->SetVisible(false);
		m_cButtonSel[i]->SetVisible(false);
		m_cStaticWeaponname[i]->SetVisible(false);
		m_cImageDurabilityBack[i]->SetVisible(false);
		m_cImageDurability[i]->SetVisible(false);
		m_cStaticExpire[i]->SetVisible(false);
		m_cButtonRepair[i]->SetVisible(false);
		m_cButtonDelete[i]->SetVisible(false);
		m_cButtonPeriodExtention[i]->SetVisible(false);
	}

	m_cScrollBarInven->SetEnabled(false);
	m_pfnOnScrollBarMove = NULL;
}

void UIInven::RefBuyListList(void)
{
	for (int i = 0; i < 10; i++)
	{
		m_cImageNosel[i]->SetVisible(false);
		m_cImageItem[i]->SetVisible(false);
		m_cImageBagnumber[i]->SetVisible(false);
		m_cImageSelup[i]->SetVisible(false);
		m_cButtonSel[i]->SetVisible(false);
		m_cStaticWeaponname[i]->SetVisible(false);
		m_cImageDurabilityBack[i]->SetVisible(false);
		m_cImageDurability[i]->SetVisible(false);
		m_cStaticExpire[i]->SetVisible(false);
		m_cButtonRepair[i]->SetVisible(false);
		m_cButtonDelete[i]->SetVisible(false);
		m_cButtonPeriodExtention[i]->SetVisible(false);
	}

	m_cScrollBarInven->SetEnabled(false);
	m_pfnOnScrollBarMove = NULL;
}

void UIInven::RefAIItemList(void)
{
	for (int i = 0; i < 10; i++)
	{
		m_cImageNosel[i]->SetVisible(false);
		m_cImageItem[i]->SetVisible(false);
		m_cImageBagnumber[i]->SetVisible(false);
		m_cImageSelup[i]->SetVisible(false);
		m_cButtonSel[i]->SetVisible(false);
		m_cStaticWeaponname[i]->SetVisible(false);
		m_cImageDurabilityBack[i]->SetVisible(false);
		m_cImageDurability[i]->SetVisible(false);
		m_cStaticExpire[i]->SetVisible(false);
		m_cButtonRepair[i]->SetVisible(false);
		m_cButtonDelete[i]->SetVisible(false);
		m_cButtonPeriodExtention[i]->SetVisible(false);
	}

	m_cScrollBarInven->SetEnabled(false);
	m_pfnOnScrollBarMove = NULL;
}

void UIInven::OnScrollBarMove(int value)
{
	if (m_pfnOnScrollBarMove)
	{
		(this->*m_pfnOnScrollBarMove)(value);
	}
}

void UIInven::OnScrollBarMove_Weapon(int iValue)
{
	int nIndex = iValue * 2;	// 2 rows
	int iCount = m_ItemBuffer.Count();

	for (int i = 0; i < 10; i++)
	{
		if (nIndex < iCount)
		{
			CInvenItem *pInvenItem = m_ItemBuffer[nIndex];
			CShopItem *pShopItem = pInvenItem->m_pShopItem;

			if (pShopItem->m_bCashItem)
			{
				m_cImageNosel[i]->SetImage("UI/UI_Character_Uniform/UniBox1.PNG");
				m_cImageNosel[i]->SetVisible(true);
			}
			else
			{
				m_cImageNosel[i]->SetImage("UI/UI_Character_Setting/Box1.PNG");
				m_cImageNosel[i]->SetVisible(true);
			}

			m_cImageItem[i]->SetImage("UI/ItemIcon/ItemIcon_%03d.PNG", (int)pShopItem->m_nItemIndex);
			m_cImageItem[i]->SetVisible(true);

			int iBagNum = g_User.FindInvenItemInBag(pInvenItem);

			if (iBagNum != 0)
			{
				m_cImageBagnumber[i]->SetImage("UI/UI_Character_Setting/BagNum%d.PNG", iBagNum);
				m_cImageBagnumber[i]->SetVisible(true);
				m_cImageSelup[i]->SetVisible(true);
			}
			else
			{
				m_cImageBagnumber[i]->SetVisible(false);
				m_cImageSelup[i]->SetVisible(false);
			}

			m_cButtonSel[i]->GetCommand()->SetString("command", "SelectWeapon");
			m_cButtonSel[i]->GetCommand()->SetInt("Item", nIndex);
			m_cButtonSel[i]->SetVisible(true);

			m_cStaticWeaponname[i]->SetText(pShopItem->m_szName);
			m_cStaticWeaponname[i]->SetContentAlignment(vgui::Label::a_west);
			m_cStaticWeaponname[i]->SetVisible(true);

			if (pShopItem->m_bRepairItem)
			{
				int iWide = pInvenItem->m_iDurability / 100.0 * PROPORT( 88 );

				m_cImageDurabilityBack[i]->SetVisible(true);
				m_cImageDurability[i]->SetWide(iWide);
				m_cImageDurability[i]->SetVisible(true);

				m_cButtonRepair[i]->GetCommand()->SetInt("Item", nIndex);
				m_cButtonRepair[i]->SetVisible(true);
			}
			else
			{
				m_cImageDurabilityBack[i]->SetVisible(false);
				m_cImageDurability[i]->SetVisible(false);
				m_cButtonRepair[i]->SetVisible(false);
			}

			if (pShopItem->m_bDeleteItem)
			{
				m_cButtonDelete[i]->GetCommand()->SetInt("Item", nIndex);
				m_cButtonDelete[i]->SetVisible(true);
			}
			else
			{
				m_cButtonDelete[i]->SetVisible(false);
			}

			if (pInvenItem->m_iPeriodTime > 0)
			{
				m_cStaticExpire[i]->SetText(L"7天 23小时");
				m_cStaticExpire[i]->SetVisible(true);

				m_cButtonPeriodExtention[i]->GetCommand()->SetInt("Item", nIndex);
				m_cButtonPeriodExtention[i]->SetVisible(true);
			}
			else
			{
				m_cStaticExpire[i]->SetVisible(false);
				m_cButtonPeriodExtention[i]->SetVisible(false);
			}
		}
		else
		{
			m_cImageNosel[i]->SetVisible(false);
			m_cImageItem[i]->SetVisible(false);
			m_cImageBagnumber[i]->SetVisible(false);
			m_cImageSelup[i]->SetVisible(false);
			m_cButtonSel[i]->SetVisible(false);
			m_cStaticWeaponname[i]->SetVisible(false);
			m_cImageDurabilityBack[i]->SetVisible(false);
			m_cImageDurability[i]->SetVisible(false);
			m_cStaticExpire[i]->SetVisible(false);
			m_cButtonRepair[i]->SetVisible(false);
			m_cButtonDelete[i]->SetVisible(false);
			m_cButtonPeriodExtention[i]->SetVisible(false);
		}

		nIndex++;
	}
}

void UIInven::OnScrollBarMove_Char(int iValue)
{
	int nIndex = iValue * 2;	// 2 rows
	int iCount = m_ItemBuffer.Count();

	for (int i = 0; i < 10; i++)
	{
		if (nIndex < iCount)
		{
			CInvenItem *pInvenItem = m_ItemBuffer[nIndex];
			CShopItem *pShopItem = pInvenItem->m_pShopItem;

			m_cImageNosel[i]->SetImage("UI/UI_Character_Setting/Box1.PNG");
			m_cImageNosel[i]->SetVisible(true);

			m_cImageItem[i]->SetImage("UI/ItemIcon/ItemIcon_%03d.PNG", (int)pShopItem->m_nItemIndex);
			m_cImageItem[i]->SetVisible(true);

			m_cImageBagnumber[i]->SetVisible(false);
			m_cImageSelup[i]->SetVisible(false);

			m_cButtonSel[i]->GetCommand()->SetString("command", "SelectChar");
			m_cButtonSel[i]->GetCommand()->SetInt("Item", nIndex);
			m_cButtonSel[i]->SetVisible(true);

			m_cStaticWeaponname[i]->SetText(pShopItem->m_szName);
			m_cStaticWeaponname[i]->SetContentAlignment(vgui::Label::a_center);
			m_cStaticWeaponname[i]->SetVisible(true);

			m_cImageDurabilityBack[i]->SetVisible(false);
			m_cImageDurability[i]->SetVisible(false);
			m_cStaticExpire[i]->SetVisible(false);
			m_cButtonRepair[i]->SetVisible(false);
			m_cButtonDelete[i]->SetVisible(false);
			m_cButtonPeriodExtention[i]->SetVisible(false);
		}
		else
		{
			m_cImageNosel[i]->SetVisible(false);
			m_cImageItem[i]->SetVisible(false);
			m_cImageBagnumber[i]->SetVisible(false);
			m_cImageSelup[i]->SetVisible(false);
			m_cButtonSel[i]->SetVisible(false);
			m_cStaticWeaponname[i]->SetVisible(false);
			m_cImageDurabilityBack[i]->SetVisible(false);
			m_cImageDurability[i]->SetVisible(false);
			m_cStaticExpire[i]->SetVisible(false);
			m_cButtonRepair[i]->SetVisible(false);
			m_cButtonDelete[i]->SetVisible(false);
			m_cButtonPeriodExtention[i]->SetVisible(false);
		}

		nIndex++;
	}
}

void UIInven::OnScrollBarMove_Item(int iValue)
{
	int nIndex = iValue * 2;	// 2 rows
	int iCount = m_ItemBuffer.Count();

	for (int i = 0; i < 10; i++)
	{
		if (nIndex < iCount)
		{
			CInvenItem *pInvenItem = m_ItemBuffer[nIndex];
			CShopItem *pShopItem = pInvenItem->m_pShopItem;

			if (pShopItem->m_bCashItem)
			{
				m_cImageNosel[i]->SetImage("UI/UI_Character_Uniform/UniBox1.PNG");
				m_cImageNosel[i]->SetVisible(true);
			}
			else
			{
				m_cImageNosel[i]->SetImage("UI/UI_Character_Setting/Box1.PNG");
				m_cImageNosel[i]->SetVisible(true);
			}

			m_cImageItem[i]->SetImage("UI/ItemIcon/ItemIcon_%03d.PNG", (int)pShopItem->m_nItemIndex);
			m_cImageItem[i]->SetVisible(true);

			m_cImageBagnumber[i]->SetVisible(false);
			m_cImageSelup[i]->SetVisible(false);

			m_cButtonSel[i]->GetCommand()->SetString("command", "SelectItem");
			m_cButtonSel[i]->GetCommand()->SetInt("Item", nIndex);
			m_cButtonSel[i]->SetVisible(true);

			m_cStaticWeaponname[i]->SetText(pShopItem->m_szName);
			m_cStaticWeaponname[i]->SetContentAlignment(vgui::Label::a_center);
			m_cStaticWeaponname[i]->SetVisible(true);

			m_cImageDurabilityBack[i]->SetVisible(false);
			m_cImageDurability[i]->SetVisible(false);
			m_cButtonRepair[i]->SetVisible(false);
			m_cButtonDelete[i]->SetVisible(false);

			if (pInvenItem->m_iPeriodTime > 0)
			{
				m_cStaticExpire[i]->SetText(L"7天 23小时");
				m_cStaticExpire[i]->SetVisible(true);

				m_cButtonPeriodExtention[i]->GetCommand()->SetInt("PeriodExtentionItem", nIndex);
				m_cButtonPeriodExtention[i]->SetVisible(true);
			}
			else
			{
				m_cStaticExpire[i]->SetVisible(false);
				m_cButtonPeriodExtention[i]->SetVisible(false);
			}
		}
		else
		{
			m_cImageNosel[i]->SetVisible(false);
			m_cImageItem[i]->SetVisible(false);
			m_cImageBagnumber[i]->SetVisible(false);
			m_cImageSelup[i]->SetVisible(false);
			m_cButtonSel[i]->SetVisible(false);
			m_cStaticWeaponname[i]->SetVisible(false);
			m_cImageDurabilityBack[i]->SetVisible(false);
			m_cImageDurability[i]->SetVisible(false);
			m_cStaticExpire[i]->SetVisible(false);
			m_cButtonRepair[i]->SetVisible(false);
			m_cButtonDelete[i]->SetVisible(false);
			m_cButtonPeriodExtention[i]->SetVisible(false);
		}

		nIndex++;
	}
}

void UIInven::OnScrollBarMove_Present(int value)
{
}

void UIInven::OnScrollBarMove_BuyList(int value)
{
}

void UIInven::OnScrollBarMove_AIItem(int value)
{
}

void UIInven::OnMouseWheeled(int delta)
{
	if (!m_pfnOnScrollBarMove)
		return;

	if (!m_cScrollBarInven->IsEnabled())
		return;

	int x, y;

	vgui::input()->GetCursorPosition(x, y);
	ScreenToLocal(x, y);

	if (x < PROPORT(270) || x > PROPORT(770) || y < PROPORT(184) || y > PROPORT(584))
		return;

	if (delta > 0)
	{
		m_cScrollBarInven->SetValue(m_cScrollBarInven->GetValue() - 1);
	}

	if (delta < 0)
	{
		m_cScrollBarInven->SetValue(m_cScrollBarInven->GetValue() + 1);
	}

	BaseClass::OnMouseWheeled(delta);
}

void UIInven::OnKeyCodePressed(vgui::KeyCode code)
{
	if (code == vgui::KEY_ESCAPE)
	{
		gUI.GetGameEnd()->Activate(this);
		return;
	}

	BaseClass::OnKeyCodePressed(code);
}