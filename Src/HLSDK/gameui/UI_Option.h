#ifndef UI_OPTION_H
#define UI_OPTION_H

#include "CF_Dialog.h"
#include "CF_Tab.h"

#include "Setting.h"

namespace vgui
{
	class CFStatic;
	class CFButton;
	class CFRadioButton;
	class CFImage;
	class CFSlider;
	class CFEdit;
	class CFScrollBar;
	class CFKeyEdit;
}

class UIOption : public vgui::CFDialog
{
	DECLARE_CLASS_SIMPLE(UIOption, vgui::CFDialog);

public:
	UIOption(vgui::Panel *parent);

	virtual void Activate(void);

protected:
	MESSAGE_FUNC_PARAMS(OnCommandFull, "Command", params);
	MESSAGE_FUNC_PTR_INT(OnRadioButtonCheck, "RadioButtonChecked", panel, tabposition);
	MESSAGE_FUNC_PARAMS(OnSliderMove, "SliderMoved", params);
	MESSAGE_FUNC_PTR(OnTextChange, "TextChanged", panel);
	MESSAGE_FUNC_PTR(OnKeyCapture, "KeyCaptured", panel);

private:
	void LoadAllSetting(void);
	void SaveAllSetting(void);

	void MakeDefaultSetting(void);

	void RecoverGraphicSetting(void);
	void RecoverControlSetting(void);
	void RecoverSoundSetting(void);
	void RecoverCommunitySetting(void);
	void RecoverEtcSetting(void);

	void DefaultGraphicSetting(void);
	void DefaultControlSetting(void);
	void DefaultSoundSetting(void);
	void DefaultCommunitySetting(void);
	void DefaultEtcSetting(void);

	void SettingToGraphicTab(void);
	void SettingToControlTab(void);
	void SettingToSoundTab(void);
	void SettingToCommunityTab(void);
	void SettingToEtcTab(void);

	SETTING m_Setting;
	SETTING	m_DefaultSetting;
	SETTING	m_CurrentSetting;

private:
	// TopTab
	void TabGraphic(void);
	void TabControl(void);
	void TabSound(void);
	void TabCommunity(void);
	void TabCombat(void);
	void TabEtc(void);
	void TabModeSetting(void);

	// CommunityTab
	void TabChatting(void);
	void TabFunctionControl(void);

	vgui::CFTab				m_cGameOptionSettingTab;
	vgui::CFTab				m_cOptionCommunityTab;

	vgui::CFButton			*m_cButtonRestore;
	vgui::CFButton			*m_cButtonBack;
	vgui::CFButton			*m_cButtonOk;
	vgui::CFButton			*m_cButtonCancel;
	//tab bgbox
	vgui::CFImage			*m_cImageCombatBG;
	//tab buttons
	vgui::CFRadioButton		*m_cButtonGraphic;
	vgui::CFRadioButton		*m_cButtonControl;
	vgui::CFRadioButton		*m_cButtonSound;
	vgui::CFRadioButton		*m_cButtonCommunity;
	vgui::CFRadioButton		*m_cButtonCombat;
	vgui::CFRadioButton		*m_cButtonEtc;
	vgui::CFRadioButton		*m_cButtonModeSetting;
	//graphic
	vgui::CFRadioButton		*m_cRadioAutoMatic;
	vgui::CFRadioButton		*m_cRadioGraphicCustom;
	vgui::CFImage			*m_cImageAutoControl;
//	vgui::CFButton			*m_cButtonAutoGaugePoint;
	vgui::CFSlider			*m_cButtonAutoGaugePoint;
	vgui::CFRadioButton		*m_cRadio16Bit;
	vgui::CFRadioButton		*m_cRadio32Bit;
	vgui::CFRadioButton		*m_cRadioMapHigh;
	vgui::CFRadioButton		*m_cRadioMapLow;
	vgui::CFRadioButton		*m_cRadioBulletMarkHigh;
	vgui::CFRadioButton		*m_cRadioBulletMarkMid;
	vgui::CFRadioButton		*m_cRadioBulletMarkLow;
	vgui::CFRadioButton		*m_cRadioBulletSmokeHigh;
	vgui::CFRadioButton		*m_cRadioBulletSmokeMid;
	vgui::CFRadioButton		*m_cRadioBulletSmokeLow;
	vgui::CFRadioButton		*m_cRadioBloodOn;
	vgui::CFRadioButton		*m_cRadioBloodOff;
	vgui::CFRadioButton		*m_cRadioBulletOn;
	vgui::CFRadioButton		*m_cRadioBulletOff;
	vgui::CFRadioButton		*m_cRadioEnviromentOn;
	vgui::CFRadioButton		*m_cRadioEnviromentOff;
	vgui::CFButton			*m_cButtonGammaDown;
	vgui::CFButton			*m_cButtonGammaUp;
	vgui::CFStatic			*m_cStaticBright;
//	vgui::CFButton			*m_cButtonGaugePoint;
	vgui::CFSlider			*m_cButtonGaugePoint;
	vgui::CFImage			*m_cImageGammaGauge;
	vgui::CFRadioButton		*m_cRadioResolutionFull;
	vgui::CFRadioButton		*m_cRadioResolutionWindow;
	vgui::CFStatic			*m_cStaticResolution;
	vgui::CFButton			*m_cComboResolutionDBtn;
	//control
	vgui::CFStatic			*m_cStaticMouse;
	vgui::CFStatic			*m_cStaticZoomMouse;
	vgui::CFKeyEdit			*m_cButtonSetControl[19];
	vgui::CFStatic			*m_cStaticControlEdit[20];
	vgui::CFSlider			*m_cButtonControlGaugePoint;
	vgui::CFButton			*m_cButtonSenseDown;
	vgui::CFButton			*m_cButtonSenseUp;
	vgui::CFImage			*m_cImageControlGauge;
	vgui::CFSlider			*m_cButtonZoomControlGaugePoint;
	vgui::CFButton			*m_cButtonZoomSenseDown;
	vgui::CFButton			*m_cButtonZoomSenseUp;
	vgui::CFImage			*m_cImageZoomControlGauge;
	//sound
	vgui::CFStatic			*m_cStaticVolume;
	vgui::CFStatic			*m_cStaticEffectVol;
	vgui::CFStatic			*m_cStaticRadio;
	vgui::CFButton			*m_cButtonBgmDown;
	vgui::CFButton			*m_cButtonBgmUp;
	vgui::CFButton			*m_cButtonSoundDown;
	vgui::CFButton			*m_cButtonSoundUp;
	vgui::CFButton			*m_cButtonRadioDown;
	vgui::CFButton			*m_cButtonRadioUp;
	vgui::CFSlider			*m_cButtonBGMGaugePoint;
	vgui::CFSlider			*m_cButtonSoundGaugePoint;
	vgui::CFSlider			*m_cButtonRadioGaugePoint;
	vgui::CFImage			*m_cImageBGMGauge;
	vgui::CFImage			*m_cImageSoundGauge;
	vgui::CFImage			*m_cImageRadioGauge;
	vgui::CFRadioButton		*m_cRadioBgmOn;
	vgui::CFRadioButton		*m_cRadioBgmOff;
	vgui::CFRadioButton		*m_cRadioSoundOn;
	vgui::CFRadioButton		*m_cRadioSoundOff;
	vgui::CFRadioButton		*m_cRadioRadioOn;
	vgui::CFRadioButton		*m_cRadioRadioOff;
	vgui::CFRadioButton		*m_cRadioKorean;
	vgui::CFRadioButton		*m_cRadioEnglish;
	// community
	vgui::CFRadioButton		*m_cButtonChattingTab;
	vgui::CFRadioButton		*m_cButtonFunctionControlTab;
	vgui::CFStatic			*m_cStaticOptionCommunityDefaultAction1[10];
	vgui::CFKeyEdit			*m_cButtonSetControl2[3];
	vgui::CFStatic			*m_cStaticControlEdit2[3];
	vgui::CFEdit			*m_cEditMacro[4];
	vgui::CFRadioButton		*m_cRadioRejInvOn;
	vgui::CFRadioButton		*m_cRadioRejInvOff;
	vgui::CFRadioButton		*m_cRadioRejWhisperOn;
	vgui::CFRadioButton		*m_cRadioRejWhisperOff;
	vgui::CFRadioButton		*m_cRadioBlockInviteFriendOn;
	vgui::CFRadioButton		*m_cRadioBlockInviteFriendOff;
	vgui::CFRadioButton		*m_cRadioShowAlterably;
	vgui::CFRadioButton		*m_cRadioShowBadge;
	vgui::CFRadioButton		*m_cRadioShowAchieve;
	vgui::CFRadioButton		*m_cRadioShowChatAchieve;
	vgui::CFRadioButton		*m_cRadioShowChatNormal;
	vgui::CFRadioButton		*m_cRadioShowRankMedal;
	vgui::CFRadioButton		*m_cRadioHideRankMedal;
	vgui::CFRadioButton		*m_cRadioShowClanRankMedal;
	vgui::CFRadioButton		*m_cRadioHideClanRankMedal;
	// etc
	vgui::CFRadioButton		*m_cRadioDefaultRightHand;
	vgui::CFRadioButton		*m_cRadioLeftHand;
	vgui::CFRadioButton		*m_cRadioDefaultMouseUpDown;
	vgui::CFRadioButton		*m_cRadioInverseMouseUpDown;
	vgui::CFRadioButton		*m_cRadioCrossColor[3];
	vgui::CFRadioButton		*m_cRadioCrossAType;
	vgui::CFRadioButton		*m_cRadioCrossBType;
	vgui::CFRadioButton		*m_cRadioCrossCType;
	vgui::CFRadioButton		*m_cRadioReplaySaveOn;
	vgui::CFRadioButton		*m_cRadioReplaySaveOff;
	vgui::CFImage			*m_cImageEtcCross;
	vgui::CFImage			*m_cImageSpray;
	vgui::CFButton			*m_cButtonSpray[6];
	vgui::CFStatic			*m_cStaticSpray[6];
	vgui::CFScrollBar		*m_cScrollBarSprayList;
	vgui::CFRadioButton		*m_cRadioLoadingScreenTypeA;
	vgui::CFRadioButton		*m_cRadioLoadingScreenTypeB;
};

#endif