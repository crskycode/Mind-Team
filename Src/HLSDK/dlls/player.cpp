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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "in_buttons.h"
#include "shake.h"
#include "gamerule.h"

extern "C"
{
#include <pm_shared.h>
#include <pm_hull.h>
}

#include "CharacterData.h"
#include "WeaponData.h"
#include "RadioData.h"
#include "MsgAudioData.h"


// From world.cpp
void CopyToBodyQue(entvars_t* pev);

int gmsgArmor = 0;
int gmsgEnableBag = 0;
int gmsgBagRebuy = 0;
int gmsgCharacter = 0;
int gmsgClassInfo = 0;
int gmsgConnected = 0;
int gmsgCurWeapon = 0;
int gmsgDmgArrow = 0;
int gmsgKillerIcon = 0;
int gmsgKillMsg = 0;
int gmsgFade = 0;
int gmsgFlashbang = 0;
int gmsgGameMode = 0;
int gmsgGameTip = 0;
int gmsgGameOver = 0;
int gmsgGauge = 0;
int gmsgHealth = 0;
int gmsgInitHUD = 0;
int gmsgMsgAudio = 0;
int gmsgMsgBox = 0;
int gmsgRadio = 0;
int gmsgResetHUD = 0;
int gmsgResetRound = 0;
int gmsgRoundEnd = 0;
int gmsgRoundTime = 0;
int gmsgScoreAttrib = 0;
int gmsgScoreInfo = 0;
int gmsgAceType = 0;
int gmsgSetFOV = 0;
int gmsgShake = 0;
int gmsgSpecTarget = 0;
int gmsgSpecHealth = 0;
int gmsgSpecWeapon = 0;
int gmsgTeamInfo = 0;
int gmsgTeamScore = 0;
int gmsgTextMsg = 0;
int gmsgWeapSlot = 0;
int gmsgWeapSlotA = 0;
int gmsgWinStatus = 0;


void LinkUserMessages(void)
{
	if (gmsgArmor)
		return;

	gmsgArmor = REG_USER_MSG("Armor", 2);
	gmsgEnableBag = REG_USER_MSG("EnableBag", 1);
	gmsgBagRebuy = REG_USER_MSG("BagRebuy", 0);
	gmsgCharacter = REG_USER_MSG("Character", 2);
	gmsgClassInfo = REG_USER_MSG("ClassInfo", 2);
	gmsgConnected = REG_USER_MSG("Connected", 0);
	gmsgCurWeapon = REG_USER_MSG("CurWeapon", 6);
	gmsgDmgArrow = REG_USER_MSG("DmgArrow", 7);	// BYTE COORD COORD COORD
	gmsgKillerIcon = REG_USER_MSG("KillerIcon", 1);
	gmsgKillMsg = REG_USER_MSG("KillMsg", 7);
	gmsgFade = REG_USER_MSG("ScreenFade", sizeof(ScreenFade));
	gmsgFlashbang = REG_USER_MSG("Flashbang", 2);
	gmsgGameMode = REG_USER_MSG("GameMode", 2);
	gmsgGameTip = REG_USER_MSG("GameTip", -1);
	gmsgGameOver = REG_USER_MSG("GameOver", 0);
	gmsgGauge = REG_USER_MSG("Gauge", 2);
	gmsgHealth = REG_USER_MSG("Health", 2);
	gmsgInitHUD = REG_USER_MSG("InitHUD", 0);
	gmsgMsgAudio = REG_USER_MSG("MsgAudio", -1);
	gmsgMsgBox = REG_USER_MSG("MsgBox", -1);
	gmsgRadio = REG_USER_MSG("Radio", 7);
	gmsgResetHUD = REG_USER_MSG("ResetHUD", 0);
	gmsgResetRound = REG_USER_MSG("ResetRound", 0);	//84
	gmsgRoundEnd = REG_USER_MSG("RoundEnd", 1);
	gmsgRoundTime = REG_USER_MSG("RoundTime", 2);	//86
	gmsgScoreAttrib = REG_USER_MSG("ScoreAttrib", 2);
	gmsgScoreInfo = REG_USER_MSG("ScoreInfo", 5);
	gmsgAceType = REG_USER_MSG("AceType", 2);
	gmsgSetFOV = REG_USER_MSG("SetFOV", 3);
	gmsgShake = REG_USER_MSG("ScreenShake", sizeof(ScreenShake));
	gmsgSpecTarget = REG_USER_MSG("SpecTarget", 1);
	gmsgSpecHealth = REG_USER_MSG("SpecHealth", 2);
	gmsgSpecWeapon = REG_USER_MSG("SpecWeapon", 2);
	gmsgTeamInfo = REG_USER_MSG("TeamInfo", 2);
	gmsgTeamScore = REG_USER_MSG("TeamScore", 6);	//96
	gmsgTextMsg = REG_USER_MSG("TextMsg", -1);
	gmsgWeapSlot = REG_USER_MSG("WeapSlot", 3);
	gmsgWeapSlotA = REG_USER_MSG("WeapSlotA", 2);
	gmsgWinStatus = REG_USER_MSG("WinStatus", 1);	// BYTE
}

LINK_ENTITY_TO_CLASS( player, CBasePlayer );

int CBasePlayer::GetId(void)
{
	return g_engfuncs.pfnIndexOfEdict(pev->pContainingEntity);
}

bool CBasePlayer::FInViewCone(CBaseEntity *pEntity)
{
	return FInViewCone( pEntity->Center() );
}

bool CBasePlayer::FInViewCone(const Vector &vecSpot)
{
	Vector los = ( vecSpot - EyePosition() );

	// do this in 2D
	los.z = 0;
	VectorNormalize( los );

	UTIL_MakeVectors(pev->v_angle);
	Vector facingDir = gpGlobals->v_forward.Make2D();

	float flDot = DotProduct( los, facingDir );

	if ( flDot > 0.5f )
		return true;

	return false;
}

void CBasePlayer::EyeVectors(Vector *pForward, Vector *pRight, Vector *pUp)
{
	ANGLE_VECTORS(pev->v_angle, (float *)pForward, (float *)pRight, (float *)pUp);
}

int CBasePlayer::GetTeamNumber(void) const
{
	return m_iTeam;
}

void CBasePlayer::SnapEyeAngles(const Vector &viewAngles)
{
	pev->angles = viewAngles;
	pev->v_angle = viewAngles;
	pev->fixangle = TRUE;
}

const char *CBasePlayer::GetPlayerName(void) const
{
	return STRING( pev->netname );
}

void CBasePlayer::SetCharacter(int index)
{
	if (!index)
		return;

	CharacterData *pData = CharacterData_GetData(index);

	if (!pData)
		return;

	m_iCharacterId = pData->iCharacterIndex;

	switch (m_iTeam)
	{
		case TEAM_GR:
		{
			pev->modelindex = MODEL_INDEX(STRING(pData->iszGRModelFile));
			break;
		}
		case TEAM_BL:
		{
			pev->modelindex = MODEL_INDEX(STRING(pData->iszBLModelFile));
			break;
		}
	}

	MESSAGE_BEGIN(MSG_ONE, gmsgCharacter, NULL, ENT(pev));
	WRITE_SHORT(m_iCharacterId);
	MESSAGE_END();
}

void CBasePlayer::DeathSound(void)
{
	/*switch (RANDOM_LONG(1, 5))
	{
	case 1:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_pain5.wav", 1, ATTN_NORM);
		break;
	case 2:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_pain6.wav", 1, ATTN_NORM);
		break;
	case 3:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_pain7.wav", 1, ATTN_NORM);
		break;
	}*/
}

Vector CBasePlayer::GetGunPosition(void)
{
	return pev->origin + pev->view_ofs;
}

Vector CBasePlayer::GetAimVector( void )
{
	MAKE_VECTORS(pev->v_angle);
	return gpGlobals->v_forward;
}

void CBasePlayer::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector &vecDir, TraceResult *ptr, int bitsDamageType)
{
	if (!pev->takedamage)
		return;

	m_LastHitGroup = ptr->iHitgroup;

	switch (ptr->iHitgroup)
	{
	case HITGROUP_GENERIC:
		break;

	case HITGROUP_HEAD:
		break;

	case HITGROUP_CHEST:
		flDamage *= 0.6;
		break;

	case HITGROUP_STOMACH:
		flDamage *= 0.5;
		break;

	case HITGROUP_LEFTARM:
		flDamage *= 0.33;
		break;

	case HITGROUP_RIGHTARM:
		flDamage *= 0.33;
		break;

	case HITGROUP_LEFTLEG:
		flDamage *= 0.33;
		break;

	case HITGROUP_RIGHTLEG:
		flDamage *= 0.33;
		break;
	}

	AddMultiDamage(pevAttacker, this, flDamage, bitsDamageType);
}

void CBasePlayer::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	if (!pev->takedamage)
		return;

	pev->health -= flDamage;

	m_bitsDamageType = bitsDamageType;
	m_bitsHUDDamage = -1;

	CBaseEntity *pKiller = CBaseEntity::Instance( pevInflictor );

	if (pKiller->IsPlayer())
	{
		m_vecDamageOrigin = dynamic_cast<CBasePlayer *>(pKiller)->GetGunPosition();
	}
	else
	{
		m_vecDamageOrigin = pKiller->pev->origin;
	}

	if (pev->health <= 0.0)
	{
		Killed(pevInflictor, pevAttacker);
	}
}

void CBasePlayer::PackDeadPlayerItems(void)
{
	if (HasWeapons())
	{
		RemoveAllWeapons();
	}
}

BOOL CBasePlayer::HasWeapons(void)
{
	for (int i = 0; i < MAX_WEAPON_TYPES; i++)
	{
		if (m_rgpPlayerWeapons[i])
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CBasePlayer::RemoveAllWeapons(void)
{
	SelectWeapon(NULL);

	m_pLastWeapon = NULL;

	for (int i = 0; i < MAX_WEAPON_TYPES; i++)
	{
		CBasePlayerWeapon *&pIter = m_rgpPlayerWeapons[i];

		while (pIter)
		{
			pIter->Kill();
			RemovePlayerWeapon(pIter);
		}
	}
}

void CBasePlayer::RemoveWeaponSlot(int iSlot)
{
	CBasePlayerWeapon *&pIter = m_rgpPlayerWeapons[iSlot];

	while (pIter)
	{
		if (pIter == m_pActiveWeapon)
		{
			SelectWeapon(NULL);
		}

		pIter->Kill();

		RemovePlayerWeapon(pIter);
	}

	RetireWeapon();
}

void CBasePlayer::Killed(entvars_t *pevInflictor, entvars_t *pevAttacker)
{
	assert(pevInflictor);
	assert(pevAttacker);

	if (m_pActiveWeapon)
		m_pActiveWeapon->Holster();
	
	SetAnimation(PLAYER_DIE);
	
	pev->deadflag = DEAD_DYING;
	pev->health = 0;
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->flags &= ~FL_ONGROUND;
	
	pev->iuser1 = OBS_IN_EYE;
	pev->iuser2 = ENTINDEX(pev);
	pev->iuser3 = 0;

	//m_pSpectatorTarget = UTIL_PlayerByIndex(pev->iuser3);

	m_iClientHealth = 0;
	
	MESSAGE_BEGIN(MSG_ONE, gmsgHealth, NULL, ENT(pev));
	WRITE_SHORT(m_iClientHealth);
	MESSAGE_END();
	
	//MESSAGE_BEGIN(MSG_ONE, gmsgCurWeapon, NULL, ENT(pev));
	//WRITE_SHORT(0);
	//WRITE_SHORT(0);
	//WRITE_SHORT(0);
	//MESSAGE_END();
	
	pev->velocity = g_vecZero;
	
	SetFOV(90);
	
	DeathSound();
	
	//pev->angles.x = 0;
	//pev->angles.z = 0;

	int dir = 0;

	Vector vecAttackDir = ( pevAttacker->origin - pev->origin ).Normalize();

	Vector angSeeToKiller;

	VEC_TO_ANGLES( vecAttackDir, angSeeToKiller );

	Vector vecForward, vecRight, vecUp;

	g_engfuncs.pfnAngleVectors( pev->angles, vecForward, vecRight, vecUp );

	//UTIL_DrawBeamPoints( pev->origin, pev->origin + (vecAttackDir * 150), 50, 255, 255, 255 );

	//UTIL_DrawBeamPoints( pev->origin, pev->origin + (vecForward * 50), 50, 255, 0, 0 );
	//UTIL_DrawBeamPoints( pev->origin, pev->origin + (vecRight * 50), 50, 0, 255, 0 );
	//UTIL_DrawBeamPoints( pev->origin, pev->origin + (vecUp * 50), 50, 0, 0, 255 );

	float ca = cos( 45.0 * M_PI / 180.0 );

	// Is attack from front ?
	if ( DotProduct( vecAttackDir, vecForward ) > ca )
	{
		dir = 1;
	}

	// Is attack from back ?
	else if ( DotProduct( vecAttackDir, -vecForward ) > ca )
	{
		dir = 2;
	}

	// Is attack from right ?
	else if ( DotProduct( vecAttackDir, vecRight ) > ca )
	{
		dir = 3;
	}

	// Is attack from left ?
	else if ( DotProduct(vecAttackDir, -vecRight ) > ca )
	{
		dir = 4;
	}

	pev->gaitsequence = 0;
	switch (dir)
	{
	case 1: pev->sequence = LookupSequence("M-ThrowAxeBdown"); break;
	case 2: pev->sequence = LookupSequence("M-ThrowAxeFdown"); break;
	case 3: pev->sequence = LookupSequence("M-ThrowAxeLdown"); break;
	case 4: pev->sequence = LookupSequence("M-ThrowAxeRdown"); break;

	default:
		pev->sequence = LookupSequence("M-ThrowAxeBdown"); break;
	}
	pev->animtime = gpGlobals->time;
	pev->frame = 0;
	pev->framerate = 1;

	m_flDeadTime = gpGlobals->time;

	// 如果把角度朝向KILLER模型的方向也会改变, 看起来玩家倒向错误的方向
	//pev->angles = angSeeToKiller;
	//pev->v_angle = angSeeToKiller;

	// Use in GameRule
	m_pAttacker = CBaseEntity::Instance(pevAttacker);
	m_pInflictor = CBaseEntity::Instance(pevInflictor);

	g_pGameRule->PlayerKill(this);
}

int CBasePlayer::GetAnimationSequence(const char *activity, const char *weapon)
{
	char	label[32], label_ref[32];
	int		index;

	strcpy(label, "M-");

	if (pev->flags & FL_DUCKING)
	{
		strcat(label, "c-");
	}

	strcat(label, activity);
	strcat(label, "-");
	strcpy(label_ref, label);

	if (*weapon)
	{
		strcat(label, weapon);
	}

	index = LookupSequence(label);

	if (index == -1)
	{
		strcat(label_ref, "knife");

		index = LookupSequence(label_ref);

		if (index == -1)
		{
			index = 0;
		}
	}

	return index;
}

void CBasePlayer::SetAnimation(PLAYER_ANIM playerAnim)
{
	int animDesired = -1;
	char szAnim[32];

	switch (playerAnim)
	{
		case PLAYER_IDLE:
		{
			animDesired  = GetAnimationSequence("idle", m_szAnimExtention);
			break;
		}
		case PLAYER_WALK_LSIDE:
		{
			animDesired  = GetAnimationSequence("walk-Lside", m_szAnimExtention);
			break;
		}
		case PLAYER_WALK_RSIDE:
		{
			animDesired  = GetAnimationSequence("walk-Rside", m_szAnimExtention);
			break;
		}
		case PLAYER_WALK:
		{
			animDesired  = GetAnimationSequence("walk", m_szAnimExtention);
			break;
		}
		case PLAYER_WALK_BSIDE:
		{
			animDesired  = GetAnimationSequence("walk-Bside", m_szAnimExtention);
			break;
		}
		case PLAYER_RUN_LSIDE:
		{
			animDesired  = GetAnimationSequence("run-Lside", m_szAnimExtention);
			break;
		}
		case PLAYER_RUN_RSIDE:
		{
			animDesired  = GetAnimationSequence("run-Rside", m_szAnimExtention);
			break;
		}
		case PLAYER_RUN:
		{
			animDesired  = GetAnimationSequence("run", m_szAnimExtention);
			break;
		}
		case PLAYER_RUN_BSIDE:
		{
			animDesired  = GetAnimationSequence("run-Bside", m_szAnimExtention);
			break;
		}
		case PLAYER_JUMP:
		{
			animDesired = LookupSequence("M-jump");
			break;
		}
	}

	if (animDesired != -1)
	{
		pev->sequence = animDesired;
		pev->gaitsequence = animDesired;
		ResetSequenceInfo();
		return;
	}
}

BOOL CBasePlayer::IsOnLadder(void)
{
	return (pev->movetype == MOVETYPE_FLY);
}

int CBasePlayer::GetCharacterId(void)
{
	return m_iCharacterId;
}

void CBasePlayer::DeathThink(void)
{
	if (m_iJoiningState != JOINED)
		return;

	PackDeadPlayerItems();
	
	if (pev->modelindex && gpGlobals->time - m_flDeadTime < 3)
	{
		StudioFrameAdvance();
		return;
	}

	pev->deadflag = DEAD_DEAD;

	pev->framerate = 0;

	if (!m_bIsSpectator)
	{
		StartSpectator(pev->origin, pev->angles);
	}
}

int CBasePlayer::GetFOV(void)
{
	return m_iFOV;
}

void CBasePlayer::SetFOV(int fov, float time)
{
	m_iFOV = pev->fov = fov;
	m_fFOVChangeTime = time;

	MESSAGE_BEGIN(MSG_ONE, gmsgSetFOV, NULL, ENT(pev));
	WRITE_BYTE(m_iFOV);
	WRITE_SHORT(m_fFOVChangeTime * 1000);
	MESSAGE_END();
}

void CBasePlayer::Radio(int group, int sentence)
{
	if (!IsAlive())
		return;

	if (gpGlobals->time < m_flNextRadio)
		return;

	m_flNextRadio = gpGlobals->time + 0.5;

	CharacterData *pCharacter = CharacterData_GetData(m_iCharacterId);

	if ( !pCharacter )
		return;

	const char *pSound;
	int iRand;

	RadioData_GetFile(m_iTeam, pCharacter->bIsWoman, pCharacter->bIsChinese, group, sentence, pSound, iRand);

	if ( !pSound )
		return;

	for (int iPlayer = 1; iPlayer <= gpGlobals->maxClients; iPlayer++)
	{
		CBasePlayer *pl = (CBasePlayer *)UTIL_PlayerByIndex(iPlayer);

		if ( !pl )
			continue;

		if (pl->m_iTeam != m_iTeam)
			continue;

		MESSAGE_BEGIN(MSG_ONE_UNRELIABLE, gmsgRadio, NULL, pl->edict());
		WRITE_BYTE(entindex());
		WRITE_BYTE(m_iTeam);
		WRITE_SHORT(m_iCharacterId);
		WRITE_BYTE(group);
		WRITE_BYTE(sentence);
		WRITE_BYTE(iRand);
		MESSAGE_END();
	}
}

void CBasePlayer::MessageAudio(const char *sentence, BOOL bAnyone)
{
	if (!sentence || !*sentence)
		return;

	CharacterData *pCharacter = CharacterData_GetData(m_iCharacterId);

	if ( !pCharacter )
		return;

	const char *pSound;
	int iRand;

	MsgAudio_GetFile(m_iTeam, pCharacter->bIsWoman, pCharacter->bIsChinese, sentence, pSound, iRand);

	if ( !pSound )
		return;

	for (int iPlayer = 1; iPlayer <= gpGlobals->maxClients; iPlayer++)
	{
		CBasePlayer *pl = (CBasePlayer *)UTIL_PlayerByIndex(iPlayer);

		if ( !pl )
			continue;

		if (pl->m_iTeam != m_iTeam && !bAnyone)
			continue;

		MESSAGE_BEGIN(MSG_ONE_UNRELIABLE, gmsgMsgAudio, NULL, pl->edict());
		WRITE_BYTE(entindex());
		WRITE_BYTE(m_iTeam);
		WRITE_SHORT(m_iCharacterId);
		WRITE_STRING(sentence);
		WRITE_BYTE(iRand);
		MESSAGE_END();
	}
}

void CBasePlayer::StartSpectator(Vector &vecPosition, Vector &vecViewAngle)
{
	m_bIsSpectator = TRUE;

	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->flags = FL_CLIENT | FL_SPECTATOR;
	pev->effects = EF_NODRAW;
	pev->view_ofs = g_vecZero;
	pev->deadflag = DEAD_DEAD;
	pev->health = 1;
	pev->takedamage = DAMAGE_NO;

	SetFOV(90);

	SET_ORIGIN(edict(), vecPosition);

	pev->punchangle = g_vecZero;
	pev->angles = pev->v_angle = vecViewAngle;
	pev->fixangle = TRUE;

	pev->iuser1 = 0;
	pev->iuser2 = 0;
	pev->iuser3 = 0;

	Spectator_SetMode(OBS_CHASE_FREE);

	g_engfuncs.pfnSetClientMaxspeed(edict(), 900);
}


#define PLAYER_SEARCH_RADIUS 64.0f


void CBasePlayer::PlayerUse(void)
{
	if (!FBitSet(pev->button | m_afButtonPressed | m_afButtonReleased, IN_USE))
		return;
	
	CBaseEntity *pObject = NULL;
	CBaseEntity *pClosest = NULL;
	float flMaxDot = VIEW_FIELD_NARROW;

	UTIL_MakeVectors(pev->v_angle);

	while ((pObject = UTIL_FindEntityInSphere(pObject, pev->origin, PLAYER_SEARCH_RADIUS)) != NULL)
	{
		if (pObject->ObjectCaps() & (FCAP_IMPULSE_USE | FCAP_CONTINUOUS_USE | FCAP_ONOFF_USE))
		{
			Vector vecLOS = VecBModelOrigin(pObject->pev) - (pev->origin + pev->view_ofs);
			vecLOS = UTIL_ClampVectorToBox(vecLOS, pObject->pev->size * 0.5);
			float flDot = DotProduct(vecLOS, gpGlobals->v_forward);

			if (flDot > flMaxDot)
			{
				flMaxDot = flDot;
				pClosest = pObject;
			}
		}
	}

	pObject = pClosest;

	if (pObject)
	{
		int caps = pObject->ObjectCaps();

		if (((pev->button & IN_USE) && (caps & FCAP_CONTINUOUS_USE)) || ((m_afButtonPressed & IN_USE) && (caps & (FCAP_IMPULSE_USE | FCAP_ONOFF_USE))))
		{
			pObject->Use(this, this, USE_SET, 1);
			return;
		}

		if ((m_afButtonReleased & IN_USE) && (pObject->ObjectCaps() & FCAP_ONOFF_USE))
			pObject->Use(this, this, USE_SET, 0);
	}
	else if (m_afButtonPressed & IN_USE)
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "common/ui_notify.wav", 0.4, ATTN_NORM);
}

void CBasePlayer::ResetMaxSpeed( void )
{
	float speed = 240;

	if (m_pActiveWeapon)
		speed = m_pActiveWeapon->GetMaxSpeed();

	g_engfuncs.pfnSetClientMaxspeed(edict(), speed);
}

void CBasePlayer::Jump()
{
	if (FBitSet(pev->flags, FL_WATERJUMP))
		return;
	
	if (pev->waterlevel >= 2)
		return;
	
	if (!FBitSet(m_afButtonPressed, IN_JUMP))
		return;
	
	if (!FBitSet(pev->flags, FL_ONGROUND) || !pev->groundentity)
		return;
	
	SetAnimation(PLAYER_JUMP);
	
	entvars_t *pevGround = VARS(pev->groundentity);
	
	if (pevGround && FBitSet(pevGround->flags, FL_CONVEYOR))
	{
		pev->velocity = pev->velocity + pev->basevelocity;
	}
}

void CBasePlayer::Duck(void)
{
	if (FBitSet(pev->button, IN_DUCK))
	{
	}
}

void CBasePlayer::PreThink(void)
{
	int buttonsChanged = (m_afButtonLast ^ pev->button);

	m_afButtonPressed = buttonsChanged & pev->button;
	m_afButtonReleased = buttonsChanged & (~pev->button);

	if (m_iJoiningState != JOINED)
		JoiningThink();

	DropPunchAngle();
	pev->maxspeed = 200;

	ItemPreFrame();

	if (pev->flags & FL_ONGROUND)
	{
		if (m_flVelocityModifier < 1)
		{
			m_flVelocityModifier += 0.01;
			pev->velocity = pev->velocity * m_flVelocityModifier;
		}

		if (m_flVelocityModifier > 1)
			m_flVelocityModifier = 1;
	}

	UpdateClientData();

	if (gpGlobals->time > m_tmHandleSignals)
	{
		HandleSignals();
		m_tmHandleSignals = gpGlobals->time + 0.5;
	}

	if (m_bIsSpectator)
	{
		Spectator_HandleButtons();
		Spectator_CheckTarget();
		Spectator_CheckProperties();
		return;
	}

	if (pev->deadflag != DEAD_NO)
	{
		DeathThink();
		return;
	}

	if (FBitSet(pev->button, IN_JUMP))
		Jump();

	if (FBitSet(pev->button, IN_DUCK) || FBitSet(pev->flags, FL_DUCKING))
		Duck();

	if (!FBitSet(pev->flags, FL_ONGROUND))
	{
		m_flFallVelocity = -pev->velocity.z;
	}
}

void CBasePlayer::PostThink(void)
{	
	if (IsAlive())
	{
		StudioFrameAdvance();
		StudioProcessGait();

		m_flNextAttack = max(m_flNextAttack - gpGlobals->frametime, -1);

		PlayerUse();
		ImpulseCommands();
		ItemPostFrame();

		if (FBitSet(pev->flags, FL_ONGROUND) && m_flFallVelocity >= PLAYER_FALL_PUNCH_THRESHHOLD)
		{
			if (pev->watertype == CONTENT_WATER)
			{
			}
			else if (m_flFallVelocity > PLAYER_MAX_SAFE_FALL_SPEED)
			{
				float flFallDamage = (m_flFallVelocity - PLAYER_MAX_SAFE_FALL_SPEED) * DAMAGE_FOR_FALL_SPEED * 1.25;

				if (flFallDamage > pev->health)
				{
					EMIT_SOUND(ENT(pev), CHAN_ITEM, "common/bodysplat.wav", 1, ATTN_NORM);
				}

				if (flFallDamage > 0)
					TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), flFallDamage, DMG_FALL);
			}
		}

		if (pev->button & (IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT))
		{
			if (FBitSet(pev->button, IN_SPEED) || FBitSet(pev->flags, FL_DUCKING))
			{
				if (pev->button & IN_FORWARD)
				{
					SetAnimation(PLAYER_WALK);
				}
				else if (pev->button & IN_BACK)
				{
					SetAnimation(PLAYER_WALK_BSIDE);
				}
				else if (pev->button & IN_MOVELEFT)
				{
					SetAnimation(PLAYER_WALK_LSIDE);
				}
				else if (pev->button & IN_MOVERIGHT)
				{
					SetAnimation(PLAYER_WALK_RSIDE);
				}
			}
			else
			{
				if (pev->button & IN_FORWARD)
				{
					SetAnimation(PLAYER_RUN);
				}
				else if (pev->button & IN_BACK)
				{
					SetAnimation(PLAYER_RUN_BSIDE);
				}
				else if (pev->button & IN_MOVELEFT)
				{
					SetAnimation(PLAYER_RUN_LSIDE);
				}
				else if (pev->button & IN_MOVERIGHT)
				{
					SetAnimation(PLAYER_RUN_RSIDE);
				}
			}
		}
		else
		{
			SetAnimation(PLAYER_IDLE);
		}
	}

	if (m_flGodModeTime && gpGlobals->time > m_flGodModeTime)
	{
		pev->renderfx = kRenderFxNone;
		pev->renderamt = 255;
		pev->takedamage = DAMAGE_AIM;
		m_flGodModeTime = 0;
	}

	if (m_flSpawnTime && gpGlobals->time > m_flSpawnTime)
	{
		if (g_pGameRule->PlayerCanSpawn(this))
		{
			Spawn();
		}

		m_flSpawnTime = 0;
	}

	if (pev->flags & FL_ONGROUND)
		m_flFallVelocity = 0;

	m_afButtonLast = pev->button;
}

void CBasePlayer::DropPunchAngle(void)
{
	float base, modifier;
	float len;

	if (m_pActiveWeapon)
	{
		m_pActiveWeapon->GetDropPunchAngle(base, modifier);
	}
	else
	{
		base = 10;
		modifier = 0.5;
	}

	len = VectorNormalize(pev->punchangle);
	len -= (base + len * modifier) * gpGlobals->frametime;
	len = max(len, 0.0);
	VectorScale(pev->punchangle, len, pev->punchangle);
}

void CBasePlayer::JoiningThink(void)
{
	switch (m_iJoiningState)
	{
		case JOINED:
		{
			return;
		}
		case PUTINSERVER:
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgConnected, NULL, ENT(pev));
			MESSAGE_END();

			m_iJoiningState = WAITINGCLIENT;

			return;
		}
		case WAITINGCLIENT:
		{
			return;
		}
		case GETINTOGAME:
		{
			if ((m_iJoinToTeam != TEAM_GR && m_iJoinToTeam != TEAM_BL) || !CharacterData_GetData(m_iJoinToCharacter))
			{
				m_iJoiningState = WAITINGCLIENT;
				return;
			}

			m_iTeam = m_iJoinToTeam;
			m_bIsObserver = m_iJoinToObserver;

			// Try to join
			if (!g_pGameRule->PlayerCanJoin(this))
			{
				m_iJoiningState = WAITINGCLIENT;
				return;
			}

			if (!m_bIsObserver)
			{
				m_iClass = RANDOM_LONG(10, 100);
				m_iCharacterId = m_iJoinToCharacter;

				SetCharacter(m_iCharacterId);

			}
			else
			{
				pev->classname = MAKE_STRING("player");
				pev->flags &= ~FL_PROXY;
				pev->flags |= FL_CLIENT;

				m_iClass = 0;
				m_iCharacterId = m_iJoinToCharacter;

				m_iSpectatorLastMode = OBS_CHASE_FREE;

				CBaseEntity *pSpot = NULL;

				if (m_iTeam == TEAM_BL)
					pSpot = UTIL_FindEntityByClassname(NULL, "info_player_deathmatch");
				else
					pSpot = UTIL_FindEntityByClassname(NULL, "info_player_start");

				StartSpectator(pSpot->pev->origin, pSpot->pev->angles);
			}

			m_iJoiningState = JOINED;

			g_pGameRule->PlayerJoin(this);

			return;
		}
	}
}

void CBasePlayer::Spawn(void)
{
	Precache();

	m_szAnimExtention[0] = 0;
	m_flVelocityModifier = 1;
	m_flFallVelocity = 0;
	m_hSpectatorTarget.Set(NULL);
	m_bIsObserver = FALSE;
	m_bIsSpectator = FALSE;
	m_fInitHUD = TRUE;
	m_bitsHUDDamage = 0;
	m_bitsDamageType = 0;
	m_iFOV = 90;
	m_fFOVChangeTime = 0;
	m_pLastWeapon = NULL;
	m_flNextAttack = 0;
	m_LastHitGroup = 0;
	m_bitsDamageType = 0;
	m_flDeadTime = 0;
	m_bEnableBag = FALSE;

	pev->classname = MAKE_STRING("player");
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_WALK;
	pev->flags = FL_CLIENT;
	pev->effects = 0;
	pev->deadflag = DEAD_NO;
	pev->health = 100;
	pev->max_health = 100;
	pev->takedamage = DAMAGE_AIM;
	pev->friction = 1;
	pev->gravity = 1;
	pev->maxspeed = 1000;
	pev->iuser1 = 0;
	pev->iuser2 = 0;
	pev->iuser3 = 0;
	pev->iuser4 = 0;
	pev->fuser1 = 0;
	pev->fuser2 = 0;
	pev->fuser3 = 0;
	pev->fuser4 = 0;

	if (pev->flags & FL_DUCKING)
		pev->view_ofs = VEC_DUCK_VIEW;
	else
		pev->view_ofs = VEC_IDLE_VIEW;

	SetFOV(90, 0);

	RemoveAllWeapons();

	g_pGameRule->PlayerSpawn(this);
}

void CBasePlayer::Precache(void)
{
	PRECACHE_MODEL("Models/Character/Player.mdl");
}

BOOL CBasePlayer::IsObserver(void)
{
	return pev->iuser1 != 0;
}

BOOL CBasePlayer::IsObservingPlayer(CBasePlayer *pTarget)
{
	return pev->iuser2 == pTarget->entindex();
}

void CBasePlayer::SelectNextWeapon(BOOL bReverse)
{
	if (!m_pActiveWeapon)
	{
		RetireWeapon();
		return;
	}

	int iStartSlot = m_pActiveWeapon->GetSlot();
	int iCurrentSlot = iStartSlot;

	if (!bReverse)
	{
		if (m_pActiveWeapon->m_pNext)
		{
			SelectWeapon(m_pActiveWeapon->m_pNext);
			return;
		}
		else
		{
			do 
			{
				iCurrentSlot++;

				if (iCurrentSlot > MAX_WEAPON_TYPES - 1)
					iCurrentSlot = 0;

				CBasePlayerWeapon *pWeapon = m_rgpPlayerWeapons[iCurrentSlot];

				if (pWeapon)
				{
					SelectWeapon(pWeapon);
					return;
				}
			}
			while (iCurrentSlot != iStartSlot);
		}
	}
	else
	{
		CBasePlayerWeapon *pWeapon = m_rgpPlayerWeapons[iStartSlot];

		while (pWeapon && pWeapon->m_pNext != m_pActiveWeapon)
		{
			pWeapon = pWeapon->m_pNext;
		}

		if (pWeapon)
		{
			SelectWeapon(pWeapon);
			return;
		}
		else
		{
			do 
			{
				iCurrentSlot--;

				if (iCurrentSlot < 0)
					iCurrentSlot = MAX_WEAPON_TYPES - 1;

				CBasePlayerWeapon *pWeapon = m_rgpPlayerWeapons[iCurrentSlot];

				if (pWeapon)
				{
					while (pWeapon && pWeapon->m_pNext)
					{
						pWeapon = pWeapon->m_pNext;
					}

					if (pWeapon)
					{
						SelectWeapon(pWeapon);
						return;
					}
				}
			}
			while (iCurrentSlot != iStartSlot);
		}
	}
}

void CBasePlayer::SelectLastWeapon(void)
{
	if (m_pLastWeapon)
	{
		SelectWeapon(m_pLastWeapon);
	}
}

BOOL CBasePlayer::GiveWeapon(int index, BOOL bSelect)
{
	WeaponData *info = WeaponData_GetData(index);

	if (!info)
		return FALSE;

	edict_t *pEntity = CREATE_NAMED_ENTITY(info->iszWeaponClass);

	if (!pEntity)
		return FALSE;

	CBasePlayerWeapon *pWeapon = (CBasePlayerWeapon *)CBaseEntity::Instance(pEntity);

	memcpy(&pWeapon->m_Info, info, sizeof(WeaponData));

	pWeapon->Spawn();

	if (!AddPlayerWeapon(pWeapon))
	{
		pWeapon->Kill();
		return FALSE;
	}

	if (bSelect)
	{
		SelectWeapon(pWeapon);
	}

	return TRUE;
}

void CBasePlayer::ImpulseCommands(void)
{
	pev->impulse = 0;
}

BOOL CBasePlayer::AddPlayerWeapon(CBasePlayerWeapon *pWeapon)
{
	CBasePlayerWeapon *pInsert = m_rgpPlayerWeapons[pWeapon->GetSlot()];

	while (pInsert)
	{
		if (pInsert->GetId() == pWeapon->GetId())
			return FALSE;

		pInsert = pInsert->m_pNext;
	}

	pWeapon->m_pNext = m_rgpPlayerWeapons[pWeapon->GetSlot()];
	m_rgpPlayerWeapons[pWeapon->GetSlot()] = pWeapon;

	pWeapon->AttachToPlayer(this);

	MESSAGE_BEGIN(MSG_ONE, gmsgWeapSlot, NULL, ENT(pev));
	WRITE_BYTE(1);	// Add
	WRITE_SHORT(pWeapon->GetId());
	MESSAGE_END();

	return TRUE;
}

void CBasePlayer::RemovePlayerWeapon(CBasePlayerWeapon *pWeapon)
{
	if (pWeapon == m_pActiveWeapon)
	{
		SelectWeapon(NULL);
	}
	else if (pWeapon == m_pLastWeapon)
	{
		m_pLastWeapon = NULL;
	}

	CBasePlayerWeapon **pIter = &m_rgpPlayerWeapons[pWeapon->GetSlot()];

	BOOL bRemoved = FALSE;

	if (pWeapon == *pIter)
	{
		*pIter = (*pIter)->m_pNext;
		bRemoved = TRUE;
	}
	else
	{
		while (*pIter && (*pIter)->m_pNext != pWeapon)
		{
			pIter = &(*pIter)->m_pNext;
		}

		if (*pIter)
		{
			(*pIter)->m_pNext = (*pIter)->m_pNext->m_pNext;
			bRemoved = TRUE;
		}
	}

	if (bRemoved)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgWeapSlot, NULL, ENT(pev));
		WRITE_BYTE(0);	// Remove
		WRITE_SHORT(pWeapon->GetId());
		MESSAGE_END();
	}
}

void CBasePlayer::ItemPreFrame(void)
{
	if (m_pActiveWeapon)
		m_pActiveWeapon->ItemPreFrame();
}

void CBasePlayer::ItemPostFrame(void)
{
	if (m_pActiveWeapon)
		m_pActiveWeapon->ItemPostFrame();	
}

void CBasePlayer::UpdateClientData(void)
{
	if (!m_fGameHUDInitialized)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgInitHUD, NULL, edict());
		MESSAGE_END();

		m_fGameHUDInitialized = TRUE;
	}

	if (m_fInitHUD)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgResetHUD, NULL, edict());
		MESSAGE_END();

		m_fInitHUD = FALSE;
	}

	//if (m_iFOV != m_iClientFOV)
	//{
	//	MESSAGE_BEGIN(MSG_ONE, gmsgSetFOV, NULL, edict());
	//	WRITE_BYTE(m_iFOV);
	//	MESSAGE_END();
	//	
	//	m_iClientFOV = m_iFOV;
	//}
	
	if (pev->health != m_iClientHealth)
	{
		int iHealth = max(pev->health, 0);
		
		MESSAGE_BEGIN(MSG_ONE, gmsgHealth, NULL, edict());
		WRITE_SHORT(iHealth);
		MESSAGE_END();
		
		m_iClientHealth = pev->health;
	}
	
	if (pev->armorvalue != m_iClientArmor)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgArmor, NULL, edict());
		WRITE_SHORT(pev->armorvalue);
		MESSAGE_END();
		
		m_iClientArmor = pev->armorvalue;
	}

	if (m_bitsHUDDamage != m_bitsDamageType)
	{
		if (m_bitsDamageType & DMG_FALL)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgDmgArrow, NULL, ENT(pev));
			WRITE_BYTE(TRUE);
			WRITE_COORD(0);
			WRITE_COORD(0);
			WRITE_COORD(0);
			MESSAGE_END();
		}
		else
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgDmgArrow, NULL, ENT(pev));
			WRITE_BYTE(FALSE);
			WRITE_COORD(m_vecDamageOrigin.x);
			WRITE_COORD(m_vecDamageOrigin.y);
			WRITE_COORD(m_vecDamageOrigin.z);
			MESSAGE_END();
		}

		m_bitsHUDDamage = m_bitsDamageType;
	}

	int iCurWeaponId, iCurClip, iCurAmmo;

	if (m_pActiveWeapon)
	{
		iCurWeaponId = m_pActiveWeapon->m_Info.iWeaponIndex;

		if (m_pActiveWeapon->m_Info.iTargetSlot == WPNSLOT_KNIFE)
			iCurClip = -1;
		else
			iCurClip = m_pActiveWeapon->m_iClip;

		iCurAmmo = m_pActiveWeapon->m_iAmmo;
	}
	else
	{
		iCurWeaponId = 0;
		iCurClip = 0;
		iCurAmmo = 0;
	}

	if (iCurWeaponId != m_iClientWeaponId || iCurClip != m_iClientClip || iCurAmmo != m_iClientAmmo)
	{
		m_iClientWeaponId = iCurWeaponId;
		m_iClientClip = iCurClip;
		m_iClientAmmo = iCurAmmo;

		MESSAGE_BEGIN(MSG_ONE, gmsgCurWeapon, NULL, edict());
		WRITE_SHORT(iCurWeaponId);
		WRITE_SHORT(iCurClip);
		WRITE_SHORT(iCurAmmo);
		MESSAGE_END();
	}
}

void CBasePlayer::EnableControl(BOOL fControl)
{
	if (!fControl)
		pev->flags |= FL_FROZEN;
	else
		pev->flags &= ~FL_FROZEN;
}

void CBasePlayer::RetireWeapon(void)
{
	int iSlot = -1;

	if (m_pActiveWeapon)
	{
		iSlot = m_pActiveWeapon->GetSlot();

		if (m_pActiveWeapon->m_pNext)
		{
			SelectWeapon(m_pActiveWeapon->m_pNext);
			return;
		}
		else
		{
			CBasePlayerWeapon *pWeapon = m_rgpPlayerWeapons[iSlot];

			if (m_pActiveWeapon != pWeapon)
			{
				SelectWeapon(pWeapon);
				return;
			}
		}
	}

	for (int i = 0; i < MAX_WEAPON_TYPES; i++)
	{
		CBasePlayerWeapon *pWeapon = m_rgpPlayerWeapons[i];

		if (pWeapon)
		{
			if (iSlot != -1 && iSlot == pWeapon->GetSlot())
				continue;

			SelectWeapon(pWeapon);
			return;
		}
	}

	SelectWeapon(NULL);
}

void CBasePlayer::DropPlayerWeapon( void )
{
	if (!m_pActiveWeapon)
		return;

	if (!m_pActiveWeapon->CanDrop())
		return;

	CBasePlayerWeapon *pWeapon = m_pActiveWeapon;

	RetireWeapon();

	UTIL_MakeVectors(pev->angles);

	CWeaponBox *pWeaponBox = (CWeaponBox *)CBaseEntity::Create("weaponbox", pev->origin + gpGlobals->v_forward * 10, pev->angles, ENT(pev));
	
	pWeaponBox->Spawn();
	pWeaponBox->pev->angles.x = 0;
	pWeaponBox->pev->angles.z = 0;
	pWeaponBox->pev->velocity = gpGlobals->v_forward * 300;
	pWeaponBox->pev->nextthink = gpGlobals->time + 300;

	pWeaponBox->PackWeapon(pWeapon);

	WeaponData *pData = WeaponData_GetData(pWeapon->m_Info.iTargetSlot);

	if (pData)
	{
		SET_MODEL(ENT(pWeaponBox->pev), STRING(pData->iszModelFileName));
	}
}

BOOL CBasePlayer::HasPlayerWeapon(int index)
{
	for (int i = 0; i < MAX_WEAPON_TYPES; i++)
	{
		CBasePlayerWeapon *pCheck = m_rgpPlayerWeapons[i];

		while (pCheck)
		{
			if (pCheck->m_Info.iWeaponIndex == index)
				return TRUE;

			pCheck = pCheck->m_pNext;
		}
	}

	return FALSE;
}

void CBasePlayer::SelectWeapon(CBasePlayerWeapon *pWeapon)
{
	if (m_pActiveWeapon)
	{
		if (!pWeapon)
		{
			m_pActiveWeapon->Holster();
			m_pActiveWeapon = NULL;
			return;
		}

		if (pWeapon == m_pActiveWeapon)
			return;

		if (!m_pActiveWeapon->CanHolster())
			return;

		m_pActiveWeapon->Holster();
	}
	else
	{
		if (!pWeapon)
		{
			return;
		}
	}

	if (!pWeapon->CanDeploy())
		return;

	m_pLastWeapon = m_pActiveWeapon;

	if (m_pLastWeapon)
	{
		if (m_pLastWeapon->m_Info.iTargetSlot == pWeapon->m_Info.iTargetSlot)
		{
			m_pLastWeapon->m_bSlotActive = FALSE;
		}
	}

	m_pActiveWeapon = pWeapon;

	m_pActiveWeapon->m_bSlotActive = TRUE;
	m_pActiveWeapon->Deploy();

	MESSAGE_BEGIN(MSG_ONE, gmsgWeapSlotA, NULL, ENT(pev));
	WRITE_SHORT(m_pActiveWeapon->GetId());
	MESSAGE_END();

	ResetMaxSpeed();
}

void CBasePlayer::SelectSlot(int iSlot)
{
	CBasePlayerWeapon *pWeapon;

	pWeapon = m_rgpPlayerWeapons[iSlot];

	if (m_pActiveWeapon && m_pActiveWeapon->m_Info.iTargetSlot == iSlot)
	{
		if (m_pActiveWeapon->m_pNext)
		{
			SelectWeapon(m_pActiveWeapon->m_pNext);
			return;
		}
		else
		{
			if (pWeapon != m_pActiveWeapon)
			{
				SelectWeapon(pWeapon);
				return;
			}
		}

		return;
	}

	while (pWeapon)
	{
		if (pWeapon->m_bSlotActive)
		{
			SelectWeapon(pWeapon);
			return;
		}

		pWeapon = pWeapon->m_pNext;
	}

	pWeapon = m_rgpPlayerWeapons[iSlot];

	if (pWeapon)
	{
		SelectWeapon(pWeapon);
	}
}

CBasePlayerWeapon *CBasePlayer::GetActiveWeapon(void) const
{
	return m_pActiveWeapon;
}

int CBasePlayer::GetActiveWeaponId(void)
{
	if (m_pActiveWeapon)
		return m_pActiveWeapon->GetId();

	return 0;
}

void CBasePlayer::StudioProcessGait(void)
{
}

void CBasePlayer::HandleSignals(void)
{
	if (m_iTeam == TEAM_GR || m_iTeam == TEAM_BL)
	{
		CBaseEntity *pEntity = NULL;
		char *classname = (m_iTeam == TEAM_GR) ? "info_player_start" : "info_player_deathmatch";

		BOOL bFound = FALSE;

		while ((pEntity = UTIL_FindEntityByClassname(pEntity, classname)) != NULL)
		{
			if ((pEntity->pev->origin - pev->origin).Length() < 200)
			{
				bFound = TRUE;
				break;
			}
		}

		if (m_bEnableBag != bFound)
		{
			m_bEnableBag = bFound;

			MESSAGE_BEGIN(MSG_ONE, gmsgEnableBag, NULL, ENT(pev));
			WRITE_BYTE(m_bEnableBag);
			MESSAGE_END();
		}
	}
}

void CBasePlayer::SetACE(int type)
{
	if (m_iAceType != type)
	{
		MESSAGE_BEGIN(MSG_ALL, gmsgAceType);
		WRITE_BYTE(GetId());
		WRITE_BYTE(type);
		MESSAGE_END();

		m_iAceType = type;
	}
}

bool CBasePlayer::InSameTeam(CBasePlayer *player) const
{
	return player->m_iTeam == m_iTeam;
}

bool CBasePlayer::IsReloading(void) const
{
	if (!m_pActiveWeapon)
		return false;

	return m_pActiveWeapon->m_fInReload || m_pActiveWeapon->m_fInSpecialReload;
}

int GetPlayerGaitsequence(const edict_t *pEdict)
{
	return pEdict->v.gaitsequence;
}

bool CheckAngle(entvars_t *pev, entvars_t *pevTarget, float flAngle)
{
	Vector2D cross;
	float rad;

	cross = (pevTarget->origin - pev->origin).Make2D();

	cross = cross.Normalize();

	UTIL_MakeVectors(pev->angles);

	rad = DotProduct(cross, gpGlobals->v_forward.Make2D());

	return rad > cos(Radian(flAngle));
}

int CBasePlayer::KnifeAttack(float flRange, float flAngle, float flDamage)
{
	int result;
	Vector vecSrc, vecEnd;
	TraceResult tr;
	CBaseEntity *pEntity = NULL;

	vecSrc = GetGunPosition();

	UTIL_MakeVectors(pev->v_angle);

	vecEnd = vecSrc + (gpGlobals->v_forward * flRange);

	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(pev), &tr);

	result = 0;

	if (tr.flFraction < 1)
		result = 1;

	float end_z = vecEnd.z;

	while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, flRange)) != NULL)
	{
		if (pEntity->pev->flags & FL_KILLME)
			continue;

		if (pEntity == this)
			continue;

		if (!pEntity->IsAlive())
			continue;

		if (!CheckAngle(pev, pEntity->pev, flAngle))
			continue;

		vecSrc = GetGunPosition();
		vecEnd = pEntity->Center();

		vecEnd.z = vecSrc.z + (end_z - vecSrc.z) * ((vecSrc - vecEnd).Length() / flRange);

		vecEnd = vecSrc + ((vecEnd - vecSrc).Normalize() * flRange);

		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(pev), &tr);

		if (tr.flFraction >= 1)
			UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT(pev), &tr);

		if (tr.flFraction < 1)
		{
			if (pEntity->IsPlayer())
				result = 2;

			if (tr.pHit == pEntity->edict())
			{
				UTIL_MakeVectors(pev->v_angle);

				ClearMultiDamage();
				pEntity->TraceAttack(pev, flDamage, gpGlobals->v_forward, &tr, DMG_KNIFE);
				ApplyMultiDamage(pev, pev);
			}
		}
	}

	return result;
}

Vector CBasePlayer::FireBullets(Vector &vecSrc, Vector &vecDirShooting, float flSpread, float flDistance, int iPenetration, int iPenetrationPower, int flPenetrationDistance, int iDamage, float flRangeModifier, entvars_t *pevAttacker, int shared_rand)
{
	int iOriginalPenetration = iPenetration;
	int iCurrentDamage = iDamage;
	float flCurrentDistance;
	TraceResult tr, tr2;
	Vector vecRight = gpGlobals->v_right;
	Vector vecUp = gpGlobals->v_up;
	CBaseEntity *pEntity;
	bool bHitMetal = false;

	//switch (iBulletType)
	//{
	//	case BULLET_PLAYER_9MM:
	//	{
	//		iPenetrationPower = 21;
	//		flPenetrationDistance = 800;
	//		iSparksAmount = 15;
	//		iCurrentDamage += (-4 + RANDOM_LONG(0, 10));
	//		break;
	//	}

	//	case BULLET_PLAYER_45ACP:
	//	{
	//		iPenetrationPower = 15;
	//		flPenetrationDistance = 500;
	//		iSparksAmount = 20;
	//		iCurrentDamage += (-2 + RANDOM_LONG(0, 4));
	//		break;
	//	}

	//	case BULLET_PLAYER_50AE:
	//	{
	//		iPenetrationPower = 30;
	//		flPenetrationDistance = 1000;
	//		iSparksAmount = 20;
	//		iCurrentDamage += (-4 + RANDOM_LONG(0, 10));
	//		break;
	//	}

	//	case BULLET_PLAYER_762MM:
	//	{
	//		iPenetrationPower = 39;
	//		flPenetrationDistance = 5000;
	//		iSparksAmount = 30;
	//		iCurrentDamage += (-2 + RANDOM_LONG(0, 4));
	//		break;
	//	}

	//	case BULLET_PLAYER_556MM:
	//	{
	//		iPenetrationPower = 35;
	//		flPenetrationDistance = 4000;
	//		iSparksAmount = 30;
	//		iCurrentDamage += (-3 + RANDOM_LONG(0, 6));
	//		break;
	//	}

	//	case BULLET_PLAYER_338MAG:
	//	{
	//		iPenetrationPower = 45;
	//		flPenetrationDistance = 8000;
	//		iSparksAmount = 30;
	//		iCurrentDamage += (-4 + RANDOM_LONG(0, 8));
	//		break;
	//	}

	//	case BULLET_PLAYER_57MM:
	//	{
	//		iPenetrationPower = 30;
	//		flPenetrationDistance = 2000;
	//		iSparksAmount = 20;
	//		iCurrentDamage += (-4 + RANDOM_LONG(0, 10));
	//		break;
	//	}

	//	case BULLET_PLAYER_357SIG:
	//	{
	//		iPenetrationPower = 25;
	//		flPenetrationDistance = 800;
	//		iSparksAmount = 20;
	//		iCurrentDamage += (-4 + RANDOM_LONG(0, 10));
	//		break;
	//	}

	//	default:
	//	{
	//		iPenetrationPower = 0;
	//		flPenetrationDistance = 0;
	//		break;
	//	}
	//}

	if (!pevAttacker)
		pevAttacker = pev;

	gMultiDamage.type = DMG_BULLET;

	float x, y;

	if (IsPlayer())
	{
		x = UTIL_SharedRandomFloat(shared_rand + 0, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + 1, -0.5, 0.5);
		y = UTIL_SharedRandomFloat(shared_rand + 2, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + 3, -0.5, 0.5);
	}
	//else
	//{
	//	do
	//	{
	//		x = RANDOM_FLOAT(-0.5, 0.5) + RANDOM_FLOAT(-0.5, 0.5);
	//		y = RANDOM_FLOAT(-0.5, 0.5) + RANDOM_FLOAT(-0.5, 0.5);
	//		z = x * x + y * y;
	//	}
	//	while (z > 1);
	//}

	Vector vecDir = vecDirShooting + (vecRight * (x * flSpread)) + (vecUp * (y * flSpread));
	Vector vecEnd = vecSrc + vecDir * flDistance;
	Vector vecOldSrc;
	Vector vecNewSrc;
	float flDamageModifier = 0.5;

	while (iPenetration != 0)
	{
		ClearMultiDamage();
		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(pev), &tr);

		char cTextureType = UTIL_TextureHit(&tr, vecSrc, vecEnd);
		bool bSparks = false;

		//switch (cTextureType)
		//{
		//	case CHAR_TEX_METAL:
		//	{
		//		bSparks = true;
		//		bHitMetal = true;
		//		iPenetrationPower *= 0.15;
		//		flDamageModifier = 0.2;
		//		break;
		//	}

		//	case CHAR_TEX_CONCRETE:
		//	{
		//		iPenetrationPower *= 0.25;
		//		flDamageModifier = 0.25;
		//		break;
		//	}

		//	case CHAR_TEX_GRATE:
		//	{
		//		bSparks = true;
		//		bHitMetal = true;
		//		iPenetrationPower *= 0.5;
		//		flDamageModifier = 0.4;
		//		break;
		//	}

		//	case CHAR_TEX_VENT:
		//	{
		//		bSparks = true;
		//		bHitMetal = true;
		//		iPenetrationPower *= 0.5;
		//		flDamageModifier = 0.45;
		//		break;
		//	}

		//	case CHAR_TEX_TILE:
		//	{
		//		iPenetrationPower *= 0.65;
		//		flDamageModifier = 0.3;
		//		break;
		//	}

		//	case CHAR_TEX_COMPUTER:
		//	{
		//		bSparks = true;
		//		bHitMetal = true;
		//		iPenetrationPower *= 0.4;
		//		flDamageModifier = 0.45;
		//		break;
		//	}

		//	case CHAR_TEX_WOOD:
		//	{
		//		iPenetrationPower *= 1;
		//		flDamageModifier = 0.6;
		//		break;
		//	}

		//	default:
		//	{
		//		bSparks = false;
		//		break;
		//	}
		//}

		if (tr.flFraction != 1.0)
		{
			pEntity = CBaseEntity::Instance(tr.pHit);
			iPenetration--;
			flCurrentDistance = tr.flFraction * flDistance;
			iCurrentDamage *= pow(flRangeModifier, flCurrentDistance / 500);

			if (flCurrentDistance > flPenetrationDistance)
				iPenetration = 0;

			//if (tr.iHitgroup == HITGROUP_SHIELD)
			//{
			//	iPenetration = 0;

			//	if (tr.flFraction != 1.0)
			//	{
			//		if (RANDOM_LONG(0, 1))
			//			EMIT_SOUND(pEntity->edict(), CHAN_VOICE, "weapons/ric_metal-1.wav", 1, ATTN_NORM);
			//		else
			//			EMIT_SOUND(pEntity->edict(), CHAN_VOICE, "weapons/ric_metal-2.wav", 1, ATTN_NORM);

			//		UTIL_Sparks(tr.vecEndPos);

			//		pEntity->pev->punchangle.x = iCurrentDamage * RANDOM_FLOAT(-0.15, 0.15);
			//		pEntity->pev->punchangle.z = iCurrentDamage * RANDOM_FLOAT(-0.15, 0.15);

			//		if (pEntity->pev->punchangle.x < 4)
			//			pEntity->pev->punchangle.x = 4;

			//		if (pEntity->pev->punchangle.z < -5)
			//			pEntity->pev->punchangle.z = -5;
			//		else if (pEntity->pev->punchangle.z > 5)
			//			pEntity->pev->punchangle.z = 5;
			//	}

			//	break;
			//}

			//if (VARS(tr.pHit)->solid == SOLID_BSP)
			//{
			//	switch (RANDOM_LONG(0, 3))
			//	{
			//		case 0: UTIL_DecalShoot(tr.vecEndPos, DECAL_INDEX("{shot1")); break;
			//		case 1: UTIL_DecalShoot(tr.vecEndPos, DECAL_INDEX("{shot2")); break;
			//		case 2: UTIL_DecalShoot(tr.vecEndPos, DECAL_INDEX("{shot3")); break;
			//		case 3: UTIL_DecalShoot(tr.vecEndPos, DECAL_INDEX("{shot4")); break;
			//	}
			//}

			if ((VARS(tr.pHit)->solid == SOLID_BSP) && (iPenetration != 0))
			{
				//if (bPistol)
				//	DecalGunshot(&tr, iBulletType, false, pev, bHitMetal);
				//else if (RANDOM_LONG(0, 3))
				//	DecalGunshot(&tr, iBulletType, true, pev, bHitMetal);

				vecSrc = tr.vecEndPos + (vecDir * iPenetrationPower);
				flDistance = (flDistance - flCurrentDistance) * 0.5;
				vecEnd = vecSrc + (vecDir * flDistance);

				pEntity->TraceAttack(pevAttacker, iCurrentDamage, vecDir, &tr, DMG_BULLET);

				iCurrentDamage *= flDamageModifier;
			}
			else
			{
				//if (bPistol)
				//	DecalGunshot(&tr, iBulletType, false, pev, bHitMetal);
				//else if (RANDOM_LONG(0, 3))
				//	DecalGunshot(&tr, iBulletType, true, pev, bHitMetal);

				vecSrc = tr.vecEndPos + (vecDir * 42);
				flDistance = (flDistance - flCurrentDistance) * 0.75;
				vecEnd = vecSrc + (vecDir * flDistance);

				pEntity->TraceAttack(pevAttacker, iCurrentDamage, vecDir, &tr, DMG_BULLET);

				iCurrentDamage *= 0.75;
			}
		}
		else
			iPenetration = 0;

		ApplyMultiDamage(pev, pevAttacker);
	}

	return Vector(x * flSpread, y * flSpread, 0);
}

void CBasePlayer::FireShotgun(int cShots, Vector &vecSrc, Vector &vecDirShooting, float flSpread, float flDistance, int iDamage, float flRangeModifier, entvars_t *pevAttacker, int shared_rand)
{
	TraceResult tr;
	Vector vecRight = gpGlobals->v_right;
	Vector vecUp = gpGlobals->v_up;

	ClearMultiDamage();
	gMultiDamage.type = DMG_BULLET;

	for (int iShot = 0; iShot < cShots; iShot++)
	{
		float x, y, z;

		do
		{
			x = UTIL_SharedRandomFloat(shared_rand + 0, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + 1, -0.5, 0.5);
			y = UTIL_SharedRandomFloat(shared_rand + 2, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + 3, -0.5, 0.5);
			z = x * x + y * y;

			shared_rand += 4;
		}
		while (z > 1);

		Vector vecDir = vecDirShooting  + x * flSpread * vecRight + y * flSpread * vecUp;
		Vector vecEnd = vecSrc + vecDir * flDistance;

		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(pev), &tr);

		if (tr.flFraction != 1)
		{
			CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

			if (pEntity)
			{
				float flTakeDamage = iDamage - (iDamage - iDamage * flRangeModifier) * (1 - tr.flFraction);

				pEntity->TraceAttack(pevAttacker, flTakeDamage, vecDir, &tr, DMG_BULLET);
			}
		}
	}

	ApplyMultiDamage(pevAttacker, pevAttacker);
}

void CBasePlayer::SetGodMode(BOOL bState, float flHold)
{
	if (bState)
	{
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor = Vector(255, 255, 255);
		pev->renderamt = 5;
		pev->takedamage = DAMAGE_NO;
		m_flGodModeTime = gpGlobals->time + flHold;
	}
	else
	{
		pev->renderfx = kRenderFxNone;
		pev->renderamt = 255;
		pev->takedamage = DAMAGE_AIM;
		m_flGodModeTime = 0;
	}
}

void CBasePlayer::SetRespawn(float flTime)
{
	m_flSpawnTime = gpGlobals->time + flTime;
}

void CBasePlayer::BagRebuy(void)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgBagRebuy, NULL, ENT(pev));
	MESSAGE_END();
}