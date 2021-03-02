/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef PLAYER_H
#define PLAYER_H

#include <in_buttons.h>

#define PLAYER_FATAL_FALL_SPEED		(float)1100// approx 60 feet
#define PLAYER_MAX_SAFE_FALL_SPEED	(float)500// approx 20 feet
#define DAMAGE_FOR_FALL_SPEED		(float)100.0 / ( PLAYER_FATAL_FALL_SPEED - PLAYER_MAX_SAFE_FALL_SPEED )// damage per unit per second.
#define PLAYER_MIN_BOUNCE_SPEED		(float)350
#define PLAYER_FALL_PUNCH_THRESHHOLD	(float)250.0 // won't punch player's screen/make scrape noise unless player falling at least this fast.

//
// Player PHYSICS FLAGS bits
//
#define		PFLAG_ONLADDER		( 1<<0 )
#define		PFLAG_ONSWING		( 1<<0 )
#define		PFLAG_ONTRAIN		( 1<<1 )
#define		PFLAG_ONBARNACLE	( 1<<2 )
#define		PFLAG_DUCKING		( 1<<3 )		// In the process of ducking, but totally squatted yet
#define		PFLAG_USING			( 1<<4 )		// Using a continuous entity
#define		PFLAG_OBSERVER		( 1<<5 )		// player is locked in stationary cam mode. Spectators can move, observers can't.

typedef enum
{
	PLAYER_IDLE,
	PLAYER_WALK_LSIDE,		// Move left
	PLAYER_WALK_RSIDE,		// Move right
	PLAYER_WALK,
	PLAYER_WALK_BSIDE,		// Move back
	PLAYER_RUN_LSIDE,		// Run left
	PLAYER_RUN_RSIDE,		// Run right
	PLAYER_RUN,
	PLAYER_RUN_BSIDE,		// Run back
	PLAYER_JUMP,
	PLAYER_DIE,
	PLAYER_RELOAD,
	PLAYER_ATTACK1,
	PLAYER_ATTACK2,
	PLAYER_ATTACK3,
} PLAYER_ANIM;

#define HITGROUP_GENERIC	0	// 其它
#define HITGROUP_HEAD		1	// 头部
#define HITGROUP_CHEST		2	// 胸部
#define HITGROUP_STOMACH	3	// 腹部
#define HITGROUP_LEFTARM	4	// 左臂
#define HITGROUP_RIGHTARM	5	// 右臂
#define HITGROUP_LEFTLEG	6	// 左腿
#define HITGROUP_RIGHTLEG	7	// 右腿

#define VIEW_FIELD_FULL		-1.0
#define VIEW_FIELD_WIDE		-0.7
#define VIEW_FIELD_NARROW	0.7
#define VIEW_FIELD_ULTRA_NARROW		0.9

#define MAX_WEAPON_TYPES	5

enum JoinState
{
	/* 客户端已成功加入队伍（作为玩家或观察者） */
	JOINED,

	/* 客户端已成功进入服务器（用于通知客户端） */
	PUTINSERVER,

	/* 正在等待客户端提交命令 */
	WAITINGCLIENT,

	/* 客户端尝试加入队伍（加入失败将会返回 WAITINGCLIENT 状态） */
	GETINTOGAME,
};

class CBasePlayer : public CBaseAnimating
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual int ObjectCaps(void) { return 0; }
	virtual void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector &vecDir, TraceResult *ptr, int bitsDamageType);
	virtual void TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	virtual void Killed(entvars_t *pevInflictor, entvars_t *pevAttacker);

	virtual bool IsAlive(void) const { return (pev->deadflag == DEAD_NO) && (pev->health > 0); }
	virtual bool IsPlayer(void) const { return true; }
	virtual bool IsNetClient(void) const { return true; }
	virtual bool IsBot(void) const { return false; }

public:
	int GetId(void);
	void ResetMaxSpeed(void);
	void PreThink(void);
	void PostThink(void);
	void Jump(void);
	void Duck(void);
	void DropPunchAngle(void);
	void UpdateClientData(void);
	void ImpulseCommands(void);
	Vector GetGunPosition(void);
	Vector GetAimVector(void);
	bool FInViewCone(CBaseEntity *pEntity);
	bool FInViewCone(const Vector &vecSpot);
	void EyeVectors(Vector *pForward = NULL, Vector *pRight = NULL, Vector *pUp = NULL);
	int GetTeamNumber(void) const;
	void SnapEyeAngles(const Vector &viewAngles);
	Vector &EyeAngles(void) const { return pev->v_angle; }
	const char *GetPlayerName(void) const;
	void SetCharacter(int index);
	bool IsAutoFollowAllowed(void) const { return (gpGlobals->time > m_allowAutoFollowTime); }
	void InhibitAutoFollow(float duration) { m_allowAutoFollowTime = gpGlobals->time + duration; }
	void AllowAutoFollow(void) { m_allowAutoFollowTime = 0; }
	BOOL AddPlayerWeapon(CBasePlayerWeapon *pWeapon);
	void RemovePlayerWeapon(CBasePlayerWeapon *pWeapon);
	BOOL GiveWeapon(int index, BOOL bSelect = FALSE);
	BOOL HasWeapons(void);
	void RemoveAllWeapons(void);
	void RemoveWeaponSlot(int iSlot);
	void SelectWeapon(CBasePlayerWeapon *pWeapon);
	void SelectSlot(int iSlot);
	void SelectNextWeapon(BOOL bReverse);
	void SelectLastWeapon(void);
	void RetireWeapon(void);
	void DropPlayerWeapon(void);
	BOOL HasPlayerWeapon(int index);
	CBasePlayerWeapon *GetActiveWeapon(void) const;
	int GetActiveWeaponId(void);
	BOOL IsOnLadder(void);
	int GetCharacterId(void);
	int GetAnimationSequence(const char *activity, const char *weapon);
	void SetAnimation(PLAYER_ANIM playerAnim);
	void Spectator_SetMode(int iMode);
	BOOL IsObserver(void);
	BOOL IsObservingPlayer(CBasePlayer *pTarget);
	void EnableControl(BOOL fControl);
	int GetFOV(void);
	void SetFOV(int fov, float time = 0);
	void Radio(int group, int num);
	void MessageAudio(const char *sentence, BOOL bAnyone);
	Vector FireBullets(Vector &vecSrc, Vector &vecDirShooting, float flSpread, float flDistance, int iPenetration, int iPenetrationPower, int flPenetrationDistance, int iDamage, float flRangeModifier, entvars_t *pevAttacker, int shared_rand);
	void FireShotgun(int cShots, Vector &vecSrc, Vector &vecDirShooting, float flSpread, float flDistance, int iDamage, float flRangeModifier, entvars_t *pevAttacker, int shared_rand);
	int KnifeAttack(float flRange, float flAngle, float flDamage);
	void SetGodMode(BOOL bState, float flHold);
	void SetRespawn(float flTime);
	void BagRebuy(void);
	void HandleSignals(void);
	void SetACE(int type);
	bool InSameTeam(CBasePlayer *player) const;
	bool IsDucked(void) const { return FBitSet( pev->flags, FL_DUCKING ); }
	bool IsReloading(void) const;
	float GetHealth(void) const { return pev->health; }
	bool IsBlind(void) const { return false; }
	void Blind(float holdTime, float fadeTime, float startingAlpha) {}
	int GetFOV(void) const { return m_iFOV; }
	int GetDefaultFOV(void) const { return 90; }
	bool HasC4(void) const { return false; }

private:
	void DeathSound(void);
	void StartSpectator(Vector &vecPosition, Vector &vecViewAngle);

	CBaseEntity *	Spectator_IsValidTarget(int iTarget);
	void			Spectator_FindNextPlayer(bool bReverse);
	void			Spectator_HandleButtons(void);
	void			Spectator_CheckTarget(void);
	void			Spectator_CheckProperties(void);

	void JoiningThink(void);
	void DeathThink(void);
	void ItemPreFrame(void);
	void ItemPostFrame(void);
	void PlayerUse(void);
	void PackDeadPlayerItems(void);
	void StudioProcessGait(void);

public:
	int random_seed;
	int m_iCharacterId;
	char m_szAnimExtention[32];
	float m_flVelocityModifier;
	float m_flFallVelocity;

	int m_iJoinToTeam;	// 队伍
	int m_iJoinToCharacter;	// 角色ID
	int m_iJoinToObserver;	// 观战

	BOOL m_bIsObserver;	// 观战者 (作为观察者加入游戏的那种)
	BOOL m_bIsSpectator;	// 观察者

	EHANDLE m_hSpectatorTarget;
	float m_fSpectatorNextInput;
	int m_iSpectatorLastMode;

	int m_iSpectatorTarget;
	int m_iSpectatorHealth;
	int m_iSpectatorFOV;
	int m_iSpectatorWeapon;

	BOOL m_fJustConnected;
	BOOL m_bDisconnected;
	int m_iJoiningState;
	int m_afButtonLast;
	int m_afButtonPressed;
	int m_afButtonReleased;
	edict_t *m_pentSndLast;
	float m_flSndRoomtype;
	float m_flSndRange;
	CBasePlayerWeapon *m_rgpPlayerWeapons[MAX_WEAPON_TYPES];
	CBasePlayerWeapon *m_pActiveWeapon;
	CBasePlayerWeapon *m_pLastWeapon;
	BOOL m_fGameHUDInitialized;
	BOOL m_fInitHUD;
	int m_iFOV;
	float m_fFOVChangeTime;
	int m_iNumSpawns;
	float m_flNextAttack;
	int m_LastHitGroup;
	int m_bitsHUDDamage;
	int m_bitsDamageType;
	Vector m_vecDamageOrigin;
	float m_flDeadTime;
	float m_flNextRadio;
	int m_iClass;
	int m_iTeam;
	int m_iKills;
	int m_iDeads;
	int m_iScoreAttrib;
	int m_iClientFOV;
	int m_iClientHealth;
	int m_iClientArmor;
	int m_iClientWeaponId;
	int m_iClientClip;
	int m_iClientAmmo;
	float m_flGodModeTime;
	float m_flSpawnTime;
	float m_tmHandleSignals;
	BOOL m_bEnableBag;
	int m_iAceType;

	CBaseEntity *m_pAttacker;
	CBaseEntity *m_pInflictor;

	float m_flLastKillTime;
	int m_iMultiKillCount;
	int m_iRevengeTarget;

	float m_blindUntilTime;
	float m_blindStartTime;
	float m_blindHoldTime;
	float m_blindFadeTime;
	float m_blindAlpha;

	float m_allowAutoFollowTime;
};

int GetPlayerGaitsequence( const edict_t *pEdict );

//#define TEAM_NO 0
//#define TEAM_GR 1
//#define TEAM_BL 2

#endif