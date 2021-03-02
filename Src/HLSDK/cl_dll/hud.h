/***
 * 
 * Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 * 
 * This product contains software technology licensed from Id
 * Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 * All Rights Reserved.
 * 
 * Use, distribution, and modification of this source code and/or resulting
 * object code is restricted to non-commercial enhancements to products from
 * Valve LLC. All other use, distribution, or modification is prohibited
 * without written permission from Valve LLC.
 * 
***/

#ifndef HUD_H
#define HUD_H

#include "parsemsg.h"

#include "ClientState.h"
#include "GameState.h"

#include "qgl.h"

#include "MetaHook_Interface.h"

#include "util.h"
#include "encode.h"

#include "buffer.h"

#define ScreenWide	( gHUD.m_scrinfo.iWidth )
#define ScreenTall	( gHUD.m_scrinfo.iHeight )

#define XPROJECT(x) ( (1.0 + (x)) * ScreenWide * 0.5 )
#define YPROJECT(y) ( (1.0 - (y)) * ScreenTall * 0.5 )

#define LocalIndex	( gEngfuncs.GetLocalPlayer()->index )
#define ClientTime	( gHUD.m_flTime )

#define OBS_NONE			0
#define OBS_CHASE_LOCKED	1
#define OBS_CHASE_FREE		2
#define OBS_ROAMING			3
#define OBS_IN_EYE			4
#define OBS_MAP_FREE		5
#define OBS_MAP_CHASE		6

#define IsFirstPerson	( ClientState.iUser1 == OBS_NONE || ClientState.iUser1 == OBS_IN_EYE )

struct HudColor
{
	int r, g, b, a;
};

enum
{
	HUD_NONE,
	HUD_CHARACTER,
	HUD_HEALTH,
	HUD_AMMO,
	HUD_WEAPONSLOT,
	HUD_CROSSHAIR,
	HUD_SNIPERSCOPE,
	HUD_RADAR,
	HUD_GOALSCORE,
	HUD_NOTIFYBOX,
	HUD_GAUGE,
	HUD_GAMETIP,
	HUD_RADIO,
	HUD_TACTICALMAP,
	HUD_SCOREBOARD,
	HUD_KILLMSG,
	HUD_UNDERMARK,
	HUD_SPECTATE,
	HUD_BAGICON,
	HUD_SENSITY,
	HUD_DAMAGEARROW,
	HUD_MISSIONLEFT,
	HUD_ACEICON,
	HUD_TEAMMATEINFO,
	HUD_KILLERICON,
	HUD_CENTERTIPS,
	HUD_WINSTATUS,
	HUD_MARK,
	HUD_FLASHBANGEFFECT,
};

//-----------------------------------------------------------------
// BASE
//-----------------------------------------------------------------
class CHudBase
{
public:
	virtual void Init(void) {};
	virtual void Shutdown(void) {};
	virtual void VidInit(void) {};
	virtual void Reset(void) {};
	virtual void DataInit(void) {};
	virtual void Think(void) {};
	virtual void Redraw(void) {};
	virtual void Save(CBufferWriter &buf) {};
	virtual void Restore(CBufferReader &buf) {};
};

//-----------------------------------------------------------------
// CHARACTER
//-----------------------------------------------------------------
class CHudCharacter : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);
	void Save(CBufferWriter &buf);
	void Restore(CBufferReader &buf);
};

//-----------------------------------------------------------------
// HEALTH
//-----------------------------------------------------------------
class CHudHealth : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void DrawNumber(int n ,int x, int y);

	tex_t *m_pBackgroundArmor, *m_pBackgroundHealth;
	tex_t *m_pNumber;
};

//-----------------------------------------------------------------
// AMMO
//-----------------------------------------------------------------
class CHudAmmo : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void DrawNumber(int n, int x, int y, int r, int g, int b);

	tex_t *m_pBackground;
	tex_t *m_pNumber;

	wchar_t m_szCacheName[64];
	int m_iWeaponId;
};

//-----------------------------------------------------------------
// WEAPON SLOT
//-----------------------------------------------------------------
class CHudWeaponSlot : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void GetSelectIcon(void *data, tex_t *&icon, int &fix);

	void AddPlayerWeapon(int id);
	void RemovePlayerWeapon(int id);

	void MsgFunc_WeapSlot(const char *pszName, int iSize, void *pbuf);
	void MsgFunc_WeapSlotA(const char *pszName, int iSize, void *pbuf);

	int m_iActiveWeapon;
	int m_iActiveSlot;
	float m_flStartTime;

	class CBasePlayerWeapon
	{
	public:
		int m_iActive;
		int m_iId;
		int m_iSlot;
		void *m_pData;
		CBasePlayerWeapon *m_pNext;
	};

	CBasePlayerWeapon *m_rgpPlayerWeapons[5];

	tex_t *m_pWeaponSlot;
};

//-----------------------------------------------------------------
// CROSSHAIR
//-----------------------------------------------------------------
class CHudCrosshair : public CHudBase
{
public:
	void Redraw(void);

	void Fire(void);

	int m_iAmmoLastCheck;
	float m_flCrosshairDistance;
	float m_flLastCalcTime;
};

//-----------------------------------------------------------------
// SNIPER CROSSHAIR
//-----------------------------------------------------------------
class CHudSniperScope : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void DrawLine(int x0, int y0, int x1, int y1);
	void DrawAuxLine(int x0, int y0, int x1, int y1);
	void DrawPoint(int x0, int y0);
	void FillRect(int x0, int y0, int x1, int y1);

	int m_iShaderProgram;
};

//-----------------------------------------------------------------
// RADAR
//-----------------------------------------------------------------
class CHudRadar : public CHudBase
{
public:
	void VidInit(void);
	void DataInit(void);
	void Redraw(void);

	bool IsVisible(void);
	void Show(void);
	void Hide(void);

	void DrawPlayer(void);
	void DrawRadio(void);
	void DrawBombPoint(void);
	void DrawC4(void);

	// Transform the coordinates of the world space to the coordinates of the overview space
	void WorldToOverview(Vector &vecWorldPos, Vector &vecOverviewPos);

	// Transform the coordinates of the world space to the coordinates of the radar hud
	void WorldToRadar(Vector &vecWorldPos, Vector &vecRadarPos, bool *bOutSide, float *flAngle);

	struct OverviewData_t
	{
		bool bRotated;
		Vector vecOrigin;
		float flZoom;
	};

	bool m_bDisplay;

	OverviewData_t m_OverviewData;

	tex_t *m_pBackground;
	tex_t *m_pFrame;
	tex_t *m_pMapIcon;
	tex_t *m_pRadarIcon;

	Vector m_vecEyePos;
	float m_flEyeYaw;

	float m_fFrameYaw;
};

//-----------------------------------------------------------------
// LOCATION
//-----------------------------------------------------------------
class CHudLocation : public CHudBase
{
public:
	void Init(void);
	void Redraw(void);

	wchar_t m_szLocation[32];
};

//-----------------------------------------------------------------
// GOALSCORE
//-----------------------------------------------------------------
class CHudGoalScore : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void DrawTimer(int x, int y);

	void DrawCenterNumber(int n, int x, int y, tex_t *tex, int r, int g, int b);
	void DrawSmallNumber(int n, int x, int y, int r, int g, int b);

	tex_t *m_pBackgroundBL, *m_pBackgroundGR;
	tex_t *m_pWhiteNumber, *m_pGrayNumber, *m_pSmallNumber;

	float m_flNextFlash;
	bool m_fFlash;
};

//-----------------------------------------------------------------
// MSGBOX
//-----------------------------------------------------------------
class CHudNotifyBox : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void MsgFunc_Notify(const char *pszName, int iSize, void *pbuf);

	tex_t *m_iBack;

	wchar_t m_szMsg[64];
	float m_flDisplay;
};

//-----------------------------------------------------------------
// GAUGE
//-----------------------------------------------------------------
class CHudGauge : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void MsgFunc_Gauge(const char *pszName, int iSize, void *pbuf);

	float m_flStartTime;
	float m_flHoldTime;

	tex_t *m_pGaugeBoxR;
	tex_t *m_pGaugeC;
	tex_t *m_pGaugeEffectC;
};

//-----------------------------------------------------------------
// GAMETIP
//-----------------------------------------------------------------
class CHudGameTip : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void MsgFunc_GameTip(const char *pszName, int iSize, void *pbuf);

	float m_flStartTime;
	float m_flHoldTime;

	wchar_t *m_szText1;
	wchar_t *m_szText2;
	int m_iWide1;
	int m_iWide2;
};

//-----------------------------------------------------------------
// RADIO
//-----------------------------------------------------------------
class CHudRadio : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Redraw(void);

	void Show(int group);
	void Hide(void);

	bool IsVisable(void);

	void Select(int num);

	int m_iGroup;
};

//-----------------------------------------------------------------
// TACTICAL MAP
//-----------------------------------------------------------------
class CHudTacticalMap : public CHudBase
{
public:
	void VidInit(void);
	void DataInit(void);
	void Redraw(void);

	void WorldToMap(Vector &world, Vector &point);

	void Show(void);
	void Hide(void);

	float m_flStartIn;
	float m_flStartOut;

	tex_t *m_pMapTexture;
};

//-----------------------------------------------------------------
// SCOREBOARD
//-----------------------------------------------------------------
class CHudScoreBoard : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void Show(void);
	void Hide(void);

	void PreparePlayerInfo(void);

	class PlayerInfo
	{
	public:
		int index;
		wchar_t name[32];
		wchar_t clan[32];
		int iclass;
		int kills;
		int deads;
		int ping;
		bool alive;
		int acetype;
		bool isbot;
	};

	PlayerInfo m_PlayerInfoGR[32];
	PlayerInfo m_PlayerInfoBL[32];

	float m_flStartIn;
	float m_flStartOut;

	float m_flNextUpdate;

	tex_t *m_pScoreBoard;
	tex_t *m_pClassIcon;
	tex_t *m_pClanMark;
	tex_t *m_pAceIcon;
	tex_t *m_pDeathIcon;

	tex_t *m_pMissionBox;
};

//-----------------------------------------------------------------
// KILL MESSAGE
//-----------------------------------------------------------------
class CHudKillMsg : public CHudBase
{
public:
	void VidInit(void);
	void Think(void);
	void Redraw(void);

	void Add(int iKillerId, int iVictimId, int iWeaponId, int iMultiKill, int iFlags);
	void Clear(void);

	void MsgFunc_ResetRound(const char *pszName, int iSize, void *pbuf);
	void MsgFunc_KillMsg(const char *pszName, int iSize, void *pbuf);

	void ScrollUp(void);

	struct KillMsg
	{
		bool active;
		wchar_t killer_name[32];
		int killer_length;
		HudColor killer_color;
		wchar_t victim_name[32];
		int victim_length;
		HudColor victim_color;
		tex_t *weapon_icon;
		int weapon_wide;
		int multiKill;
		int flags;
	};

	float m_flNextScrollUp;

	KillMsg m_MsgList[4];

	tex_t *m_pWeaponSkull;

	tex_t *m_pShotHead;
	tex_t *m_pShotGoldHead;
	tex_t *m_pShotWall;
	tex_t *m_pShotWallHead;
	tex_t *m_pShotWallGoldHead;

	tex_t *m_pMultiKill2;
	tex_t *m_pMultiKill3;
	tex_t *m_pMultiKill4;
	tex_t *m_pMultiKill5;
	tex_t *m_pMultiKill6;
	tex_t *m_pMultiKillMax;

	tex_t *m_pFirstKill;
	tex_t *m_pLastKill;
};

//-----------------------------------------------------------------
// UNKDER MARK
//-----------------------------------------------------------------
class CHudUnderMark : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void DrawMark(int x, int y, int type);

	void Add(int iType);
	void Clear(void);

	void MsgFunc_ResetRound(const char *pszName, int iSize, void *pbuf);
	void MsgFunc_KillMsg(const char *pszName, int iSize, void *pbuf);

	int m_iNumMark;
	int m_rgMark[1000];

	tex_t *m_pMarkTexture;
};

//-----------------------------------------------------------------
// SPECTATE
//-----------------------------------------------------------------
class CHudSpectate : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void DrawHPNumber(int n ,int x, int y);

	void PopupInfo(void);
	void HolsterInfo(void);
	bool IsInfoPopup(void);

	void MsgFunc_SpecTarget(const char *pszName, int iSize, void *pbuf);
	void MsgFunc_SpecHealth(const char *pszName, int iSize, void *pbuf);
	void MsgFunc_SpecWeapon(const char *pszName, int iSize, void *pbuf);

	bool m_bActiveUnderUserInfo;
	bool m_bActiveUnderBar;
	bool m_bActiveInformation;

	int m_iSpecTarget;
	int m_iSpecHealth;
	int m_iSpecWeapon;

	wchar_t m_szSpecTargetName[32];
	wchar_t m_szSpecWeaponName[32];

	int m_iSpecTargetNameLength;
	int m_iSpecWeaponNameLength;

	float m_flInfoStartIn;
	float m_flInfoStartOut;

	tex_t *m_pUnderUserInfoGR;
	tex_t *m_pUnderUserInfoBL;

	tex_t *m_pNoClanMark;
	tex_t *m_pDefaultClanMark;

	tex_t *m_pHPNumber;

	tex_t *m_pSpecUnder;
	tex_t *m_pSpecMain;
};

//-----------------------------------------------------------------
// BUY ZONE
//-----------------------------------------------------------------
class CHudBagIcon : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void DrawNumber(int n, int x, int y);

	void MsgFunc_EnableBag(const char *pszName, int iSize, void *pbuf);

	float m_flStartIn;

	bool m_bEnableBag;
	int m_iBagNumber;

	tex_t *m_pBagBG;
	tex_t *m_pBagLine;

	tex_t *m_pNumber;
};

//-----------------------------------------------------------------
// MOUSE SENSITY
//-----------------------------------------------------------------
class CHudSensity : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void DrawNum(int n, int x, int y);

	void Inc(void);
	void Dec(void);

	float m_flDisplayTime;

	int m_iValue;

	tex_t *m_pBG;
	tex_t *m_pGauge;
	tex_t *m_pNum;
};

//-----------------------------------------------------------------
// DAMAGE ARROW
//-----------------------------------------------------------------
class CHudDamageArrow : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void MsgFunc_DmgArrow(const char *pszName, int iSize, void *pbuf);

	bool m_bIsFall;
	Vector m_vecOrigin;
	float m_flStartTime;

	tex_t *m_pArrow;
};

//-----------------------------------------------------------------
// MISSION
//-----------------------------------------------------------------
class CHudMissionLeft : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	bool IsVisible(void);
	void Show(void);
	void Hide(void);

	float m_flInTime;
	float m_flOutTime;

	tex_t *m_pAltKey;
	tex_t *m_pMissionBG;
};

//-----------------------------------------------------------------
// ACE
//-----------------------------------------------------------------
class CHudAceIcon : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void MsgFunc_AceType(const char *pszName, int iSize, void *pbuf);

	int m_iType;

	tex_t *m_pAceIcon;
};

//-----------------------------------------------------------------
// TEAMMATE
//-----------------------------------------------------------------
class CHudTeammateInfo : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void DrawMeter(int n, int x, int y);

	void Toggle(void);

	int m_iType;

	tex_t *m_pMeterNum;
};

//-----------------------------------------------------------------
// KILLER
//-----------------------------------------------------------------
class CHudKillerIcon : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void MsgFunc_KillerIcon(const char *pszName, int iSize, void *pbuf);

	int m_iKillerID;
	float m_flStartTime;

	tex_t *m_pMark50;
	tex_t *m_pMark70;
	tex_t *m_pMark100;
};

//-----------------------------------------------------------------
// CENTER
//-----------------------------------------------------------------
class CHudCenterTips : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void MsgFunc_Center(const char *pszName, int iSize, void *pbuf);

	float m_flStartTime;
	float m_flHoldTime;
	wchar_t m_szText[64];
};

//-----------------------------------------------------------------
// WIN STATUS
//-----------------------------------------------------------------
class CHudWinStatus : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void MsgFunc_WinStatus(const char *pszName, int iSize, void *pbuf);

	float m_flStartTime;
	int m_iHelpType;

	tex_t *m_pSucc;
	tex_t *m_pFail;
	tex_t *m_pDraw;
};

//-----------------------------------------------------------------
// MARK
//-----------------------------------------------------------------
class CHudMark : public CHudBase
{
public:
	void VidInit(void);
	void Redraw(void);

	void MsgFunc_KillMsg(const char *pszName, int iSize, void *pbuf);

	int m_iScript;
	int m_iBadgeLayout;
	int m_iMultiFxLayout;

	tex_t *m_pBadgeHeadshot;
	tex_t *m_pBadgeHeadshotGold;
	tex_t *m_pBadgeHeadshotVVIP;
	tex_t *m_pBadgeHeadshotGoldVVIP;
	tex_t *m_pBadgeKnife0;
	tex_t *m_pBadgeKnife1;
	tex_t *m_pBadgeKnife2;
	tex_t *m_pBadgeKnife3;
	tex_t *m_pBadgeGrenade;
	tex_t *m_pBadgeWallshot;
	tex_t *m_pBadgeHeadWallshot;
	tex_t *m_pBadgeHeadWallshotGold;
	tex_t *m_pBadgeMulti1;
	tex_t *m_pBadgeMulti2;
	tex_t *m_pBadgeMulti3;
	tex_t *m_pBadgeMulti4;
	tex_t *m_pBadgeMulti5;
	tex_t *m_pBadgeMulti6;

	tex_t *m_pMulti2Fx;
	tex_t *m_pMulti3Fx;
	tex_t *m_pMulti4Fx;
	tex_t *m_pMulti5Fx;
	tex_t *m_pMulti6Fx;

	tex_t *m_pUpgrade1;
	tex_t *m_pUpgrade2;
	tex_t *m_pUpgrade3;

	tex_t *m_pBadgeExAssault1;
	tex_t *m_pBadgeExAssault2;
	tex_t *m_pBadgeExAssault3;
	tex_t *m_pBadgeExElite1;
	tex_t *m_pBadgeExElite2;
	tex_t *m_pBadgeExElite3;
	tex_t *m_pBadgeExKnife1;
	tex_t *m_pBadgeExKnife2;
	tex_t *m_pBadgeExKnife3;
	tex_t *m_pBadgeExScout1;
	tex_t *m_pBadgeExScout2;
	tex_t *m_pBadgeExScout3;
	tex_t *m_pBadgeExSniper1;
	tex_t *m_pBadgeExSniper2;
	tex_t *m_pBadgeExSniper3;

	tex_t *m_pBadgeMain;
	tex_t *m_pBadgeEx;
	tex_t *m_pBadgeFx;
	tex_t *m_pBadgeUp;

	float m_flStartTime;
};

//-----------------------------------------------------------------
// FLASHBANG
//-----------------------------------------------------------------
class CHudFlashbangEffect : public CHudBase
{
public:
	void Init(void);
	void Shutdown(void);
	void VidInit(void);
	void Reset(void);
	void Redraw(void);

	void MsgFunc_Flashbang(const char *pszName, int iSize, void *pbuf);

	BYTE *m_ubScreenBits;
	int m_iScreenTexture;
	bool m_bGrabScreen;
	float m_flStartTime;
	float m_flFlashTime;
	float m_flFadeOutTime;
};

//-----------------------------------------------------------------
// MANAGER
//-----------------------------------------------------------------
class CHud
{
public:
	void Init(void);
	void Shutdown(void);
	void VidInit(void);
	void Reset(void);
	void DataInit(void);
	void Think(void);
	void Frame(void);
	void Redraw(void);
	void Save(CBufferWriter &buf);
	void Restore(CBufferReader &buf);

	cvar_t *m_pCvarHudDraw;

	float m_flTime;
	SCREENINFO m_scrinfo;

	// SimSun 12 Bold font.
	int m_iFont;
	int m_iFontOutline;

	// pointer or NULL
	wchar_t *GetPlayerName(int index);

	// calc fade
	int CalcFadePercen(float m_flStartTime, float flInTime, float flHoldTime, float flOutTime, float &flPrecen);

	// world to screen
	bool Project(float *world, float *screen);

	CHudCharacter m_Character;
	CHudHealth m_Health;
	CHudAmmo m_Ammo;
	CHudWeaponSlot m_WeaponSlot;
	CHudCrosshair m_Crosshair;
	CHudSniperScope m_SniperCrosshair;
	CHudRadar m_Radar;
	CHudLocation m_Location;
	CHudKillMsg m_KillMsg;
	CHudUnderMark m_UnderMark;
	CHudGoalScore m_GoalScore;
	CHudNotifyBox m_MsgBox;
	CHudGauge m_Gauge;
	CHudGameTip m_GameTip;
	CHudRadio m_Radio;
	CHudScoreBoard m_ScoreBoard;
	CHudSpectate m_Spectate;
	CHudBagIcon m_Bag;
	CHudTacticalMap m_TacticalMap;
	CHudSensity m_Sensity;
	CHudDamageArrow m_DamageArrow;
	CHudMissionLeft m_SpecMission;
	CHudAceIcon m_Ace;
	CHudTeammateInfo m_Teammate;
	CHudKillerIcon m_KillerMark;
	CHudCenterTips m_Center;
	CHudWinStatus m_WinStatus;
	CHudMark m_Mark;
	CHudFlashbangEffect m_Flashbang;
};

extern CHud gHUD;

#endif