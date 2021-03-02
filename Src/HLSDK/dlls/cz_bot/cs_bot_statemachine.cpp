/****
 * 
 * Copyright (c) 2017, Crsky
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

#include "cs_bot.h"


// This method is the ONLY legal way to change a bot's current state
void CCSBot::SetState(BotState *state)
{
	PrintIfWatched("SetState: %s -> %s\n", (m_state != NULL) ? m_state->GetName() : "NULL", state->GetName());

	// if we changed state from within the special Attack state, we are no longer attacking
	if (m_isAttacking)
		StopAttacking();

	if (m_state != NULL)
		m_state->OnExit(this);

	state->OnEnter(this);

	m_state = state;
	m_stateTimestamp = gpGlobals->time;
}

void CCSBot::Idle()
{
	SetTask(SEEK_AND_DESTROY);
	SetState(&m_idleState);
}

void CCSBot::EscapeFromBomb()
{
//	SetTask(ESCAPE_FROM_BOMB);
//	SetState(&m_escapeFromBombState);
}

void CCSBot::Follow(CBasePlayer *player)
{
	if (player == NULL)
		return;

	// note when we began following
	if (!m_isFollowing || m_leader != player)
		m_followTimestamp = gpGlobals->time;

	m_isFollowing = true;
	m_leader = player;

	SetTask(FOLLOW);
	m_followState.SetLeader(player);
	SetState(&m_followState);
}

// Continue following our leader after finishing what we were doing
void CCSBot::ContinueFollowing()
{
	SetTask(FOLLOW);
	m_followState.SetLeader(m_leader);
	SetState(&m_followState);
}

// Stop following
void CCSBot::StopFollowing()
{
	m_isFollowing = false;
	m_leader = NULL;
	m_allowAutoFollowTime = gpGlobals->time + 10.0f;
}

// Begin process of rescuing hostages
void CCSBot::RescueHostages()
{
	SetTask(RESCUE_HOSTAGES);
}

// Use the entity
void CCSBot::UseEntity(CBaseEntity *entity)
{
	m_useEntityState.SetEntity(entity);
	SetState(&m_useEntityState);
}

// DEPRECATED: Use TryToHide() instead.
// Move to a hiding place.
// If 'searchFromArea' is non-NULL, hiding spots are looked for from that area first.
void CCSBot::Hide(CNavArea *searchFromArea, float duration, float hideRange, bool holdPosition)
{
	DestroyPath();

	CNavArea *source;
	Vector sourcePos;
	if (searchFromArea)
	{
		source = searchFromArea;
		sourcePos = *searchFromArea->GetCenter();
	}
	else
	{
		source = m_lastKnownArea;
		sourcePos = pev->origin;
	}

	if (source == NULL)
	{
		PrintIfWatched("Hide from area is NULL.\n");
		Idle();
		return;
	}

	m_hideState.SetSearchArea(source);
	m_hideState.SetSearchRange(hideRange);
	m_hideState.SetDuration(duration);
	m_hideState.SetHoldPosition(holdPosition);

	// search around source area for a good hiding spot
	Vector useSpot;

	const Vector *pos = FindNearbyHidingSpot(this, &sourcePos, source, hideRange, IsSniper());
	if (pos == NULL)
	{
		PrintIfWatched("No available hiding spots.\n");
		// hide at our current position
		useSpot = pev->origin;
	}
	else
	{
		useSpot = *pos;
	}

	m_hideState.SetHidingSpot(useSpot);

	// build a path to our new hiding spot
	if (ComputePath(TheNavAreaGrid.GetNavArea(&useSpot), &useSpot, FASTEST_ROUTE) == false)
	{
		PrintIfWatched("Can't pathfind to hiding spot\n");
		Idle();
		return;
	}

	SetState(&m_hideState);
}

// Move to the given hiding place
void CCSBot::Hide(const Vector *hidingSpot, float duration, bool holdPosition)
{
	CNavArea *hideArea = TheNavAreaGrid.GetNearestNavArea(hidingSpot);
	if (hideArea == NULL)
	{
		PrintIfWatched("Hiding spot off nav mesh\n");
		Idle();
		return;
	}

	DestroyPath();

	m_hideState.SetSearchArea(hideArea);
	m_hideState.SetSearchRange(750.0f);
	m_hideState.SetDuration(duration);
	m_hideState.SetHoldPosition(holdPosition);
	m_hideState.SetHidingSpot(*hidingSpot);

	// build a path to our new hiding spot
	if (ComputePath(hideArea, hidingSpot, FASTEST_ROUTE) == false)
	{
		PrintIfWatched("Can't pathfind to hiding spot\n");
		Idle();
		return;
	}

	SetState(&m_hideState);
}

// Try to hide nearby. Return true if hiding, false if can't hide here.
// If 'searchFromArea' is non-NULL, hiding spots are looked for from that area first.
bool CCSBot::TryToHide(CNavArea *searchFromArea, float duration, float hideRange, bool holdPosition, bool useNearest)
{
	CNavArea *source;
	Vector sourcePos;
	if (searchFromArea)
	{
		source = searchFromArea;
		sourcePos = *searchFromArea->GetCenter();
	}
	else
	{
		source = m_lastKnownArea;
		sourcePos = pev->origin;
	}

	if (source == NULL)
	{
		PrintIfWatched("Hide from area is NULL.\n");
		return false;
	}

	m_hideState.SetSearchArea(source);
	m_hideState.SetSearchRange(hideRange);
	m_hideState.SetDuration(duration);
	m_hideState.SetHoldPosition(holdPosition);

	// search around source area for a good hiding spot
	const Vector *pos = FindNearbyHidingSpot(this, &sourcePos, source, hideRange, IsSniper(), useNearest);
	if (pos == NULL)
	{
		PrintIfWatched("No available hiding spots.\n");
		return false;
	}

	m_hideState.SetHidingSpot(*pos);

	// build a path to our new hiding spot
	if (ComputePath(TheNavAreaGrid.GetNavArea(pos), pos, FASTEST_ROUTE) == false)
	{
		PrintIfWatched("Can't pathfind to hiding spot\n");
		return false;
	}

	SetState(&m_hideState);
	return true;
}

// Retreat to a nearby hiding spot, away from enemies
bool CCSBot::TryToRetreat()
{
	const float maxRange = 1000.0f;
	const Vector *spot = FindNearbyRetreatSpot(this, maxRange);

	if (spot != NULL)
	{
		// ignore enemies for a second to give us time to hide
		// reaching our hiding spot clears our disposition
		IgnoreEnemies(10.0f);

		float holdTime = RANDOM_FLOAT(3.0f, 15.0f);

		StandUp();
		Run();
		Hide(spot, holdTime);

		PrintIfWatched("Retreating to a safe spot!\n");
		return true;
	}

	return false;
}

void CCSBot::Hunt()
{
	SetState(&m_huntState);
}

// Attack our the given victim
// NOTE: Attacking does not change our task.
void CCSBot::Attack(CBasePlayer *victim)
{
	if (victim == NULL)
		return;

	// zombies never attack
	if (cv_bot_zombie.value != 0.0f)
		return;

	// cannot attack if we are reloading
	if (IsActiveWeaponReloading())
		return;

	// change enemy
	SetEnemy(victim);

	// Do not "re-enter" the attack state if we are already attacking
	if (IsAttacking())
		return;

	// if we are currently hiding, increase our chances of crouching and holding position
	if (IsAtHidingSpot())
		m_attackState.SetCrouchAndHold((RANDOM_FLOAT(0, 100) < 60.0f) != 0);
	else
		m_attackState.SetCrouchAndHold(false);

	PrintIfWatched("ATTACK BEGIN (reaction time = %g (+ update time), surprise time = %g, attack delay = %g)\n"

#ifdef REGAMEDLL_FIXES
		, GetProfile()->GetReactionTime(), m_surpriseDelay, GetProfile()->GetAttackDelay()
#endif

	);

	m_isAttacking = true;
	m_attackState.OnEnter(this);

	// cheat a bit and give the bot the initial location of its victim
	m_lastEnemyPosition = victim->pev->origin;
	m_lastSawEnemyTimestamp = gpGlobals->time;
	m_aimSpreadTimestamp = gpGlobals->time;

	// compute the angle difference between where are looking, and where we need to look
	Vector toEnemy = victim->pev->origin - pev->origin;
	Vector idealAngle = UTIL_VecToAngles(toEnemy);

	float_precision deltaYaw = float_precision(Q_abs(m_lookYaw - idealAngle.y));

	while (deltaYaw > 180.0f)
		deltaYaw -= 360.0f;

	if (deltaYaw < 0.0f)
		deltaYaw = -deltaYaw;

	// immediately aim at enemy - accuracy penalty depending on how far we must turn to aim
	// accuracy is halved if we have to turn 180 degrees
	float turn = deltaYaw * 0.0055555557;/// 180.0f;
	float accuracy = GetProfile()->GetSkill() / (1.0f + turn);

	SetAimOffset(accuracy);

	// define time when aim offset will automatically be updated
	// longer time the more we had to turn (surprise)
	m_aimOffsetTimestamp = gpGlobals->time + RANDOM_FLOAT(0.25f + turn, 1.5f);
}

// Exit the Attack state
void CCSBot::StopAttacking()
{
	PrintIfWatched("ATTACK END\n");
	m_attackState.OnExit(this);
	m_isAttacking = false;

	// if we are following someone, go to the Idle state after the attack to decide whether we still want to follow
	if (IsFollowing())
	{
		Idle();
	}
}

bool CCSBot::IsAttacking() const
{
	return m_isAttacking;
}

// Return true if we are escaping from the bomb
bool CCSBot::IsEscapingFromBomb() const
{
//	if (m_state == static_cast<const BotState *>(&m_escapeFromBombState))
//		return true;

	return false;
}

// Return true if we are defusing the bomb
bool CCSBot::IsDefusingBomb() const
{
//	if (m_state == static_cast<const BotState *>(&m_defuseBombState))
//		return true;

	return false;
}

// Return true if we are hiding
bool CCSBot::IsHiding() const
{
	if (m_state == static_cast<const BotState *>(&m_hideState))
		return true;

	return false;
}

// Return true if we are hiding and at our hiding spot
bool CCSBot::IsAtHidingSpot() const
{
	if (!IsHiding())
		return false;

	return m_hideState.IsAtSpot();
}

// Return true if we are huting
bool CCSBot::IsHunting() const
{
	if (m_state == static_cast<const BotState *>(&m_huntState))
		return true;

	return false;
}

// Return true if we are in the MoveTo state
bool CCSBot::IsMovingTo() const
{
	if (m_state == static_cast<const BotState *>(&m_moveToState))
		return true;

	return false;
}

// Return true if we are buying
bool CCSBot::IsBuying() const
{
//	if (m_state == static_cast<const BotState *>(&m_buyState))
//		return true;

	return false;
}

// Move to potentially distant position
void CCSBot::MoveTo(const Vector *pos, RouteType route)
{
	m_moveToState.SetGoalPosition(*pos);
	m_moveToState.SetRouteType(route);
	SetState(&m_moveToState);
}

void CCSBot::PlantBomb()
{
//	SetState(&m_plantBombState);
}

// Bomb has been dropped - go get it
void CCSBot::FetchBomb()
{
//	SetState(&m_fetchBombState);
}

void CCSBot::DefuseBomb()
{
//	SetState(&m_defuseBombState);
}

// Investigate recent enemy noise
void CCSBot::InvestigateNoise()
{
	SetState(&m_investigateNoiseState);
}
