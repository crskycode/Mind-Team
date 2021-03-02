#ifndef SETTING_H
#define SETTING_H

#pragma warning(disable:4482)

typedef struct
{
	int	width;
	int height;
	int bpp;
	int window;
}
VIDEOMODE;

typedef struct
{
	char name[16];
}
KEYENT;

enum SWITCH
{
	OFF = 0,
	ON,
};

enum QUALMODE
{
	LOW = 0,
	MID,
	HIGH,
};

typedef struct
{
	//Graphic
	VIDEOMODE		VideoMode;
	QUALMODE		Mapping;
	QUALMODE		BulletMark;
	QUALMODE		BulletSmoke;
	SWITCH			Blood;
	SWITCH			Bullet;
	SWITCH			Enviroment;
	int				Gamma;		//{0,100}
	//Control
	KEYENT			CtrlFront;
	KEYENT			CtrlBack;
	KEYENT			CtrlLeft;
	KEYENT			CtrlRight;
	KEYENT			CtrlDuck;
	KEYENT			CtrlWalk;
	KEYENT			CtrlJump;
	KEYENT			CtrlFire;
	KEYENT			CtrlSpecial;
	KEYENT			CtrlReload;
	KEYENT			CtrlDrop;
	KEYENT			CtrlUse;
	KEYENT			CtrlBag;
	KEYENT			CtrlSlot;
	KEYENT			CtrlPrimary;
	KEYENT			CtrlSecond;
	KEYENT			CtrlKnife;
	KEYENT			CtrlGrenade;
	KEYENT			CtrlC4;
	int				Sense;		//{0,100}
	int				ZoomSense;		//{0,100}
	//Sound
	SWITCH			BGM;
	int				BGMVol;	//{0,100}
	SWITCH			Effect;
	int				EffectVol;		//{0,100}
	SWITCH			Radio;
	int				RadioVol;	//{0,100}
	//Community
	KEYENT			CtrlRadio1;
	KEYENT			CtrlRadio2;
	KEYENT			CtrlRadio3;
	wchar_t			Macro1[100];
	wchar_t			Macro2[100];
	wchar_t			Macro3[100];
	wchar_t			Macro4[100];
	SWITCH			RejInv;
	SWITCH			RejWhisper;
	SWITCH			BlockInviteFriend;
	int				ShowAchieve;	// {alterably,badge,achieve}
	SWITCH			ShowChatAchieve;
	SWITCH			ShowRankMedal;
	SWITCH			ShowClanRankMedal;
	//Etc
	SWITCH			LeftHand;
	SWITCH			InverseMouse;
	int				CrossColor;		//{0,2}
	int				CrossType;	//{0,2}
	SWITCH			ReplaySave;
	int				LoadingScreenType;	//{0,1}
}
SETTING;

#endif