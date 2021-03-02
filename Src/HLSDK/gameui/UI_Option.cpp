#include "UI_Option.h"
#include "CF_Base.h"
#include <keyvalues.h>

UIOption::UIOption(vgui::Panel *parent) : vgui::CFDialog(parent, "Option")
{
	SetProportional(true);

	LoadControlSettings("UI/Scripts/Option/Option_Base.txt");
	LoadControlSettings("UI/Scripts/Option/Option_CombatBase.txt");
	LoadControlSettings("UI/Scripts/Option/Option_GraphicBase.txt");
	LoadControlSettings("UI/Scripts/Option/Option_ResolutionCombo.txt");
	LoadControlSettings("UI/Scripts/Option/Option_ControlBase.txt");
	LoadControlSettings("UI/Scripts/Option/Option_SoundBase.txt");
	LoadControlSettings("UI/Scripts/Option/Option_CommunityTab.txt");
	LoadControlSettings("UI/Scripts/Option/Community_Chatting.txt");
	LoadControlSettings("UI/Scripts/Option/Community_FunctionControl.txt");
	LoadControlSettings("UI/Scripts/Option/Option_EtcBase.txt");

	// dialog control
	m_cButtonRestore = FIND_CONTROL_BUTTON("ButtonRestore");
	m_cButtonBack = FIND_CONTROL_BUTTON("ButtonBack");
	m_cButtonOk = FIND_CONTROL_BUTTON("ButtonOk");
	m_cButtonCancel = FIND_CONTROL_BUTTON("ButtonCancel");

	// tab|header
	m_cImageCombatBG = FIND_CONTROL_IMAGE("ImageCombatBG");
	m_cButtonGraphic = FIND_CONTROL_RADIOBUTTON("ButtonGraphic");
	m_cButtonControl = FIND_CONTROL_RADIOBUTTON("ButtonControl");
	m_cButtonSound = FIND_CONTROL_RADIOBUTTON("ButtonSound");
	m_cButtonCommunity = FIND_CONTROL_RADIOBUTTON("ButtonCommunity");
	m_cButtonCombat = FIND_CONTROL_RADIOBUTTON("ButtonCombat");
	m_cButtonEtc = FIND_CONTROL_RADIOBUTTON("ButtonEtc");
	m_cButtonModeSetting = FIND_CONTROL_RADIOBUTTON("ButtonModeSetting");

	// tab|graphic
	m_cRadioAutoMatic = FIND_CONTROL_RADIOBUTTON("RadioAutoMatic");
	m_cRadioGraphicCustom = FIND_CONTROL_RADIOBUTTON("RadioGraphicCustom");
	m_cImageAutoControl = FIND_CONTROL_IMAGE("ImageAutoControl");
	m_cButtonAutoGaugePoint = FIND_CONTROL_SLIDER("ButtonAutoGaugePoint");
	m_cRadio16Bit = FIND_CONTROL_RADIOBUTTON("Radio16Bit");
	m_cRadio32Bit = FIND_CONTROL_RADIOBUTTON("Radio32Bit");
	m_cRadioMapHigh = FIND_CONTROL_RADIOBUTTON("RadioMapHigh");
	m_cRadioMapLow = FIND_CONTROL_RADIOBUTTON("RadioMapLow");
	m_cRadioBulletMarkHigh = FIND_CONTROL_RADIOBUTTON("RadioBulletMarkHigh");
	m_cRadioBulletMarkMid = FIND_CONTROL_RADIOBUTTON("RadioBulletMarkMid");
	m_cRadioBulletMarkLow = FIND_CONTROL_RADIOBUTTON("RadioBulletMarkLow");
	m_cRadioBulletSmokeHigh = FIND_CONTROL_RADIOBUTTON("RadioBulletSmokeHigh");
	m_cRadioBulletSmokeMid = FIND_CONTROL_RADIOBUTTON("RadioBulletSmokeMid");
	m_cRadioBulletSmokeLow = FIND_CONTROL_RADIOBUTTON("RadioBulletSmokeLow");
	m_cRadioBloodOn = FIND_CONTROL_RADIOBUTTON("RadioBloodOn");
	m_cRadioBloodOff = FIND_CONTROL_RADIOBUTTON("RadioBloodOff");
	m_cRadioBulletOn = FIND_CONTROL_RADIOBUTTON("RadioBulletOn");
	m_cRadioBulletOff = FIND_CONTROL_RADIOBUTTON("RadioBulletOff");
	m_cRadioEnviromentOn = FIND_CONTROL_RADIOBUTTON("EnviromentOn");
	m_cRadioEnviromentOff = FIND_CONTROL_RADIOBUTTON("EnviromentOff");
	m_cButtonGammaDown = FIND_CONTROL_BUTTON("ButtonGammaDown");
	m_cButtonGammaUp = FIND_CONTROL_BUTTON("ButtonGammaUp");
	m_cStaticBright = FIND_CONTROL_STATIC("StaticBright");
	m_cButtonGaugePoint = FIND_CONTROL_SLIDER("ButtonGaugePoint");
	m_cImageGammaGauge = FIND_CONTROL_IMAGE("ImageGammaGauge");
	m_cRadioResolutionFull = FIND_CONTROL_RADIOBUTTON("RadioResolutionFull");
	m_cRadioResolutionWindow = FIND_CONTROL_RADIOBUTTON("RadioResolutionWindow");
	m_cStaticResolution = FIND_CONTROL_STATIC("ResolutionStatic");
	m_cComboResolutionDBtn = FIND_CONTROL_BUTTON("ComboResolutionDBtn");

	// tab|control
	m_cStaticMouse = FIND_CONTROL_STATIC("StaticMouse");
	m_cStaticZoomMouse = FIND_CONTROL_STATIC("StaticZoomMouse");
	m_cButtonSetControl[0] = FIND_CONTROL_KEYEDIT("ButtonSetControl1");
	m_cButtonSetControl[1] = FIND_CONTROL_KEYEDIT("ButtonSetControl2");
	m_cButtonSetControl[2] = FIND_CONTROL_KEYEDIT("ButtonSetControl3");
	m_cButtonSetControl[3] = FIND_CONTROL_KEYEDIT("ButtonSetControl4");
	m_cButtonSetControl[4] = FIND_CONTROL_KEYEDIT("ButtonSetControl5");
	m_cButtonSetControl[5] = FIND_CONTROL_KEYEDIT("ButtonSetControl6");
	m_cButtonSetControl[6] = FIND_CONTROL_KEYEDIT("ButtonSetControl7");
	m_cButtonSetControl[7] = FIND_CONTROL_KEYEDIT("ButtonSetControl8");
	m_cButtonSetControl[8] = FIND_CONTROL_KEYEDIT("ButtonSetControl9");
	m_cButtonSetControl[9] = FIND_CONTROL_KEYEDIT("ButtonSetControl10");
	m_cButtonSetControl[10] = FIND_CONTROL_KEYEDIT("ButtonSetControl11");
	m_cButtonSetControl[11] = FIND_CONTROL_KEYEDIT("ButtonSetControl12");
	m_cButtonSetControl[12] = FIND_CONTROL_KEYEDIT("ButtonSetControl13");
	m_cButtonSetControl[13] = FIND_CONTROL_KEYEDIT("ButtonSetControl14");
	m_cButtonSetControl[14] = FIND_CONTROL_KEYEDIT("ButtonSetControl15");
	m_cButtonSetControl[15] = FIND_CONTROL_KEYEDIT("ButtonSetControl16");
	m_cButtonSetControl[16] = FIND_CONTROL_KEYEDIT("ButtonSetControl17");
	m_cButtonSetControl[17] = FIND_CONTROL_KEYEDIT("ButtonSetControl18");
	m_cButtonSetControl[18] = FIND_CONTROL_KEYEDIT("ButtonSetControl19");
	m_cStaticControlEdit[0] = FIND_CONTROL_STATIC("StaticControlEdit1");
	m_cStaticControlEdit[1] = FIND_CONTROL_STATIC("StaticControlEdit2");
	m_cStaticControlEdit[2] = FIND_CONTROL_STATIC("StaticControlEdit3");
	m_cStaticControlEdit[3] = FIND_CONTROL_STATIC("StaticControlEdit4");
	m_cStaticControlEdit[4] = FIND_CONTROL_STATIC("StaticControlEdit5");
	m_cStaticControlEdit[5] = FIND_CONTROL_STATIC("StaticControlEdit6");
	m_cStaticControlEdit[6] = FIND_CONTROL_STATIC("StaticControlEdit7");
	m_cStaticControlEdit[7] = FIND_CONTROL_STATIC("StaticControlEdit8");
	m_cStaticControlEdit[8] = FIND_CONTROL_STATIC("StaticControlEdit9");
	m_cStaticControlEdit[9] = FIND_CONTROL_STATIC("StaticControlEdit10");
	m_cStaticControlEdit[10] = FIND_CONTROL_STATIC("StaticControlEdit11");
	m_cStaticControlEdit[11] = FIND_CONTROL_STATIC("StaticControlEdit12");
	m_cStaticControlEdit[12] = FIND_CONTROL_STATIC("StaticControlEdit13");
	m_cStaticControlEdit[13] = FIND_CONTROL_STATIC("StaticControlEdit14");
	m_cStaticControlEdit[14] = FIND_CONTROL_STATIC("StaticControlEdit15");
	m_cStaticControlEdit[15] = FIND_CONTROL_STATIC("StaticControlEdit16");
	m_cStaticControlEdit[16] = FIND_CONTROL_STATIC("StaticControlEdit17");
	m_cStaticControlEdit[17] = FIND_CONTROL_STATIC("StaticControlEdit18");
	m_cStaticControlEdit[18] = FIND_CONTROL_STATIC("StaticControlEdit19");
	m_cStaticControlEdit[19] = FIND_CONTROL_STATIC("StaticControlEdit24");
	m_cButtonControlGaugePoint = FIND_CONTROL_SLIDER("ButtonControlGaugePoint");
	m_cButtonSenseDown = FIND_CONTROL_BUTTON("ButtonSenseDown");
	m_cButtonSenseUp = FIND_CONTROL_BUTTON("ButtonSenseUp");
	m_cImageControlGauge = FIND_CONTROL_IMAGE("ImageControlGauge");
	m_cButtonZoomControlGaugePoint = FIND_CONTROL_SLIDER("ButtonZoomControlGaugePoint");
	m_cButtonZoomSenseDown = FIND_CONTROL_BUTTON("ButtonZoomSenseDown");
	m_cButtonZoomSenseUp = FIND_CONTROL_BUTTON("ButtonZoomSenseUp");
	m_cImageZoomControlGauge = FIND_CONTROL_IMAGE("ImageZoomControlGauge");

	// tab|sound
	m_cStaticVolume = FIND_CONTROL_STATIC("StaticVolume");
	m_cStaticEffectVol = FIND_CONTROL_STATIC("StaticEffectVol");
	m_cStaticRadio = FIND_CONTROL_STATIC("StaticRadio");
	m_cButtonBgmDown = FIND_CONTROL_BUTTON("ButtonBgmDown");
	m_cButtonBgmUp = FIND_CONTROL_BUTTON("ButtonBgmUp");
	m_cButtonSoundDown = FIND_CONTROL_BUTTON("ButtonSoundDown");
	m_cButtonSoundUp = FIND_CONTROL_BUTTON("ButtonSoundUp");
	m_cButtonRadioDown = FIND_CONTROL_BUTTON("ButtonRadioDown");
	m_cButtonRadioUp = FIND_CONTROL_BUTTON("ButtonRadioUp");
	m_cButtonBGMGaugePoint = FIND_CONTROL_SLIDER("ButtonBGMGaugePoint");
	m_cButtonSoundGaugePoint = FIND_CONTROL_SLIDER("ButtonSoundGaugePoint");
	m_cButtonRadioGaugePoint = FIND_CONTROL_SLIDER("ButtonRadioGaugePoint");
	m_cImageBGMGauge = FIND_CONTROL_IMAGE("ImageBGMGauge");
	m_cImageSoundGauge = FIND_CONTROL_IMAGE("ImageSoundGauge");
	m_cImageRadioGauge = FIND_CONTROL_IMAGE("ImageRadioGauge");
	m_cRadioBgmOn = FIND_CONTROL_RADIOBUTTON("RadioBgmOn");
	m_cRadioBgmOff = FIND_CONTROL_RADIOBUTTON("RadioBgmOff");
	m_cRadioSoundOn = FIND_CONTROL_RADIOBUTTON("RadioSoundOn");
	m_cRadioSoundOff = FIND_CONTROL_RADIOBUTTON("RadioSoundOff");
	m_cRadioRadioOn = FIND_CONTROL_RADIOBUTTON("RadioRadioOn");
	m_cRadioRadioOff = FIND_CONTROL_RADIOBUTTON("RadioRadioOff");
	m_cRadioKorean = FIND_CONTROL_RADIOBUTTON("RadioKorean");
	m_cRadioEnglish = FIND_CONTROL_RADIOBUTTON("RadioEnglish");

	// tab|community
	m_cButtonChattingTab = FIND_CONTROL_RADIOBUTTON("ButtonChattingTab");
	m_cButtonFunctionControlTab = FIND_CONTROL_RADIOBUTTON("ButtonFunctionControlTab");
	m_cStaticOptionCommunityDefaultAction1[0] = FIND_CONTROL_STATIC("StaticOptionCommunityDefaultAction1_1");
	m_cStaticOptionCommunityDefaultAction1[1] = FIND_CONTROL_STATIC("StaticOptionCommunityDefaultAction2_1");
	m_cStaticOptionCommunityDefaultAction1[2] = FIND_CONTROL_STATIC("StaticOptionCommunityDefaultAction3_1");
	m_cStaticOptionCommunityDefaultAction1[3] = FIND_CONTROL_STATIC("StaticOptionCommunityDefaultAction4_1");
	m_cStaticOptionCommunityDefaultAction1[4] = FIND_CONTROL_STATIC("StaticOptionCommunityDefaultAction5_1");
	m_cStaticOptionCommunityDefaultAction1[5] = FIND_CONTROL_STATIC("StaticOptionCommunityDefaultAction6_1");
	m_cStaticOptionCommunityDefaultAction1[6] = FIND_CONTROL_STATIC("StaticOptionCommunityDefaultAction7_1");
	m_cStaticOptionCommunityDefaultAction1[7] = FIND_CONTROL_STATIC("StaticOptionCommunityDefaultAction8_1");
	m_cStaticOptionCommunityDefaultAction1[8] = FIND_CONTROL_STATIC("StaticOptionCommunityDefaultAction9_1");
	m_cStaticOptionCommunityDefaultAction1[9] = FIND_CONTROL_STATIC("StaticOptionCommunityDefaultAction10_1");
	m_cButtonSetControl2[0] = FIND_CONTROL_KEYEDIT("ButtonSetControl20");
	m_cButtonSetControl2[1] = FIND_CONTROL_KEYEDIT("ButtonSetControl21");
	m_cButtonSetControl2[2] = FIND_CONTROL_KEYEDIT("ButtonSetControl22");
	m_cStaticControlEdit2[0] = FIND_CONTROL_STATIC("StaticControlEdit20");
	m_cStaticControlEdit2[1] = FIND_CONTROL_STATIC("StaticControlEdit21");
	m_cStaticControlEdit2[2] = FIND_CONTROL_STATIC("StaticControlEdit22");
	m_cEditMacro[0] = FIND_CONTROL_EDIT("EditMacro1");
	m_cEditMacro[1] = FIND_CONTROL_EDIT("EditMacro2");
	m_cEditMacro[2] = FIND_CONTROL_EDIT("EditMacro3");
	m_cEditMacro[3] = FIND_CONTROL_EDIT("EditMacro4");
	m_cRadioRejInvOn = FIND_CONTROL_RADIOBUTTON("RadioRejInvOn");
	m_cRadioRejInvOff = FIND_CONTROL_RADIOBUTTON("RadioRejInvOff");
	m_cRadioRejWhisperOn = FIND_CONTROL_RADIOBUTTON("RadioRejWhisperOn");
	m_cRadioRejWhisperOff = FIND_CONTROL_RADIOBUTTON("RadioRejWhisperOff");
	m_cRadioBlockInviteFriendOn = FIND_CONTROL_RADIOBUTTON("RadioBlockInviteFriendOn");
	m_cRadioBlockInviteFriendOff = FIND_CONTROL_RADIOBUTTON("RadioBlockInviteFriendOff");
	m_cRadioShowAlterably = FIND_CONTROL_RADIOBUTTON("RadioShowAlterably");
	m_cRadioShowBadge = FIND_CONTROL_RADIOBUTTON("RadioShowBadge");
	m_cRadioShowAchieve = FIND_CONTROL_RADIOBUTTON("RadioShowAchieve");
	m_cRadioShowChatAchieve = FIND_CONTROL_RADIOBUTTON("RadioShowChatAchieve");
	m_cRadioShowChatNormal = FIND_CONTROL_RADIOBUTTON("RadioShowChatNormal");
	m_cRadioShowRankMedal = FIND_CONTROL_RADIOBUTTON("RadioShowRankMedal");
	m_cRadioHideRankMedal = FIND_CONTROL_RADIOBUTTON("RadioHideRankMedal");
	m_cRadioShowClanRankMedal = FIND_CONTROL_RADIOBUTTON("RadioShowClanRankMedal");
	m_cRadioHideClanRankMedal = FIND_CONTROL_RADIOBUTTON("RadioHideClanRankMedal");

	// tab|etc
	m_cRadioDefaultRightHand = FIND_CONTROL_RADIOBUTTON("RadioDefaultRightHand");
	m_cRadioLeftHand = FIND_CONTROL_RADIOBUTTON("RadioLeftHand");
	m_cRadioDefaultMouseUpDown = FIND_CONTROL_RADIOBUTTON("RadioDefaultMouseUpDown");
	m_cRadioInverseMouseUpDown = FIND_CONTROL_RADIOBUTTON("RadioInverseMouseUpDown");
	m_cRadioCrossColor[0] = FIND_CONTROL_RADIOBUTTON("RadioCrossColor1");
	m_cRadioCrossColor[1] = FIND_CONTROL_RADIOBUTTON("RadioCrossColor2");
	m_cRadioCrossColor[2] = FIND_CONTROL_RADIOBUTTON("RadioCrossColor3");
	m_cRadioCrossAType = FIND_CONTROL_RADIOBUTTON("RadioCrossAType");
	m_cRadioCrossBType = FIND_CONTROL_RADIOBUTTON("RadioCrossBType");
	m_cRadioCrossCType = FIND_CONTROL_RADIOBUTTON("RadioCrossCType");
	m_cRadioReplaySaveOn = FIND_CONTROL_RADIOBUTTON("RadioReplaySaveOn");
	m_cRadioReplaySaveOff = FIND_CONTROL_RADIOBUTTON("RadioReplaySaveOff");
	m_cImageEtcCross = FIND_CONTROL_IMAGE("ImageEtcCross");
	m_cImageSpray = FIND_CONTROL_IMAGE("ImageSpray");
	m_cButtonSpray[0] = FIND_CONTROL_BUTTON("ButtonSpray1");
	m_cButtonSpray[1] = FIND_CONTROL_BUTTON("ButtonSpray2");
	m_cButtonSpray[2] = FIND_CONTROL_BUTTON("ButtonSpray3");
	m_cButtonSpray[3] = FIND_CONTROL_BUTTON("ButtonSpray4");
	m_cButtonSpray[4] = FIND_CONTROL_BUTTON("ButtonSpray5");
	m_cButtonSpray[5] = FIND_CONTROL_BUTTON("ButtonSpray6");
	m_cStaticSpray[0] = FIND_CONTROL_STATIC("StaticSpray1");
	m_cStaticSpray[1] = FIND_CONTROL_STATIC("StaticSpray2");
	m_cStaticSpray[2] = FIND_CONTROL_STATIC("StaticSpray3");
	m_cStaticSpray[3] = FIND_CONTROL_STATIC("StaticSpray4");
	m_cStaticSpray[4] = FIND_CONTROL_STATIC("StaticSpray5");
	m_cStaticSpray[5] = FIND_CONTROL_STATIC("StaticSpray6");
	m_cScrollBarSprayList = FIND_CONTROL_SCROLLBAR("ScrollBarSprayList");
	m_cRadioLoadingScreenTypeA = FIND_CONTROL_RADIOBUTTON("RadioLoadingScreenTypeA");
	m_cRadioLoadingScreenTypeB = FIND_CONTROL_RADIOBUTTON("RadioLoadingScreenTypeB");

	// TopTab::Graphic
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioAutoMatic);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioGraphicCustom);
	m_cGameOptionSettingTab.AddCtrl(0, m_cImageAutoControl);
	m_cGameOptionSettingTab.AddCtrl(0, m_cButtonAutoGaugePoint);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadio16Bit);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadio32Bit);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioMapHigh);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioMapLow);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioBulletMarkHigh);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioBulletMarkMid);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioBulletMarkLow);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioBulletSmokeHigh);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioBulletSmokeMid);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioBulletSmokeLow);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioBloodOn);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioBloodOff);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioBulletOn);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioBulletOff);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioEnviromentOn);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioEnviromentOff);
	m_cGameOptionSettingTab.AddCtrl(0, m_cButtonGammaDown);
	m_cGameOptionSettingTab.AddCtrl(0, m_cButtonGammaUp);
	m_cGameOptionSettingTab.AddCtrl(0, m_cStaticBright);
	m_cGameOptionSettingTab.AddCtrl(0, m_cButtonGaugePoint);
	m_cGameOptionSettingTab.AddCtrl(0, m_cImageGammaGauge);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioResolutionFull);
	m_cGameOptionSettingTab.AddCtrl(0, m_cRadioResolutionWindow);
	m_cGameOptionSettingTab.AddCtrl(0, m_cStaticResolution);
	m_cGameOptionSettingTab.AddCtrl(0, m_cComboResolutionDBtn);

	// TopTab::Control
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticMouse);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticZoomMouse);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[0]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[1]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[2]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[3]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[4]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[5]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[6]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[7]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[8]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[9]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[10]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[11]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[12]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[13]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[14]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[15]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[16]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[17]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSetControl[18]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[0]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[1]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[2]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[3]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[4]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[5]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[6]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[7]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[8]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[9]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[10]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[11]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[12]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[13]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[14]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[15]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[16]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[17]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[18]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cStaticControlEdit[19]);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonControlGaugePoint);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSenseDown);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonSenseUp);
	m_cGameOptionSettingTab.AddCtrl(1, m_cImageControlGauge);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonZoomControlGaugePoint);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonZoomSenseDown);
	m_cGameOptionSettingTab.AddCtrl(1, m_cButtonZoomSenseUp);
	m_cGameOptionSettingTab.AddCtrl(1, m_cImageZoomControlGauge);

	// TopTab::Sound
	m_cGameOptionSettingTab.AddCtrl(2, m_cStaticVolume);
	m_cGameOptionSettingTab.AddCtrl(2, m_cStaticEffectVol);
	m_cGameOptionSettingTab.AddCtrl(2, m_cStaticRadio);
	m_cGameOptionSettingTab.AddCtrl(2, m_cButtonBgmDown);
	m_cGameOptionSettingTab.AddCtrl(2, m_cButtonBgmUp);
	m_cGameOptionSettingTab.AddCtrl(2, m_cButtonSoundDown);
	m_cGameOptionSettingTab.AddCtrl(2, m_cButtonSoundUp);
	m_cGameOptionSettingTab.AddCtrl(2, m_cButtonRadioDown);
	m_cGameOptionSettingTab.AddCtrl(2, m_cButtonRadioUp);
	m_cGameOptionSettingTab.AddCtrl(2, m_cButtonBGMGaugePoint);
	m_cGameOptionSettingTab.AddCtrl(2, m_cButtonSoundGaugePoint);
	m_cGameOptionSettingTab.AddCtrl(2, m_cButtonRadioGaugePoint);
	m_cGameOptionSettingTab.AddCtrl(2, m_cImageBGMGauge);
	m_cGameOptionSettingTab.AddCtrl(2, m_cImageSoundGauge);
	m_cGameOptionSettingTab.AddCtrl(2, m_cImageRadioGauge);
	m_cGameOptionSettingTab.AddCtrl(2, m_cRadioBgmOn);
	m_cGameOptionSettingTab.AddCtrl(2, m_cRadioBgmOff);
	m_cGameOptionSettingTab.AddCtrl(2, m_cRadioSoundOn);
	m_cGameOptionSettingTab.AddCtrl(2, m_cRadioSoundOff);
	m_cGameOptionSettingTab.AddCtrl(2, m_cRadioRadioOn);
	m_cGameOptionSettingTab.AddCtrl(2, m_cRadioRadioOff);
	m_cGameOptionSettingTab.AddCtrl(2, m_cRadioKorean);
	m_cGameOptionSettingTab.AddCtrl(2, m_cRadioEnglish);

	// TopTab::Community
	m_cGameOptionSettingTab.AddCtrl(3, m_cButtonChattingTab);
	m_cGameOptionSettingTab.AddCtrl(3, m_cButtonFunctionControlTab);
	// Community::Chatting
	m_cOptionCommunityTab.AddCtrl(0, m_cStaticOptionCommunityDefaultAction1[0]);
	m_cOptionCommunityTab.AddCtrl(0, m_cStaticOptionCommunityDefaultAction1[1]);
	m_cOptionCommunityTab.AddCtrl(0, m_cStaticOptionCommunityDefaultAction1[2]);
	m_cOptionCommunityTab.AddCtrl(0, m_cStaticOptionCommunityDefaultAction1[3]);
	m_cOptionCommunityTab.AddCtrl(0, m_cStaticOptionCommunityDefaultAction1[4]);
	m_cOptionCommunityTab.AddCtrl(0, m_cStaticOptionCommunityDefaultAction1[5]);
	m_cOptionCommunityTab.AddCtrl(0, m_cStaticOptionCommunityDefaultAction1[6]);
	m_cOptionCommunityTab.AddCtrl(0, m_cStaticOptionCommunityDefaultAction1[7]);
	m_cOptionCommunityTab.AddCtrl(0, m_cStaticOptionCommunityDefaultAction1[8]);
	m_cOptionCommunityTab.AddCtrl(0, m_cStaticOptionCommunityDefaultAction1[9]);
	m_cOptionCommunityTab.AddCtrl(0, m_cButtonSetControl2[0]);
	m_cOptionCommunityTab.AddCtrl(0, m_cButtonSetControl2[1]);
	m_cOptionCommunityTab.AddCtrl(0, m_cButtonSetControl2[2]);
	m_cOptionCommunityTab.AddCtrl(0, m_cStaticControlEdit2[0]);
	m_cOptionCommunityTab.AddCtrl(0, m_cStaticControlEdit2[1]);
	m_cOptionCommunityTab.AddCtrl(0, m_cStaticControlEdit2[2]);
	m_cOptionCommunityTab.AddCtrl(0, m_cEditMacro[0]);
	m_cOptionCommunityTab.AddCtrl(0, m_cEditMacro[1]);
	m_cOptionCommunityTab.AddCtrl(0, m_cEditMacro[2]);
	m_cOptionCommunityTab.AddCtrl(0, m_cEditMacro[3]);
	// Community::FunctionControl
	m_cOptionCommunityTab.AddCtrl(1, m_cRadioRejInvOn);
	m_cOptionCommunityTab.AddCtrl(1, m_cRadioRejInvOff);
	m_cOptionCommunityTab.AddCtrl(1, m_cRadioRejWhisperOn);
	m_cOptionCommunityTab.AddCtrl(1, m_cRadioRejWhisperOff);
	m_cOptionCommunityTab.AddCtrl(1, m_cRadioBlockInviteFriendOn);
	m_cOptionCommunityTab.AddCtrl(1, m_cRadioBlockInviteFriendOff);
	m_cOptionCommunityTab.AddCtrl(1, m_cRadioShowAlterably);
	m_cOptionCommunityTab.AddCtrl(1, m_cRadioShowBadge);
	m_cOptionCommunityTab.AddCtrl(1, m_cRadioShowAchieve);
	m_cOptionCommunityTab.AddCtrl(1, m_cRadioShowChatAchieve);
	m_cOptionCommunityTab.AddCtrl(1, m_cRadioShowChatNormal);
	m_cOptionCommunityTab.AddCtrl(1, m_cRadioShowRankMedal);
	m_cOptionCommunityTab.AddCtrl(1, m_cRadioHideRankMedal);
	m_cOptionCommunityTab.AddCtrl(1, m_cRadioShowClanRankMedal);
	m_cOptionCommunityTab.AddCtrl(1, m_cRadioHideClanRankMedal);
	// Add TabCtrl
	m_cGameOptionSettingTab.AddCtrl(3, &m_cOptionCommunityTab);

	// TopTab::Etc
	m_cGameOptionSettingTab.AddCtrl(5, m_cRadioDefaultRightHand);
	m_cGameOptionSettingTab.AddCtrl(5, m_cRadioLeftHand);
	m_cGameOptionSettingTab.AddCtrl(5, m_cRadioDefaultMouseUpDown);
	m_cGameOptionSettingTab.AddCtrl(5, m_cRadioInverseMouseUpDown);
	m_cGameOptionSettingTab.AddCtrl(5, m_cRadioCrossColor[0]);
	m_cGameOptionSettingTab.AddCtrl(5, m_cRadioCrossColor[1]);
	m_cGameOptionSettingTab.AddCtrl(5, m_cRadioCrossColor[2]);
	m_cGameOptionSettingTab.AddCtrl(5, m_cRadioCrossAType);
	m_cGameOptionSettingTab.AddCtrl(5, m_cRadioCrossBType);
	m_cGameOptionSettingTab.AddCtrl(5, m_cRadioCrossCType);
	m_cGameOptionSettingTab.AddCtrl(5, m_cRadioReplaySaveOn);
	m_cGameOptionSettingTab.AddCtrl(5, m_cRadioReplaySaveOff);
	m_cGameOptionSettingTab.AddCtrl(5, m_cImageEtcCross);
	m_cGameOptionSettingTab.AddCtrl(5, m_cImageSpray);
	m_cGameOptionSettingTab.AddCtrl(5, m_cButtonSpray[0]);
	m_cGameOptionSettingTab.AddCtrl(5, m_cButtonSpray[1]);
	m_cGameOptionSettingTab.AddCtrl(5, m_cButtonSpray[2]);
	m_cGameOptionSettingTab.AddCtrl(5, m_cButtonSpray[3]);
	m_cGameOptionSettingTab.AddCtrl(5, m_cButtonSpray[4]);
	m_cGameOptionSettingTab.AddCtrl(5, m_cButtonSpray[5]);
	m_cGameOptionSettingTab.AddCtrl(5, m_cStaticSpray[0]);
	m_cGameOptionSettingTab.AddCtrl(5, m_cStaticSpray[1]);
	m_cGameOptionSettingTab.AddCtrl(5, m_cStaticSpray[2]);
	m_cGameOptionSettingTab.AddCtrl(5, m_cStaticSpray[3]);
	m_cGameOptionSettingTab.AddCtrl(5, m_cStaticSpray[4]);
	m_cGameOptionSettingTab.AddCtrl(5, m_cStaticSpray[5]);
	m_cGameOptionSettingTab.AddCtrl(5, m_cScrollBarSprayList);
	m_cGameOptionSettingTab.AddCtrl(5, m_cRadioLoadingScreenTypeA);
	m_cGameOptionSettingTab.AddCtrl(5, m_cRadioLoadingScreenTypeB);

	m_cButtonRestore->SetCommand("Restore");
	m_cButtonBack->SetCommand("Back");
	m_cButtonOk->SetCommand("Ok");
	m_cButtonCancel->SetCommand("Cancel");

	m_cButtonGammaDown->SetCommand("GammaDown");
	m_cButtonGammaUp->SetCommand("GammaUp");

	m_cButtonSenseDown->SetCommand("SenseDown");
	m_cButtonSenseUp->SetCommand("SenseUp");
	m_cButtonZoomSenseDown->SetCommand("ZoomSenseDown");
	m_cButtonZoomSenseUp->SetCommand("ZoomSenseUp");

	m_cButtonBgmDown->SetCommand("BgmDown");
	m_cButtonBgmUp->SetCommand("BgmUp");
	m_cButtonSoundDown->SetCommand("SoundDown");
	m_cButtonSoundUp->SetCommand("SoundUp");
	m_cButtonRadioDown->SetCommand("RadioDown");
	m_cButtonRadioUp->SetCommand("RadioUp");
}

void UIOption::Activate(void)
{
	// opening dialog
	BaseClass::Activate();

	// reset tab
	m_cButtonGraphic->SetSelected(true);

	// loading all configs in here
	MakeDefaultSetting();
	LoadAllSetting();
	RecoverGraphicSetting();
	RecoverControlSetting();
	RecoverSoundSetting();
	RecoverCommunitySetting();
	RecoverEtcSetting();
	SettingToGraphicTab();
	SettingToControlTab();
	SettingToSoundTab();
	SettingToCommunityTab();
	SettingToEtcTab();
}

void UIOption::OnCommandFull(KeyValues *params)
{
	const char *szCommand = params->GetString("command");

	if (!strcmp(szCommand, "Restore"))
	{
		switch (m_cGameOptionSettingTab.GetIndex())
		{
			case 0:
				DefaultGraphicSetting();
				SettingToGraphicTab();
				break;
			case 1:
				DefaultControlSetting();
				SettingToControlTab();
				break;
			case 2:
				DefaultSoundSetting();
				SettingToSoundTab();
				break;
			case 3:
				DefaultCommunitySetting();
				SettingToCommunityTab();
				break;
			case 5:
				DefaultEtcSetting();
				SettingToEtcTab();
				break;
		}

		return;
	}
	if (!strcmp(szCommand, "Back"))
	{
		switch (m_cGameOptionSettingTab.GetIndex())
		{
			case 0:
				RecoverGraphicSetting();
				SettingToGraphicTab();
				break;
			case 1:
				RecoverControlSetting();
				SettingToControlTab();
				break;
			case 2:
				RecoverSoundSetting();
				SettingToSoundTab();
				break;
			case 3:
				RecoverCommunitySetting();
				SettingToCommunityTab();
				break;
			case 5:
				RecoverEtcSetting();
				SettingToEtcTab();
				break;
		}

		return;
	}
	if (!strcmp(szCommand, "Ok"))
	{
		//Get current setting
		memcpy(&m_Setting, &m_CurrentSetting, sizeof(SETTING));
		//Save to file
		SaveAllSetting();
		CloseModal();
		//MsgBox
		return;
	}
	if (!strcmp(szCommand, "Cancel"))
	{
		CloseModal();
		return;
	}

	if (!strcmp(szCommand, "GammaDown"))
	{
		m_cButtonGaugePoint->MoveUp();
		return;
	}
	if (!strcmp(szCommand, "GammaUp"))
	{
		m_cButtonGaugePoint->MoveDown();
		return;
	}

	if (!strcmp(szCommand, "SenseDown"))
	{
		m_cButtonControlGaugePoint->MoveUp();
		return;
	}
	if (!strcmp(szCommand, "SenseUp"))
	{
		m_cButtonControlGaugePoint->MoveDown();
		return;
	}

	if (!strcmp(szCommand, "ZoomSenseDown"))
	{
		m_cButtonZoomControlGaugePoint->MoveUp();
		return;
	}
	if (!strcmp(szCommand, "ZoomSenseUp"))
	{
		m_cButtonZoomControlGaugePoint->MoveDown();
		return;
	}

	if (!strcmp(szCommand, "BgmDown"))
	{
		m_cButtonBGMGaugePoint->MoveUp();
		return;
	}
	if (!strcmp(szCommand, "BgmUp"))
	{
		m_cButtonBGMGaugePoint->MoveDown();
		return;
	}

	if (!strcmp(szCommand, "SoundDown"))
	{
		m_cButtonSoundGaugePoint->MoveUp();
		return;
	}
	if (!strcmp(szCommand, "SoundUp"))
	{
		m_cButtonSoundGaugePoint->MoveDown();
		return;
	}

	if (!strcmp(szCommand, "RadioDown"))
	{
		m_cButtonRadioGaugePoint->MoveUp();
		return;
	}
	if (!strcmp(szCommand, "RadioUp"))
	{
		m_cButtonRadioGaugePoint->MoveDown();
		return;
	}
}

void UIOption::OnRadioButtonCheck(vgui::Panel *panel, int tabposition)
{
////////////////////////////////////////////////////////////////////////////
	//// TOPTAB RADIOBUTTON GROUP ////
////////////////////////////////////////////////////////////////////////////
	if (panel == m_cButtonGraphic)
	{
		TabGraphic();
		return;
	}
	if (panel == m_cButtonControl)
	{
		TabControl();
		return;
	}
	if (panel == m_cButtonSound)
	{
		TabSound();
		return;
	}
	if (panel == m_cButtonCommunity)
	{
		TabCommunity();
		return;
	}
	if (panel == m_cButtonCombat)
	{
		TabCombat();
		return;
	}
	if (panel == m_cButtonEtc)
	{
		TabEtc();
		return;
	}
	if (panel == m_cButtonModeSetting)
	{
		TabModeSetting();
		return;
	}
////////////////////////////////////////////////////////////////////////////
	//// COMMUNITY RADIOBUTTON GROUP ////
////////////////////////////////////////////////////////////////////////////
	if (panel == m_cButtonChattingTab)
	{
		TabChatting();
		return;
	}
	if (panel == m_cButtonFunctionControlTab)
	{
		TabFunctionControl();
		return;
	}
////////////////////////////////////////////////////////////////////////////
	//// CONTROL ////
////////////////////////////////////////////////////////////////////////////
	if (panel == m_cRadio16Bit)
	{
		m_CurrentSetting.VideoMode.bpp = 16;
		return;
	}
	if (panel == m_cRadio32Bit)
	{
		m_CurrentSetting.VideoMode.bpp = 32;
		return;
	}
	if (panel == m_cRadioMapHigh)
	{
		m_CurrentSetting.Mapping = QUALMODE::HIGH;
		return;
	}
	if (panel == m_cRadioMapLow)
	{
		m_CurrentSetting.Mapping = QUALMODE::LOW;
		return;
	}
	if (panel == m_cRadioBulletMarkHigh)
	{
		m_CurrentSetting.BulletMark = QUALMODE::HIGH;
		return;
	}
	if (panel == m_cRadioBulletMarkMid)
	{
		m_CurrentSetting.BulletMark = QUALMODE::MID;
		return;
	}
	if (panel == m_cRadioBulletMarkLow)
	{
		m_CurrentSetting.BulletMark = QUALMODE::LOW;
		return;
	}
	if (panel == m_cRadioBulletSmokeHigh)
	{
		m_CurrentSetting.BulletSmoke = QUALMODE::HIGH;
		return;
	}
	if (panel == m_cRadioBulletSmokeMid)
	{
		m_CurrentSetting.BulletSmoke = QUALMODE::MID;
		return;
	}
	if (panel == m_cRadioBulletSmokeLow)
	{
		m_CurrentSetting.BulletSmoke = QUALMODE::LOW;
		return;
	}
	if (panel == m_cRadioBloodOn)
	{
		m_CurrentSetting.Blood = SWITCH::ON;
		return;
	}
	if (panel == m_cRadioBloodOff)
	{
		m_CurrentSetting.Blood = SWITCH::OFF;
		return;
	}
	if (panel == m_cRadioBulletOn)
	{
		m_CurrentSetting.Bullet = SWITCH::ON;
		return;
	}
	if (panel == m_cRadioBulletOff)
	{
		m_CurrentSetting.Bullet = SWITCH::OFF;
		return;
	}
	if (panel == m_cRadioEnviromentOn)
	{
		m_CurrentSetting.Enviroment = SWITCH::ON;
		return;
	}
	if (panel == m_cRadioEnviromentOff)
	{
		m_CurrentSetting.Enviroment = SWITCH::OFF;
		return;
	}
	if (panel == m_cRadioResolutionFull)
	{
		m_CurrentSetting.VideoMode.window = 0;
		return;
	}
	if (panel == m_cRadioResolutionWindow)
	{
		m_CurrentSetting.VideoMode.window = 1;
		return;
	}
////////////////////////////////////////////////////////////////////////////
	//// SOUND ////
////////////////////////////////////////////////////////////////////////////
	if (panel == m_cRadioBgmOn)
	{
		m_CurrentSetting.BGM = SWITCH::ON;
		return;
	}
	if (panel == m_cRadioBgmOff)
	{
		m_CurrentSetting.BGM = SWITCH::OFF;
		return;
	}
	if (panel == m_cRadioSoundOn)
	{
		m_CurrentSetting.Effect = SWITCH::ON;
		return;
	}
	if (panel == m_cRadioSoundOff)
	{
		m_CurrentSetting.Effect = SWITCH::OFF;
		return;
	}
	if (panel == m_cRadioRadioOn)
	{
		m_CurrentSetting.Radio = SWITCH::ON;
		return;
	}
	if (panel == m_cRadioRadioOff)
	{
		m_CurrentSetting.Radio = SWITCH::OFF;
		return;
	}
////////////////////////////////////////////////////////////////////////////
	//// COMMUNITY ////
////////////////////////////////////////////////////////////////////////////
	if (panel == m_cRadioRejInvOn)
	{
		m_CurrentSetting.RejInv = SWITCH::ON;
		return;
	}
	if (panel == m_cRadioRejInvOff)
	{
		m_CurrentSetting.RejInv = SWITCH::OFF;
		return;
	}
	if (panel == m_cRadioRejWhisperOn)
	{
		m_CurrentSetting.RejWhisper = SWITCH::ON;
		return;
	}
	if (panel == m_cRadioRejWhisperOff)
	{
		m_CurrentSetting.RejWhisper = SWITCH::OFF;
		return;
	}
	if (panel == m_cRadioBlockInviteFriendOn)
	{
		m_CurrentSetting.BlockInviteFriend = SWITCH::ON;
		return;
	}
	if (panel == m_cRadioBlockInviteFriendOff)
	{
		m_CurrentSetting.BlockInviteFriend = SWITCH::OFF;
		return;
	}
	if (panel == m_cRadioShowAlterably)
	{
		m_CurrentSetting.ShowAchieve = 0;
		return;
	}
	if (panel == m_cRadioShowBadge)
	{
		m_CurrentSetting.ShowAchieve = 1;
		return;
	}
	if (panel == m_cRadioShowAchieve)
	{
		m_CurrentSetting.ShowAchieve = 2;
		return;
	}
	if (panel == m_cRadioShowChatAchieve)
	{
		m_CurrentSetting.ShowChatAchieve = SWITCH::ON;
		return;
	}
	if (panel == m_cRadioShowChatNormal)
	{
		m_CurrentSetting.ShowChatAchieve = SWITCH::OFF;
		return;
	}
	if (panel == m_cRadioShowRankMedal)
	{
		m_CurrentSetting.ShowRankMedal = SWITCH::ON;
		return;
	}
	if (panel == m_cRadioHideRankMedal)
	{
		m_CurrentSetting.ShowRankMedal = SWITCH::OFF;
		return;
	}
	if (panel == m_cRadioShowClanRankMedal)
	{
		m_CurrentSetting.ShowClanRankMedal = SWITCH::ON;
		return;
	}
	if (panel == m_cRadioHideClanRankMedal)
	{
		m_CurrentSetting.ShowClanRankMedal = SWITCH::OFF;
		return;
	}
////////////////////////////////////////////////////////////////////////////
	//// ETC ////
////////////////////////////////////////////////////////////////////////////
	if (panel == m_cRadioDefaultRightHand)
	{
		m_CurrentSetting.LeftHand = SWITCH::OFF;
		return;
	}
	if (panel == m_cRadioLeftHand)
	{
		m_CurrentSetting.LeftHand = SWITCH::ON;
		return;
	}
	if (panel == m_cRadioDefaultMouseUpDown)
	{
		m_CurrentSetting.InverseMouse = SWITCH::OFF;
		return;
	}
	if (panel == m_cRadioInverseMouseUpDown)
	{
		m_CurrentSetting.InverseMouse = SWITCH::ON;
		return;
	}
	if (panel == m_cRadioCrossColor[0])
	{
		m_CurrentSetting.CrossColor = 0;
		return;
	}
	if (panel == m_cRadioCrossColor[1])
	{
		m_CurrentSetting.CrossColor = 1;
		return;
	}
	if (panel == m_cRadioCrossColor[2])
	{
		m_CurrentSetting.CrossColor = 2;
		return;
	}
	if (panel == m_cRadioCrossAType)
	{
		m_CurrentSetting.CrossType = 0;
		return;
	}
	if (panel == m_cRadioCrossBType)
	{
		m_CurrentSetting.CrossType = 1;
		return;
	}
	if (panel == m_cRadioCrossCType)
	{
		m_CurrentSetting.CrossType = 2;
		return;
	}
	if (panel == m_cRadioReplaySaveOn)
	{
		m_CurrentSetting.ReplaySave = SWITCH::ON;
		return;
	}
	if (panel == m_cRadioReplaySaveOff)
	{
		m_CurrentSetting.ReplaySave = SWITCH::OFF;
		return;
	}
	if (panel == m_cRadioLoadingScreenTypeA)
	{
		m_CurrentSetting.LoadingScreenType = 1;
		return;
	}
	if (panel == m_cRadioLoadingScreenTypeB)
	{
		m_CurrentSetting.LoadingScreenType = 0;
		return;
	}
}

void UIOption::OnSliderMove(KeyValues *params)
{
	vgui::Panel *panel = (vgui::Panel *)params->GetPtr("panel");
	int value = params->GetInt("value");

	if (panel == m_cButtonAutoGaugePoint)
	{
		return;
	}

	if (panel == m_cButtonGaugePoint)
	{
		m_CurrentSetting.Gamma = value;
		int newWide = value / 100.0f * PROPORT( 300 );
		m_cImageGammaGauge->SetWide( newWide );
		m_cStaticBright->SetText( L"%d", value );
		return;
	}

	if (panel == m_cButtonControlGaugePoint)
	{
		m_CurrentSetting.Sense = value;
		int newWide = value / 100.0f * PROPORT( 210 );
		m_cImageControlGauge->SetWide( newWide );
		m_cStaticMouse->SetText( L"%d", value );
		return;
	}

	if (panel == m_cButtonZoomControlGaugePoint)
	{
		m_CurrentSetting.ZoomSense = value;
		int newWide = value / 100.0f * PROPORT( 210 );
		m_cImageZoomControlGauge->SetWide( newWide );
		m_cStaticZoomMouse->SetText( L"%d", value );
		return;
	}

	if (panel == m_cButtonBGMGaugePoint)
	{
		m_CurrentSetting.BGMVol = value;
		int newWide = value / 100.0f * PROPORT( 191 );
		m_cImageBGMGauge->SetWide( newWide );
		m_cStaticVolume->SetText( L"%d", value );
		return;
	}

	if (panel == m_cButtonSoundGaugePoint)
	{
		m_CurrentSetting.EffectVol = value;
		int newWide = value / 100.0f * PROPORT( 191 );
		m_cImageSoundGauge->SetWide( newWide );
		m_cStaticEffectVol->SetText( L"%d", value );
		return;
	}

	if (panel == m_cButtonRadioGaugePoint)
	{
		m_CurrentSetting.RadioVol = value;
		int newWide = value / 100.0f * PROPORT( 191 );
		m_cImageRadioGauge->SetWide( newWide );
		m_cStaticRadio->SetText( L"%d", value );
		return;
	}
}

void UIOption::OnTextChange(vgui::Panel *panel)
{
	vgui::CFEdit *pEdit = (vgui::CFEdit *)panel;

	if (panel == m_cEditMacro[0])
	{
		pEdit->GetText(m_CurrentSetting.Macro1, sizeof(m_CurrentSetting.Macro1));
		return;
	}
	if (panel == m_cEditMacro[1])
	{
		pEdit->GetText(m_CurrentSetting.Macro2, sizeof(m_CurrentSetting.Macro2));
		return;
	}
	if (panel == m_cEditMacro[2])
	{
		pEdit->GetText(m_CurrentSetting.Macro3, sizeof(m_CurrentSetting.Macro3));
		return;
	}
	if (panel == m_cEditMacro[3])
	{
		pEdit->GetText(m_CurrentSetting.Macro4, sizeof(m_CurrentSetting.Macro4));
		return;
	}
}

//#include "encode.h"
void UIOption::OnKeyCapture(vgui::Panel *panel)
{
//	if (panel == m_cButtonSetControl[0])
//	{
//		vgui::CFKeyEdit *ctrl = (vgui::CFKeyEdit *)panel;
//		m_cStaticControlEdit[0]->SetText(ANSIToUnicode(ctrl->GetKey()));
//	}
}

inline int SwitchToInt(SWITCH in)
{
	switch (in)
	{
		case SWITCH::OFF: return 0;
		case SWITCH::ON: return 1;
	}

	return 0;
}

inline SWITCH IntToSwitch(int in)
{
	switch (in)
	{
		case 0: return SWITCH::OFF;
		case 1: return SWITCH::ON;
	}

	return SWITCH::OFF;
}

inline int QualToInt(QUALMODE in)
{
	switch (in)
	{
		case QUALMODE::LOW: return 0;
		case QUALMODE::MID: return 1;
		case QUALMODE::HIGH: return 2;
	}

	return 0;
}

inline QUALMODE IntToQual(int in)
{
	switch (in)
	{
		case 0: return QUALMODE::LOW;
		case 1: return QUALMODE::MID;
		case 2: return QUALMODE::HIGH;
	}

	return QUALMODE::LOW;
}

inline void DeclareKey(KEYENT &dest, const char *src)
{
	strcpy((char *)&dest, src);
}

inline void CopyKey(KEYENT &src, KEYENT &dest)
{
	memcpy(&dest, &src, sizeof(KEYENT));
}

inline char *KeyToStr(KEYENT &in)
{
	return (char *)&in;
}

inline void StrToKey(const char *src, KEYENT &dest)
{
	memcpy(&dest, src, sizeof(KEYENT));
}

static void KeyValueToSetting(KeyValues *pIn, SETTING *pOut)
{
	KeyValues *pGraphicSetting = pIn->FindKey("Graphic");
	KeyValues *pControlSetting = pIn->FindKey("Control");
	KeyValues *pSoundSetting = pIn->FindKey("Sound");
	KeyValues *pCommunitySetting = pIn->FindKey("Community");
	KeyValues *pEtcSetting = pIn->FindKey("Etc");

	if (pGraphicSetting)
	{
		KeyValues *pVideoMode = pGraphicSetting->FindKey("VideoMode");

		if (pVideoMode)
		{
			pOut->VideoMode.width = pVideoMode->GetInt("Width");
			pOut->VideoMode.height = pVideoMode->GetInt("Height");
			pOut->VideoMode.bpp = pVideoMode->GetInt("BPP");
			pOut->VideoMode.window = pVideoMode->GetInt("Window");
		}

		pOut->Mapping = IntToQual(pGraphicSetting->GetInt("Mapping"));
		pOut->BulletMark = IntToQual(pGraphicSetting->GetInt("BulletMark"));
		pOut->BulletSmoke = IntToQual(pGraphicSetting->GetInt("BulletSmoke"));
		pOut->Blood = IntToSwitch(pGraphicSetting->GetInt("Blood"));
		pOut->Bullet = IntToSwitch(pGraphicSetting->GetInt("Bullet"));
		pOut->Enviroment = IntToSwitch(pGraphicSetting->GetInt("Enviroment"));
		pOut->Gamma = pGraphicSetting->GetInt("Gamma");
	}

	if (pControlSetting)
	{
		StrToKey(pControlSetting->GetString("CtrlFront"), pOut->CtrlFront);
		StrToKey(pControlSetting->GetString("CtrlBack"), pOut->CtrlBack);
		StrToKey(pControlSetting->GetString("CtrlLeft"), pOut->CtrlLeft);
		StrToKey(pControlSetting->GetString("CtrlRight"), pOut->CtrlRight);
		StrToKey(pControlSetting->GetString("CtrlDuck"), pOut->CtrlDuck);
		StrToKey(pControlSetting->GetString("CtrlWalk"), pOut->CtrlWalk);
		StrToKey(pControlSetting->GetString("CtrlJump"), pOut->CtrlJump);
		StrToKey(pControlSetting->GetString("CtrlFire"), pOut->CtrlFire);
		StrToKey(pControlSetting->GetString("CtrlSpecial"), pOut->CtrlSpecial);
		StrToKey(pControlSetting->GetString("CtrlReload"), pOut->CtrlReload);
		StrToKey(pControlSetting->GetString("CtrlDrop"), pOut->CtrlDrop);
		StrToKey(pControlSetting->GetString("CtrlUse"), pOut->CtrlUse);
		StrToKey(pControlSetting->GetString("CtrlBag"), pOut->CtrlBag);
		StrToKey(pControlSetting->GetString("CtrlSlot"), pOut->CtrlSlot);
		StrToKey(pControlSetting->GetString("CtrlPrimary"), pOut->CtrlPrimary);
		StrToKey(pControlSetting->GetString("CtrlSecond"), pOut->CtrlSecond);
		StrToKey(pControlSetting->GetString("CtrlKnife"), pOut->CtrlKnife);
		StrToKey(pControlSetting->GetString("CtrlGrenade"), pOut->CtrlGrenade);
		StrToKey(pControlSetting->GetString("CtrlC4"), pOut->CtrlC4);
		pOut->Sense = pControlSetting->GetInt("Sense");
		pOut->ZoomSense = pControlSetting->GetInt("ZoomSense");
	}

	if (pSoundSetting)
	{
		pOut->BGM = IntToSwitch(pSoundSetting->GetInt("BGM"));
		pOut->BGMVol = pSoundSetting->GetInt("BGMVol");
		pOut->Effect = IntToSwitch(pSoundSetting->GetInt("Effect"));
		pOut->EffectVol = pSoundSetting->GetInt("EffectVol");
		pOut->Radio = IntToSwitch(pSoundSetting->GetInt("Radio"));
		pOut->RadioVol = pSoundSetting->GetInt("RadioVol");
	}

	if (pCommunitySetting)
	{
		StrToKey(pControlSetting->GetString("CtrlRadio1"), pOut->CtrlRadio1);
		StrToKey(pControlSetting->GetString("CtrlRadio2"), pOut->CtrlRadio2);
		StrToKey(pControlSetting->GetString("CtrlRadio3"), pOut->CtrlRadio3);
		swprintf(pOut->Macro1, sizeof(pOut->Macro1)/sizeof(wchar_t), L"%s", pCommunitySetting->GetWString("Macro1"));
		swprintf(pOut->Macro2, sizeof(pOut->Macro2)/sizeof(wchar_t), L"%s", pCommunitySetting->GetWString("Macro2"));
		swprintf(pOut->Macro3, sizeof(pOut->Macro3)/sizeof(wchar_t), L"%s", pCommunitySetting->GetWString("Macro3"));
		swprintf(pOut->Macro4, sizeof(pOut->Macro4)/sizeof(wchar_t), L"%s", pCommunitySetting->GetWString("Macro4"));
		pOut->RejInv = IntToSwitch(pCommunitySetting->GetInt("RejInv"));
		pOut->RejWhisper = IntToSwitch(pCommunitySetting->GetInt("RejWhisper"));
		pOut->BlockInviteFriend = IntToSwitch(pCommunitySetting->GetInt("BlockInviteFriend"));
		pOut->ShowAchieve = pCommunitySetting->GetInt("ShowAchieve");
		pOut->ShowChatAchieve = IntToSwitch(pCommunitySetting->GetInt("ShowChatAchieve"));
		pOut->ShowRankMedal = IntToSwitch(pCommunitySetting->GetInt("ShowRankMedal"));
		pOut->ShowClanRankMedal = IntToSwitch(pCommunitySetting->GetInt("ShowClanRankMedal"));
	}

	if (pEtcSetting)
	{
		pOut->LeftHand = IntToSwitch(pEtcSetting->GetInt("LeftHand"));
		pOut->InverseMouse = IntToSwitch(pEtcSetting->GetInt("InverseMouse"));
		pOut->CrossColor = pEtcSetting->GetInt("CrossColor");
		pOut->CrossType = pEtcSetting->GetInt("CrossType");
		pOut->ReplaySave = IntToSwitch(pEtcSetting->GetInt("ReplaySave"));
		pOut->LoadingScreenType = pEtcSetting->GetInt("LoadingScreenType");
	}
}

static void SettingToKeyValue(SETTING *pIn, KeyValues *pOut)
{
	KeyValues *pGraphicSetting = pOut->CreateNewKey();
	pGraphicSetting->SetName("Graphic");

	KeyValues *pControlSetting = pOut->CreateNewKey();
	pControlSetting->SetName("Control");

	KeyValues *pSoundSetting = pOut->CreateNewKey();
	pSoundSetting->SetName("Sound");

	KeyValues *pCommunitySetting = pOut->CreateNewKey();
	pCommunitySetting->SetName("Community");

	KeyValues *pEtcSetting = pOut->CreateNewKey();
	pEtcSetting->SetName("Etc");
	
	if (pGraphicSetting)
	{
		KeyValues *pVideoMode = pGraphicSetting->CreateNewKey();
		pVideoMode->SetName("VideoMode");

		if (pVideoMode)
		{
			pVideoMode->SetInt("Width", pIn->VideoMode.width);
			pVideoMode->SetInt("Height", pIn->VideoMode.height);
			pVideoMode->SetInt("BPP", pIn->VideoMode.bpp);
			pVideoMode->SetInt("Window", pIn->VideoMode.window);
		}

		pGraphicSetting->SetInt("Mapping", QualToInt(pIn->Mapping));
		pGraphicSetting->SetInt("BulletMark", QualToInt(pIn->BulletMark));
		pGraphicSetting->SetInt("BulletSmoke", QualToInt(pIn->BulletSmoke));
		pGraphicSetting->SetInt("Blood", SwitchToInt(pIn->Blood));
		pGraphicSetting->SetInt("Bullet", SwitchToInt(pIn->Bullet));
		pGraphicSetting->SetInt("Enviroment", SwitchToInt(pIn->Enviroment));
		pGraphicSetting->SetInt("Gamma", pIn->Gamma);
	}

	if (pControlSetting)
	{
		pControlSetting->SetString("CtrlFront", KeyToStr(pIn->CtrlFront));
		pControlSetting->SetString("CtrlBack", KeyToStr(pIn->CtrlBack));
		pControlSetting->SetString("CtrlLeft", KeyToStr(pIn->CtrlLeft));
		pControlSetting->SetString("CtrlRight", KeyToStr(pIn->CtrlRight));
		pControlSetting->SetString("CtrlDuck", KeyToStr(pIn->CtrlDuck));
		pControlSetting->SetString("CtrlWalk", KeyToStr(pIn->CtrlWalk));
		pControlSetting->SetString("CtrlJump", KeyToStr(pIn->CtrlJump));
		pControlSetting->SetString("CtrlFire", KeyToStr(pIn->CtrlFire));
		pControlSetting->SetString("CtrlSpecial", KeyToStr(pIn->CtrlSpecial));
		pControlSetting->SetString("CtrlReload", KeyToStr(pIn->CtrlReload));
		pControlSetting->SetString("CtrlDrop", KeyToStr(pIn->CtrlDrop));
		pControlSetting->SetString("CtrlUse", KeyToStr(pIn->CtrlUse));
		pControlSetting->SetString("CtrlBag", KeyToStr(pIn->CtrlBag));
		pControlSetting->SetString("CtrlSlot", KeyToStr(pIn->CtrlSlot));
		pControlSetting->SetString("CtrlPrimary", KeyToStr(pIn->CtrlPrimary));
		pControlSetting->SetString("CtrlSecond", KeyToStr(pIn->CtrlSecond));
		pControlSetting->SetString("CtrlKnife", KeyToStr(pIn->CtrlKnife));
		pControlSetting->SetString("CtrlGrenade", KeyToStr(pIn->CtrlGrenade));
		pControlSetting->SetString("CtrlC4", KeyToStr(pIn->CtrlC4));
		pControlSetting->SetInt("Sense", pIn->Sense);
		pControlSetting->SetInt("ZoomSense", pIn->ZoomSense);
	}

	if (pSoundSetting)
	{
		pSoundSetting->SetInt("BGM", SwitchToInt(pIn->BGM));
		pSoundSetting->SetInt("BGMVol", pIn->BGMVol);
		pSoundSetting->SetInt("Effect", SwitchToInt(pIn->Effect));
		pSoundSetting->SetInt("EffectVol", pIn->EffectVol);
		pSoundSetting->SetInt("Radio", SwitchToInt(pIn->Radio));
		pSoundSetting->SetInt("RadioVol", pIn->RadioVol);
	}

	if (pCommunitySetting)
	{
		pCommunitySetting->SetString("CtrlRadio1", KeyToStr(pIn->CtrlRadio1));
		pCommunitySetting->SetString("CtrlRadio2", KeyToStr(pIn->CtrlRadio2));
		pCommunitySetting->SetString("CtrlRadio3", KeyToStr(pIn->CtrlRadio3));
		pCommunitySetting->SetWString("Macro1", pIn->Macro1);
		pCommunitySetting->SetWString("Macro2", pIn->Macro2);
		pCommunitySetting->SetWString("Macro3", pIn->Macro3);
		pCommunitySetting->SetWString("Macro4", pIn->Macro4);
		pCommunitySetting->SetInt("RejInv", SwitchToInt(pIn->RejInv));
		pCommunitySetting->SetInt("RejWhisper", SwitchToInt(pIn->RejWhisper));
		pCommunitySetting->SetInt("BlockInviteFriend", SwitchToInt(pIn->BlockInviteFriend));
		pCommunitySetting->SetInt("ShowAchieve", pIn->ShowAchieve);
		pCommunitySetting->SetInt("ShowChatAchieve", SwitchToInt(pIn->ShowChatAchieve));
		pCommunitySetting->SetInt("ShowRankMedal", SwitchToInt(pIn->ShowRankMedal));
		pCommunitySetting->SetInt("ShowClanRankMedal", SwitchToInt(pIn->ShowClanRankMedal));
	}

	if (pEtcSetting)
	{
		pEtcSetting->SetInt("LeftHand", SwitchToInt(pIn->LeftHand));
		pEtcSetting->SetInt("InverseMouse", SwitchToInt(pIn->InverseMouse));
		pEtcSetting->SetInt("CrossColor", pIn->CrossColor);
		pEtcSetting->SetInt("CrossType", pIn->CrossType);
		pEtcSetting->SetInt("ReplaySave", SwitchToInt(pIn->ReplaySave));
		pEtcSetting->SetInt("LoadingScreenType", pIn->LoadingScreenType);
	}
}

void UIOption::LoadAllSetting(void)
{
	KeyValues *pSetting = new KeyValues("Setting");

	if (!pSetting)
	{
		Error("Out of memroy.");
	}

	if (pSetting->LoadFromFile(g_pFullFileSystem, "Setting.vdf") != true)
	{
		memcpy(&m_Setting, &m_DefaultSetting, sizeof(SETTING));
		return;
	}

	KeyValueToSetting(pSetting, &m_Setting);

	pSetting->deleteThis();
}

void UIOption::SaveAllSetting(void)
{
	KeyValues *pSetting = new KeyValues("Setting");

	if (!pSetting)
	{
		Error("Out of memory.");
	}

	SettingToKeyValue(&m_Setting, pSetting);

	pSetting->SaveToFile(g_pFullFileSystem, "Setting.vdf");

	pSetting->deleteThis();
}

void UIOption::MakeDefaultSetting(void)
{
	// Graphic
	m_DefaultSetting.VideoMode.width = 1024;
	m_DefaultSetting.VideoMode.height = 768;
	m_DefaultSetting.VideoMode.bpp = 32;
	m_DefaultSetting.VideoMode.window = 1;
	m_DefaultSetting.Mapping = QUALMODE::HIGH;
	m_DefaultSetting.BulletMark = QUALMODE::HIGH;
	m_DefaultSetting.BulletSmoke = QUALMODE::HIGH;
	m_DefaultSetting.Blood = SWITCH::ON;
	m_DefaultSetting.Bullet = SWITCH::ON;
	m_DefaultSetting.Enviroment = SWITCH::ON;
	m_DefaultSetting.Gamma = 50;

	//Control
	DeclareKey(m_DefaultSetting.CtrlFront, "W");
	DeclareKey(m_DefaultSetting.CtrlBack, "S");
	DeclareKey(m_DefaultSetting.CtrlLeft, "A");
	DeclareKey(m_DefaultSetting.CtrlRight, "D");
	DeclareKey(m_DefaultSetting.CtrlDuck, "CTRL");
	DeclareKey(m_DefaultSetting.CtrlWalk, "SHIFT");
	DeclareKey(m_DefaultSetting.CtrlJump, "SPACE");
	DeclareKey(m_DefaultSetting.CtrlFire, "MOUSE1");
	DeclareKey(m_DefaultSetting.CtrlSpecial, "MOUSE2");
	DeclareKey(m_DefaultSetting.CtrlReload, "R");
	DeclareKey(m_DefaultSetting.CtrlDrop, "G");
	DeclareKey(m_DefaultSetting.CtrlUse, "E");
	DeclareKey(m_DefaultSetting.CtrlBag, "B");
	DeclareKey(m_DefaultSetting.CtrlSlot, "Q");
	DeclareKey(m_DefaultSetting.CtrlPrimary, "1");
	DeclareKey(m_DefaultSetting.CtrlSecond, "2");
	DeclareKey(m_DefaultSetting.CtrlKnife, "3");
	DeclareKey(m_DefaultSetting.CtrlGrenade, "4");
	DeclareKey(m_DefaultSetting.CtrlC4, "5");
	m_DefaultSetting.Sense = 20;
	m_DefaultSetting.ZoomSense = 50;

	//Sound
	m_DefaultSetting.BGM = SWITCH::ON;
	m_DefaultSetting.BGMVol = 50;
	m_DefaultSetting.Effect = SWITCH::ON;
	m_DefaultSetting.EffectVol = 50;
	m_DefaultSetting.Radio = SWITCH::ON;
	m_DefaultSetting.RadioVol = 50;

	//Community
	DeclareKey(m_DefaultSetting.CtrlRadio1, "Z");
	DeclareKey(m_DefaultSetting.CtrlRadio1, "X");
	DeclareKey(m_DefaultSetting.CtrlRadio1, "C");
	wcscpy(m_DefaultSetting.Macro1, L"-");
	wcscpy(m_DefaultSetting.Macro2, L"-");
	wcscpy(m_DefaultSetting.Macro3, L"-");
	wcscpy(m_DefaultSetting.Macro4, L"-");
	m_DefaultSetting.RejInv = SWITCH::OFF;
	m_DefaultSetting.RejWhisper = SWITCH::OFF;
	m_DefaultSetting.BlockInviteFriend = SWITCH::OFF;
	m_DefaultSetting.ShowAchieve = 0;
	m_DefaultSetting.ShowChatAchieve = SWITCH::ON;
	m_DefaultSetting.ShowRankMedal = SWITCH::ON;
	m_DefaultSetting.ShowClanRankMedal = SWITCH::ON;

	//Etc
	m_DefaultSetting.LeftHand = SWITCH::OFF;
	m_DefaultSetting.InverseMouse = SWITCH::OFF;
	m_DefaultSetting.CrossColor = 0;
	m_DefaultSetting.CrossType = 0;
	m_DefaultSetting.ReplaySave = SWITCH::OFF;
	m_DefaultSetting.LoadingScreenType = 0;
}

void UIOption::RecoverGraphicSetting(void)
{
	m_CurrentSetting.VideoMode.width = m_Setting.VideoMode.width;
	m_CurrentSetting.VideoMode.height = m_Setting.VideoMode.height;
	m_CurrentSetting.VideoMode.bpp = m_Setting.VideoMode.bpp;
	m_CurrentSetting.VideoMode.window = m_Setting.VideoMode.window;
	m_CurrentSetting.Mapping = m_Setting.Mapping;
	m_CurrentSetting.BulletMark = m_Setting.BulletMark;
	m_CurrentSetting.BulletSmoke = m_Setting.BulletSmoke;
	m_CurrentSetting.Blood = m_Setting.Blood;
	m_CurrentSetting.Bullet = m_Setting.Bullet;
	m_CurrentSetting.Enviroment = m_Setting.Enviroment;
	m_CurrentSetting.Gamma = m_Setting.Gamma;
}

void UIOption::RecoverControlSetting(void)
{
	CopyKey(m_Setting.CtrlFront, m_CurrentSetting.CtrlFront);
	CopyKey(m_Setting.CtrlBack, m_CurrentSetting.CtrlBack);
	CopyKey(m_Setting.CtrlLeft, m_CurrentSetting.CtrlLeft);
	CopyKey(m_Setting.CtrlRight, m_CurrentSetting.CtrlRight);
	CopyKey(m_Setting.CtrlDuck, m_CurrentSetting.CtrlDuck);
	CopyKey(m_Setting.CtrlWalk, m_CurrentSetting.CtrlWalk);
	CopyKey(m_Setting.CtrlJump, m_CurrentSetting.CtrlJump);
	CopyKey(m_Setting.CtrlFire, m_CurrentSetting.CtrlFire);
	CopyKey(m_Setting.CtrlSpecial, m_CurrentSetting.CtrlSpecial);
	CopyKey(m_Setting.CtrlReload, m_CurrentSetting.CtrlReload);
	CopyKey(m_Setting.CtrlDrop, m_CurrentSetting.CtrlDrop);
	CopyKey(m_Setting.CtrlUse, m_CurrentSetting.CtrlUse);
	CopyKey(m_Setting.CtrlBag, m_CurrentSetting.CtrlBag);
	CopyKey(m_Setting.CtrlSlot, m_CurrentSetting.CtrlSlot);
	CopyKey(m_Setting.CtrlPrimary, m_CurrentSetting.CtrlPrimary);
	CopyKey(m_Setting.CtrlSecond, m_CurrentSetting.CtrlSecond);
	CopyKey(m_Setting.CtrlKnife, m_CurrentSetting.CtrlKnife);
	CopyKey(m_Setting.CtrlGrenade, m_CurrentSetting.CtrlGrenade);
	CopyKey(m_Setting.CtrlC4, m_CurrentSetting.CtrlC4);
	m_CurrentSetting.Sense = m_Setting.Sense;
	m_CurrentSetting.ZoomSense = m_Setting.ZoomSense;
}

void UIOption::RecoverSoundSetting(void)
{
	m_CurrentSetting.BGM = m_Setting.BGM;
	m_CurrentSetting.BGMVol = m_Setting.BGMVol;
	m_CurrentSetting.Effect = m_Setting.Effect;
	m_CurrentSetting.EffectVol = m_Setting.EffectVol;
	m_CurrentSetting.Radio = m_Setting.Radio;
	m_CurrentSetting.RadioVol = m_Setting.RadioVol;
}

void UIOption::RecoverCommunitySetting(void)
{
	CopyKey(m_Setting.CtrlRadio1, m_CurrentSetting.CtrlRadio1);
	CopyKey(m_Setting.CtrlRadio2, m_CurrentSetting.CtrlRadio2);
	CopyKey(m_Setting.CtrlRadio3, m_CurrentSetting.CtrlRadio3);
	memcpy(&m_CurrentSetting.Macro1, &m_Setting.Macro1, sizeof(m_Setting.Macro1));
	memcpy(&m_CurrentSetting.Macro2, &m_Setting.Macro2, sizeof(m_Setting.Macro2));
	memcpy(&m_CurrentSetting.Macro3, &m_Setting.Macro3, sizeof(m_Setting.Macro3));
	memcpy(&m_CurrentSetting.Macro4, &m_Setting.Macro4, sizeof(m_Setting.Macro4));
	m_CurrentSetting.RejInv = m_Setting.RejInv;
	m_CurrentSetting.RejWhisper = m_Setting.RejWhisper;
	m_CurrentSetting.BlockInviteFriend = m_Setting.BlockInviteFriend;
	m_CurrentSetting.ShowAchieve = m_Setting.ShowAchieve;
	m_CurrentSetting.ShowChatAchieve = m_Setting.ShowChatAchieve;
	m_CurrentSetting.ShowRankMedal = m_Setting.ShowRankMedal;
	m_CurrentSetting.ShowClanRankMedal = m_Setting.ShowClanRankMedal;
}

void UIOption::RecoverEtcSetting(void)
{
	m_CurrentSetting.LeftHand = m_Setting.LeftHand;
	m_CurrentSetting.InverseMouse = m_Setting.InverseMouse;
	m_CurrentSetting.CrossColor = m_Setting.CrossColor;
	m_CurrentSetting.CrossType = m_Setting.CrossType;
	m_CurrentSetting.ReplaySave = m_Setting.ReplaySave;
	m_CurrentSetting.LoadingScreenType = m_Setting.LoadingScreenType;
}

void UIOption::DefaultGraphicSetting(void)
{
	m_CurrentSetting.VideoMode.width = m_DefaultSetting.VideoMode.width;
	m_CurrentSetting.VideoMode.height = m_DefaultSetting.VideoMode.height;
	m_CurrentSetting.VideoMode.bpp = m_DefaultSetting.VideoMode.bpp;
	m_CurrentSetting.VideoMode.window = m_DefaultSetting.VideoMode.window;
	m_CurrentSetting.Mapping = m_DefaultSetting.Mapping;
	m_CurrentSetting.BulletMark = m_DefaultSetting.BulletMark;
	m_CurrentSetting.BulletSmoke = m_DefaultSetting.BulletSmoke;
	m_CurrentSetting.Blood = m_DefaultSetting.Blood;
	m_CurrentSetting.Bullet = m_DefaultSetting.Bullet;
	m_CurrentSetting.Enviroment = m_DefaultSetting.Enviroment;
	m_CurrentSetting.Gamma = m_DefaultSetting.Gamma;
}

void UIOption::DefaultControlSetting(void)
{
	CopyKey(m_DefaultSetting.CtrlFront, m_CurrentSetting.CtrlFront);
	CopyKey(m_DefaultSetting.CtrlBack, m_CurrentSetting.CtrlBack);
	CopyKey(m_DefaultSetting.CtrlLeft, m_CurrentSetting.CtrlLeft);
	CopyKey(m_DefaultSetting.CtrlRight, m_CurrentSetting.CtrlRight);
	CopyKey(m_DefaultSetting.CtrlDuck, m_CurrentSetting.CtrlDuck);
	CopyKey(m_DefaultSetting.CtrlWalk, m_CurrentSetting.CtrlWalk);
	CopyKey(m_DefaultSetting.CtrlJump, m_CurrentSetting.CtrlJump);
	CopyKey(m_DefaultSetting.CtrlFire, m_CurrentSetting.CtrlFire);
	CopyKey(m_DefaultSetting.CtrlSpecial, m_CurrentSetting.CtrlSpecial);
	CopyKey(m_DefaultSetting.CtrlReload, m_CurrentSetting.CtrlReload);
	CopyKey(m_DefaultSetting.CtrlDrop, m_CurrentSetting.CtrlDrop);
	CopyKey(m_DefaultSetting.CtrlUse, m_CurrentSetting.CtrlUse);
	CopyKey(m_DefaultSetting.CtrlBag, m_CurrentSetting.CtrlBag);
	CopyKey(m_DefaultSetting.CtrlSlot, m_CurrentSetting.CtrlSlot);
	CopyKey(m_DefaultSetting.CtrlPrimary, m_CurrentSetting.CtrlPrimary);
	CopyKey(m_DefaultSetting.CtrlSecond, m_CurrentSetting.CtrlSecond);
	CopyKey(m_DefaultSetting.CtrlKnife, m_CurrentSetting.CtrlKnife);
	CopyKey(m_DefaultSetting.CtrlGrenade, m_CurrentSetting.CtrlGrenade);
	CopyKey(m_DefaultSetting.CtrlC4, m_CurrentSetting.CtrlC4);
	m_CurrentSetting.Sense = m_DefaultSetting.Sense;
	m_CurrentSetting.ZoomSense = m_DefaultSetting.ZoomSense;
}

void UIOption::DefaultSoundSetting(void)
{
	m_CurrentSetting.BGM = m_DefaultSetting.BGM;
	m_CurrentSetting.BGMVol = m_DefaultSetting.BGMVol;
	m_CurrentSetting.Effect = m_DefaultSetting.Effect;
	m_CurrentSetting.EffectVol = m_DefaultSetting.EffectVol;
	m_CurrentSetting.Radio = m_DefaultSetting.Radio;
	m_CurrentSetting.RadioVol = m_DefaultSetting.RadioVol;
}

void UIOption::DefaultCommunitySetting(void)
{
	CopyKey(m_DefaultSetting.CtrlRadio1, m_CurrentSetting.CtrlRadio1);
	CopyKey(m_DefaultSetting.CtrlRadio2, m_CurrentSetting.CtrlRadio2);
	CopyKey(m_DefaultSetting.CtrlRadio3, m_CurrentSetting.CtrlRadio3);
	memcpy(&m_CurrentSetting.Macro1, &m_DefaultSetting.Macro1, sizeof(m_Setting.Macro1));
	memcpy(&m_CurrentSetting.Macro2, &m_DefaultSetting.Macro2, sizeof(m_Setting.Macro2));
	memcpy(&m_CurrentSetting.Macro3, &m_DefaultSetting.Macro3, sizeof(m_Setting.Macro3));
	memcpy(&m_CurrentSetting.Macro4, &m_DefaultSetting.Macro4, sizeof(m_Setting.Macro4));
	m_CurrentSetting.RejInv = m_DefaultSetting.RejInv;
	m_CurrentSetting.RejWhisper = m_DefaultSetting.RejWhisper;
	m_CurrentSetting.BlockInviteFriend = m_DefaultSetting.BlockInviteFriend;
	m_CurrentSetting.ShowAchieve = m_DefaultSetting.ShowAchieve;
	m_CurrentSetting.ShowChatAchieve = m_DefaultSetting.ShowChatAchieve;
	m_CurrentSetting.ShowRankMedal = m_DefaultSetting.ShowRankMedal;
	m_CurrentSetting.ShowClanRankMedal = m_DefaultSetting.ShowClanRankMedal;
}

void UIOption::DefaultEtcSetting(void)
{
	m_CurrentSetting.LeftHand = m_DefaultSetting.LeftHand;
	m_CurrentSetting.InverseMouse = m_DefaultSetting.InverseMouse;
	m_CurrentSetting.CrossColor = m_DefaultSetting.CrossColor;
	m_CurrentSetting.CrossType = m_DefaultSetting.CrossType;
	m_CurrentSetting.ReplaySave = m_DefaultSetting.ReplaySave;
	m_CurrentSetting.LoadingScreenType = m_DefaultSetting.LoadingScreenType;
}

void UIOption::SettingToGraphicTab(void)
{
	switch (m_CurrentSetting.VideoMode.bpp)
	{
		case 16: m_cRadio16Bit->SetSelected(true);
			break;
		case 32: m_cRadio32Bit->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.VideoMode.window)
	{
		case 0: m_cRadioResolutionFull->SetSelected(true);
			break;
		case 1: m_cRadioResolutionWindow->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.Mapping)
	{
		case QUALMODE::LOW: m_cRadioMapLow->SetSelected(true);
			break;
		case QUALMODE::HIGH: m_cRadioMapHigh->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.BulletMark)
	{
		case QUALMODE::LOW: m_cRadioBulletMarkLow->SetSelected(true);
			break;
		case QUALMODE::MID: m_cRadioBulletMarkMid->SetSelected(true);
			break;
		case QUALMODE::HIGH: m_cRadioBulletMarkHigh->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.BulletSmoke)
	{
		case QUALMODE::LOW: m_cRadioBulletSmokeLow->SetSelected(true);
			break;
		case QUALMODE::MID: m_cRadioBulletSmokeMid->SetSelected(true);
			break;
		case QUALMODE::HIGH: m_cRadioBulletSmokeHigh->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.Blood)
	{
		case SWITCH::OFF: m_cRadioBloodOff->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioBloodOn->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.Bullet)
	{
		case SWITCH::OFF: m_cRadioBulletOff->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioBulletOn->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.Enviroment)
	{
		case SWITCH::OFF: m_cRadioEnviromentOff->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioEnviromentOn->SetSelected(true);
			break;
	}

	m_cButtonGaugePoint->SetValue(m_CurrentSetting.Gamma);
}

void UIOption::SettingToControlTab(void)
{
	m_cButtonControlGaugePoint->SetValue(m_CurrentSetting.Sense);
	m_cButtonZoomControlGaugePoint->SetValue(m_CurrentSetting.ZoomSense);
}

void UIOption::SettingToSoundTab(void)
{
	switch (m_CurrentSetting.BGM)
	{
		case SWITCH::OFF: m_cRadioBgmOff->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioBgmOn->SetSelected(true);
			break;
	}

	m_cButtonBGMGaugePoint->SetValue(m_CurrentSetting.BGMVol);

	switch (m_CurrentSetting.Effect)
	{
		case SWITCH::OFF: m_cRadioSoundOff->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioSoundOn->SetSelected(true);
			break;
	}

	m_cButtonSoundGaugePoint->SetValue(m_CurrentSetting.EffectVol);

	switch (m_CurrentSetting.Radio)
	{
		case SWITCH::OFF: m_cRadioRadioOff->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioRadioOn->SetSelected(true);
			break;
	}

	m_cButtonRadioGaugePoint->SetValue(m_CurrentSetting.RadioVol);
}

void UIOption::SettingToCommunityTab(void)
{
	m_cEditMacro[0]->SetText(m_CurrentSetting.Macro1);
	m_cEditMacro[1]->SetText(m_CurrentSetting.Macro2);
	m_cEditMacro[2]->SetText(m_CurrentSetting.Macro3);
	m_cEditMacro[3]->SetText(m_CurrentSetting.Macro4);

	switch (m_CurrentSetting.RejInv)
	{
		case SWITCH::OFF: m_cRadioRejInvOff->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioRejInvOn->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.RejWhisper)
	{
		case SWITCH::OFF: m_cRadioRejWhisperOff->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioRejWhisperOn->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.BlockInviteFriend)
	{
		case SWITCH::OFF: m_cRadioBlockInviteFriendOff->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioBlockInviteFriendOn->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.ShowAchieve)
	{
		case 0: m_cRadioShowAlterably->SetSelected(true);
			break;
		case 1: m_cRadioShowBadge->SetSelected(true);
			break;
		case 2: m_cRadioShowAchieve->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.ShowChatAchieve)
	{
		case SWITCH::OFF: m_cRadioShowChatNormal->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioShowChatAchieve->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.ShowRankMedal)
	{
		case SWITCH::OFF: m_cRadioHideRankMedal->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioShowRankMedal->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.ShowClanRankMedal)
	{
		case SWITCH::OFF: m_cRadioHideClanRankMedal->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioShowClanRankMedal->SetSelected(true);
			break;
	}
}

void UIOption::SettingToEtcTab(void)
{
	switch (m_CurrentSetting.LeftHand)
	{
		case SWITCH::OFF: m_cRadioDefaultRightHand->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioLeftHand->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.InverseMouse)
	{
		case SWITCH::OFF: m_cRadioDefaultMouseUpDown->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioInverseMouseUpDown->SetSelected(true);
			break;
	}
	
	switch (m_CurrentSetting.CrossColor)
	{
		case 0: m_cRadioCrossColor[0]->SetSelected(true);
			break;
		case 1: m_cRadioCrossColor[1]->SetSelected(true);
			break;
		case 2: m_cRadioCrossColor[2]->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.CrossType)
	{
		case 0: m_cRadioCrossAType->SetSelected(true);
			break;
		case 1: m_cRadioCrossBType->SetSelected(true);
			break;
		case 2: m_cRadioCrossCType->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.ReplaySave)
	{
		case SWITCH::OFF: m_cRadioReplaySaveOff->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioReplaySaveOn->SetSelected(true);
			break;
	}

	switch (m_CurrentSetting.LoadingScreenType)
	{
		case SWITCH::OFF: m_cRadioLoadingScreenTypeB->SetSelected(true);
			break;
		case SWITCH::ON: m_cRadioLoadingScreenTypeA->SetSelected(true);
			break;
	}
}

void UIOption::TabGraphic(void)
{
	m_cImageCombatBG->SetImage("UI/UI_Option/GraphicBG.PNG");
	m_cGameOptionSettingTab.Switch(0);
}

void UIOption::TabControl(void)
{
	m_cImageCombatBG->SetImage("UI/UI_Option/ControlBG.PNG");
	m_cGameOptionSettingTab.Switch(1);
}

void UIOption::TabSound(void)
{
	m_cImageCombatBG->SetImage("UI/UI_Option/SoundBG.PNG");
	m_cGameOptionSettingTab.Switch(2);
}

void UIOption::TabCommunity(void)
{
	m_cGameOptionSettingTab.Switch(3);
	// reset tab
	m_cButtonChattingTab->SetSelected(true);
}

void UIOption::TabCombat(void)
{
	m_cImageCombatBG->SetImage("UI/UI_Option/CombatBG.PNG");
	m_cGameOptionSettingTab.Switch(4);
}

void UIOption::TabEtc(void)
{
	m_cImageCombatBG->SetImage("UI/UI_Option/EtcBG.PNG");
	m_cGameOptionSettingTab.Switch(5);
}

void UIOption::TabModeSetting(void)
{
	m_cImageCombatBG->SetImage("UI/UI_Option/ModeSettingBG_WAVE.PNG");
	m_cGameOptionSettingTab.Switch(6);
}

void UIOption::TabChatting(void)
{
	m_cImageCombatBG->SetImage("UI/UI_Option/Community_ChattingBG.PNG");
	m_cOptionCommunityTab.Switch(0);
}

void UIOption::TabFunctionControl(void)
{
	m_cImageCombatBG->SetImage("UI/UI_Option/Community_FunctionControlBG.PNG");
	m_cOptionCommunityTab.Switch(1);
}