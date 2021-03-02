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


// Return the number of bots following the given player
int GetBotFollowCount(CBasePlayer *leader)
{
	int count = 0;
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *player = UTIL_PlayerByIndex(i);

		if (!player)
			continue;

		if (FNullEnt(player->pev))
			continue;

		if (FStrEq(STRING(player->pev->netname), ""))
			continue;

		if (!player->IsBot())
			continue;

 		if (!player->IsAlive())
 			continue;

		CCSBot *bot = reinterpret_cast<CCSBot *>(player);
		if (bot->IsBot() && bot->GetFollowLeader() == leader)
			++count;
	}

	return count;
}

// Change movement speed to walking
void CCSBot::Walk()
{
	if (m_mustRunTimer.IsElapsed())
	{
		CBot::Walk();
		return;
	}

	// must run
	Run();
}

// Return true if jump was started.
// This is extended from the base jump to disallow jumping when in a crouch area.
bool CCSBot::Jump(bool mustJump)
{
	// prevent jumping if we're crouched, unless we're in a crouchjump area - jump wins
	bool inCrouchJumpArea = (m_lastKnownArea &&
		(m_lastKnownArea->GetAttributes() & NAV_CROUCH) &&
		!(m_lastKnownArea->GetAttributes() & NAV_JUMP));

	if (inCrouchJumpArea)
	{
		return false;
	}

	return CBot::Jump(mustJump);
}

// Invoked when injured by something
// NOTE: We dont want to directly call Attack() here, or the bots will have super-human reaction times when injured
void CCSBot::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	CBaseEntity *attacker = CBaseEntity::Instance(pevInflictor);

	// if we were attacked by a teammate, rebuke
//	if (attacker->IsPlayer())
//	{
//		CBasePlayer *player = static_cast<CBasePlayer *>(attacker);
//		if (BotRelationship(player) == BOT_TEAMMATE && !player->IsBot())
//		{
//			GetChatter()->FriendlyFire();
//		}
//	}

	if (attacker->IsPlayer() && IsEnemy(attacker))
	{
		// Track previous attacker so we don't try to panic multiple times for a shotgun blast
		CBasePlayer *lastAttacker = m_attacker;
		float lastAttackedTimestamp = m_attackedTimestamp;

		// keep track of our last attacker
		m_attacker = static_cast<CBasePlayer *>(attacker);
		m_attackedTimestamp = gpGlobals->time;

		// no longer safe
		AdjustSafeTime();

		if (!IsSurprised() && (m_attacker != lastAttacker || m_attackedTimestamp != lastAttackedTimestamp))
		{
			CBasePlayer *enemy = static_cast<CBasePlayer *>(attacker);

			// being hurt by an enemy we can't see causes panic
			if (!IsVisible(enemy, CHECK_FOV))
			{
				bool bPanic = false;

				// if not attacking anything, look around to try to find attacker
				if (!IsAttacking())
				{
					bPanic = true;
				}
				else
				{
					// we are attacking
					if (!IsEnemyVisible())
					{
						// can't see our current enemy, panic to acquire new attacker
						bPanic = true;
					}
				}

				if (!bPanic)
				{
					float invSkill = 1.0f - GetProfile()->GetSkill();
					float panicChance = invSkill * invSkill * 50.0f;

					if (panicChance > RANDOM_FLOAT(0, 100))
					{
						bPanic = true;
					}
				}

				if (bPanic)
				{
					// can't see our current enemy, panic to acquire new attacker
					Panic(m_attacker);
				}
			}
		}
	}

	// extend
	return CBasePlayer::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

// Invoked when killed
void CCSBot::Killed(entvars_t *pevInflictor, entvars_t *pevAttacker)
{
	PrintIfWatched("Killed( attacker = %s )\n", STRING(pevAttacker->netname));

//	GetChatter()->OnDeath();

	// increase the danger where we died
	const float deathDanger = 1.0f;
	const float deathDangerRadius = 500.0f;
	IncreaseDangerNearby(m_iTeam - 1, deathDanger, m_lastKnownArea, &pev->origin, deathDangerRadius);

	// end voice feedback
//	EndVoiceFeedback();

	// extend
	CBasePlayer::Killed(pevInflictor, pevAttacker);
}

// Return true if line segment intersects rectagular volume
bool IsIntersectingBox(const Vector *start, const Vector *end, const Vector *boxMin, const Vector *boxMax)
{
	unsigned char startFlags = 0;
	unsigned char endFlags = 0;

	// classify start point
	if (start->x < boxMin->x)
		startFlags |= LO_X;
	if (start->x > boxMax->x)
		startFlags |= HI_X;

	if (start->y < boxMin->y)
		startFlags |= LO_Y;
	if (start->y > boxMax->y)
		startFlags |= HI_Y;

	if (start->z < boxMin->z)
		startFlags |= LO_Z;
	if (start->z > boxMax->z)
		startFlags |= HI_Z;

	// classify end point
	if (end->x < boxMin->x)
		endFlags |= LO_X;
	if (end->x > boxMax->x)
		endFlags |= HI_X;

	if (end->y < boxMin->y)
		endFlags |= LO_Y;
	if (end->y > boxMax->y)
		endFlags |= HI_Y;

	if (end->z < boxMin->z)
		endFlags |= LO_Z;
	if (end->z > boxMax->z)
		endFlags |= HI_Z;

	// trivial reject
	if (startFlags & endFlags)
		return false;

	// TODO: Do exact line/box intersection check
	return true;
}

// When bot is touched by another entity.
void CCSBot::BotTouch(CBaseEntity *other)
{
	// if we have touched a higher-priority player, make way
	// TODO: Need to account for reaction time, etc.
	if (other->IsPlayer())
	{
		// if we are defusing a bomb, don't move
		if (IsDefusingBomb())
			return;

		CBasePlayer *player = static_cast<CBasePlayer *>(other);

		// get priority of other player
		unsigned int otherPri = TheCSBots->GetPlayerPriority(player);

		// get our priority
		unsigned int myPri = TheCSBots->GetPlayerPriority(this);

		// if our priority is better, don't budge
		if (myPri < otherPri)
			return;

		// they are higher priority - make way, unless we're already making way for someone more important
		if (m_avoid != NULL)
		{
			unsigned int avoidPri = TheCSBots->GetPlayerPriority(static_cast<CBasePlayer *>(m_avoid));
			if (avoidPri < otherPri)
			{
				// ignore 'other' because we're already avoiding someone better
				return;
			}
		}

		m_avoid = other;
		m_avoidTimestamp = gpGlobals->time;

		return;
	}

	// If we won't be able to break it, don't try
	if (other->pev->takedamage != DAMAGE_YES)
		return;

	if (IsAttacking())
		return;

	// See if it's breakable
	if (FClassnameIs(other->pev, "func_breakable"))
	{
		Vector center = (other->pev->absmax + other->pev->absmin) / 2.0f;
		bool breakIt = true;

		if (m_pathLength)
		{
			Vector goal = m_goalPosition + Vector(0, 0, HalfHumanHeight);
			breakIt = IsIntersectingBox(&pev->origin, &goal, &other->pev->absmin, &other->pev->absmax);
		}

		if (breakIt)
		{
			// it's breakable - try to shoot it.
			SetLookAt("Breakable", &center, PRIORITY_HIGH, 0.2, 0, 5.0);

			if (IsUsingGrenade())
			{
				EquipBestWeapon();
				return;
			}

			PrimaryAttack();
		}
	}
}

bool CCSBot::IsBusy() const
{
	if (IsAttacking() ||
		IsBuying() ||
		IsDefusingBomb() ||
		GetTask() == PLANT_BOMB ||
		GetTask() == RESCUE_HOSTAGES ||
		IsSniping())
	{
		return true;
	}

	return false;
}

void CCSBot::BotDeathThink()
{
	;
}

CBasePlayer *CCSBot::FindNearbyPlayer()
{
	CBaseEntity *pEntity = NULL;
	Vector vecSrc = pev->origin;
	const float flRadius = 800.0f;

	while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecSrc, flRadius)) != NULL)
	{
		if (!pEntity->IsPlayer())
			continue;

		if (!(pEntity->pev->flags & FL_FAKECLIENT))
			continue;

		return static_cast<CBasePlayer *>(pEntity);
	}

	return NULL;
}

// Assign given player as our current enemy to attack
void CCSBot::SetEnemy(CBasePlayer *enemy)
{
	if (m_enemy != enemy)
	{
		m_enemy = enemy;
		m_currentEnemyAcquireTimestamp = gpGlobals->time;
	}
}

// If we are not on the navigation mesh (m_currentArea == NULL),
// move towards last known area.
// Return false if off mesh.
bool CCSBot::StayOnNavMesh()
{
	if (m_currentArea != NULL)
		return true;

	// move back onto the area map

	// if we have no lastKnownArea, we probably started off
	// of the nav mesh - find the closest nav area and use it
	CNavArea *goalArea;
	if (!m_currentArea && !m_lastKnownArea)
	{
		goalArea = TheNavAreaGrid.GetNearestNavArea(&pev->origin);
		PrintIfWatched("Started off the nav mesh - moving to closest nav area...\n");
	}
	else
	{
		goalArea = m_lastKnownArea;
		PrintIfWatched("Getting out of NULL area...\n");
	}

	if (goalArea != NULL)
	{
		Vector pos;
		goalArea->GetClosestPointOnArea(&pev->origin, &pos);

		// move point into area
		Vector to = pos - pev->origin;
		to.NormalizeInPlace();

		// how far to "step into" an area - must be less than min area size
		const float stepInDist = 5.0f;
		pos = pos + (stepInDist * to);

		MoveTowardsPosition(&pos);
	}

	// if we're stuck, try to get un-stuck
	// do stuck movements last, so they override normal movement
	if (m_isStuck)
	{
		Wiggle();
	}

	return false;
}

void CCSBot::Panic(CBasePlayer *enemy)
{
	if (IsSurprised())
		return;

	Vector2D dir(BotCOS(pev->v_angle.y), BotSIN(pev->v_angle.y));
	Vector2D perp(-dir.y, dir.x);
	Vector spot;

	if (GetProfile()->GetSkill() >= 0.5f)
	{
		Vector2D toEnemy = (enemy->pev->origin - pev->origin).Make2D();
		toEnemy.NormalizeInPlace();

		float along = DotProduct(toEnemy, dir);

		float c45 = 0.7071f;
		float size = 100.0f;

		float_precision shift = RANDOM_FLOAT(-75.0, 75.0);

		if (along > c45)
		{
			spot.x = pev->origin.x + dir.x * size + perp.x * shift;
			spot.y = pev->origin.y + dir.y * size + perp.y * shift;
		}
		else if (along < -c45)
		{
			spot.x = pev->origin.x - dir.x * size + perp.x * shift;
			spot.y = pev->origin.y - dir.y * size + perp.y * shift;
		}
		else if (DotProduct(toEnemy, perp) > 0.0)
		{
			spot.x = pev->origin.x + perp.x * size + dir.x * shift;
			spot.y = pev->origin.y + perp.y * size + dir.y * shift;
		}
		else
		{
			spot.x = pev->origin.x - perp.x * size + dir.x * shift;
			spot.y = pev->origin.y - perp.y * size + dir.y * shift;
		}
	}
	else
	{
		const float offset = 200.0f;
		float_precision side = RANDOM_FLOAT(-offset, offset) * 2.0f;

		spot.x = pev->origin.x - dir.x * offset + perp.x * side;
		spot.y = pev->origin.y - dir.y * offset + perp.y * side;
	}

	spot.z = pev->origin.z + RANDOM_FLOAT(-50.0, 50.0);

	// we are stunned for a moment
	m_surpriseDelay = RANDOM_FLOAT(0.1, 0.2);
	m_surpriseTimestamp = gpGlobals->time;

	SetLookAt("Panic", &spot, PRIORITY_HIGH, 0, 0, 5.0);
	PrintIfWatched("Aaaah!\n");
}

bool CCSBot::IsDoingScenario() const
{
	if (cv_bot_defer_to_human.value <= 0.0f)
		return true;

	return !UTIL_HumansOnTeam(m_iTeam, IS_ALIVE);
}

// Return true if we noticed the bomb on the ground or on the radar (for T's only)
bool CCSBot::NoticeLooseBomb() const
{
	if (TheCSBots->GetScenario() != CCSBotManager::SCENARIO_DEFUSE_BOMB)
		return false;

	CBaseEntity *bomb = TheCSBots->GetLooseBomb();

	if (bomb != NULL)
	{
		// T's can always see bomb on their radar
		return true;
	}

	return false;
}

// Return true if can see the bomb lying on the ground
bool CCSBot::CanSeeLooseBomb() const
{
	if (TheCSBots->GetScenario() != CCSBotManager::SCENARIO_DEFUSE_BOMB)
		return false;

	CBaseEntity *bomb = TheCSBots->GetLooseBomb();

	if (bomb != NULL)
	{
		if (IsVisible(&bomb->pev->origin, CHECK_FOV))
			return true;
	}

	return false;
}

// Return true if can see the planted bomb
bool CCSBot::CanSeePlantedBomb() const
{
	if (TheCSBots->GetScenario() != CCSBotManager::SCENARIO_DEFUSE_BOMB)
		return false;

	if (!GetGameState()->IsBombPlanted())
		return false;

	const Vector *bombPos = GetGameState()->GetBombPosition();

	if (bombPos != NULL && IsVisible(bombPos, CHECK_FOV))
		return true;

	return false;
}

// Return last enemy that hurt us
CBasePlayer *CCSBot::GetAttacker() const
{
	if (m_attacker != NULL && m_attacker->IsAlive())
		return m_attacker;

	return NULL;
}

// Immediately jump off of our ladder, if we're on one
void CCSBot::GetOffLadder()
{
	if (IsUsingLadder())
	{
		Jump(MUST_JUMP);
		DestroyPath();
	}
}

// Return time when given spot was last checked
float CCSBot::GetHidingSpotCheckTimestamp(HidingSpot *spot) const
{
	for (int i = 0; i < m_checkedHidingSpotCount; ++i)
	{
		if (m_checkedHidingSpot[i].spot->GetID() == spot->GetID())
			return m_checkedHidingSpot[i].timestamp;
	}

	return -999999.9f;
}

// Set the timestamp of the given spot to now.
// If the spot is not in the set, overwrite the least recently checked spot.
void CCSBot::SetHidingSpotCheckTimestamp(HidingSpot *spot)
{
	int leastRecent = 0;
	float leastRecentTime = gpGlobals->time + 1.0f;

	for (int i = 0; i < m_checkedHidingSpotCount; ++i)
	{
		// if spot is in the set, just update its timestamp
		if (m_checkedHidingSpot[i].spot->GetID() == spot->GetID())
		{
			m_checkedHidingSpot[i].timestamp = gpGlobals->time;
			return;
		}

		// keep track of least recent spot
		if (m_checkedHidingSpot[i].timestamp < leastRecentTime)
		{
			leastRecentTime = m_checkedHidingSpot[i].timestamp;
			leastRecent = i;
		}
	}

	// if there is room for more spots, append this one
	if (m_checkedHidingSpotCount < MAX_CHECKED_SPOTS)
	{
		m_checkedHidingSpot[ m_checkedHidingSpotCount ].spot = spot;
		m_checkedHidingSpot[ m_checkedHidingSpotCount ].timestamp = gpGlobals->time;
		++m_checkedHidingSpotCount;
	}
	else
	{
		// replace the least recent spot
		m_checkedHidingSpot[ leastRecent ].spot = spot;
		m_checkedHidingSpot[ leastRecent ].timestamp = gpGlobals->time;
	}
}

// Periodic check of hostage count in case we lost some
//void CCSBot::UpdateHostageEscortCount()
//{
//	const float updateInterval = 1.0f;
//	if (m_hostageEscortCount == 0 || gpGlobals->time - m_hostageEscortCountTimestamp < updateInterval)
//		return;
//
//	m_hostageEscortCountTimestamp = gpGlobals->time;
//
//	// recount the hostages in case we lost some
//	m_hostageEscortCount = 0;
//
//	CHostage *hostage = NULL;
//	while ((hostage = static_cast<CHostage *>(UTIL_FindEntityByClassname(hostage, "hostage_entity"))))
//	{
//		if (FNullEnt(hostage->edict()))
//			break;
//
//		// skip dead or rescued hostages
//		if (!hostage->IsAlive())
//			continue;
//
//		// check if hostage has targeted us, and is following
//		if (hostage->IsFollowing(this))
//			++m_hostageEscortCount;
//	}
//}

// Return true if we are outnumbered by enemies
bool CCSBot::IsOutnumbered() const
{
	return (GetNearbyFriendCount() < GetNearbyEnemyCount() - 1) ? true : false;
}

// Return number of enemies we are outnumbered by
int CCSBot::OutnumberedCount() const
{
	if (IsOutnumbered())
	{
		return (GetNearbyEnemyCount() - 1) - GetNearbyFriendCount();
	}

	return 0;
}

// Return the closest "important" enemy for the given scenario (bomb carrier, VIP, hostage escorter)
CBasePlayer *CCSBot::GetImportantEnemy(bool checkVisibility) const
{
	CBasePlayer *nearEnemy = NULL;
	float nearDist = 999999999.9f;

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *player = UTIL_PlayerByIndex(i);

		if (player == NULL)
			continue;

		if (FNullEnt(player->pev))
			continue;

		if (FStrEq(STRING(player->pev->netname), ""))
			continue;

		// is it a player?
		if (!player->IsPlayer())
			continue;

		// is it alive?
		if (!player->IsAlive())
			continue;

		// skip friends
		//if (BotRelationship(player) == BOT_TEAMMATE)
		if (player->m_iTeam == m_iTeam)
			continue;

		// is it "important"
		if (!TheCSBots->IsImportantPlayer(player))
			continue;

		// is it closest?
		Vector d = pev->origin - player->pev->origin;

		float distSq = d.x * d.x + d.y * d.y + d.z * d.z;
		if (distSq < nearDist)
		{
			if (checkVisibility && !IsVisible(player, CHECK_FOV))
				continue;

			nearEnemy = player;
			nearDist = distSq;
		}
	}

	return nearEnemy;
}

// Sets our current disposition
void CCSBot::SetDisposition(DispositionType disposition)
{
	m_disposition = disposition;

	if (m_disposition != IGNORE_ENEMIES)
	{
		m_ignoreEnemiesTimer.Invalidate();
	}
}

// Return our current disposition
CCSBot::DispositionType CCSBot::GetDisposition() const
{
	if (!m_ignoreEnemiesTimer.IsElapsed())
		return IGNORE_ENEMIES;

	return m_disposition;
}

// Ignore enemies for a short durationy
void CCSBot::IgnoreEnemies(float duration)
{
	m_ignoreEnemiesTimer.Start(duration);
}

// Increase morale one step
void CCSBot::IncreaseMorale()
{
	if (m_morale < EXCELLENT)
	{
		m_morale = static_cast<MoraleType>(m_morale + 1);
	}
}

// Decrease morale one step
void CCSBot::DecreaseMorale()
{
	if (m_morale > TERRIBLE)
	{
		m_morale = static_cast<MoraleType>(m_morale - 1);
	}
}

// Return true if we are acting like a rogue (not listening to teammates, not doing scenario goals)
// TODO: Account for morale
bool CCSBot::IsRogue() const
{
	if (!TheCSBots->AllowRogues())
		return false;

	// periodically re-evaluate our rogue status
	if (m_rogueTimer.IsElapsed())
	{
		m_rogueTimer.Start(RANDOM_FLOAT(10, 30));

		// our chance of going rogue is inversely proportional to our teamwork attribute
		const float rogueChance = 100.0f * (1.0f - GetProfile()->GetTeamwork());

		m_isRogue = (RANDOM_FLOAT(0, 100) < rogueChance);
	}

	return m_isRogue;
}

// Return true if we are in a hurry
bool CCSBot::IsHurrying() const
{
	if (!m_hurryTimer.IsElapsed())
		return true;

	// if the bomb has been planted, we are in a hurry, CT or T (they could be defusing it!)
	if (TheCSBots->GetScenario() == CCSBotManager::SCENARIO_DEFUSE_BOMB && TheCSBots->IsBombPlanted())
		return true;

	// if we are a T and hostages are being rescued, we are in a hurry
//	if (TheCSBots()->GetScenario() == CCSBotManager::SCENARIO_RESCUE_HOSTAGES
//		&& m_iTeam == TERRORIST
//		&& GetGameState()->AreAllHostagesBeingRescued())
//		return true;

	return false;
}

// Return true if it is the early, "safe", part of the round
bool CCSBot::IsSafe() const
{
	if (TheCSBots->GetElapsedRoundTime() < m_safeTime)
		return true;

	return false;
}

// Return true if it is well past the early, "safe", part of the round
bool CCSBot::IsWellPastSafe() const
{
	if (TheCSBots->GetElapsedRoundTime() > 1.25f * m_safeTime)
		return true;

	return false;
}

// Return true if we were in the safe time last update, but not now
bool CCSBot::IsEndOfSafeTime() const
{
	return m_wasSafe && !IsSafe();
}

// Return the amount of "safe time" we have left
float CCSBot::GetSafeTimeRemaining() const
{
	return m_safeTime - TheCSBots->GetElapsedRoundTime();
}

// Called when enemy seen to adjust safe time for this round
void CCSBot::AdjustSafeTime()
{
	// if we spotted an enemy sooner than we thought possible, adjust our notion of "safe" time
	if (m_safeTime > TheCSBots->GetElapsedRoundTime())
	{
		// since right now is not safe, adjust safe time to be a few seconds ago
		m_safeTime = TheCSBots->GetElapsedRoundTime() - 2.0f;
	}
}

// Return true if we haven't seen an enemy for "a long time"
bool CCSBot::HasNotSeenEnemyForLongTime() const
{
	const float longTime = 30.0f;
	return (GetTimeSinceLastSawEnemy() > longTime);
}

// Pick a random zone and hide near it
bool CCSBot::GuardRandomZone(float range)
{
	const CCSBotManager::Zone *zone = TheCSBots->GetRandomZone();

	if (zone != NULL)
	{
		CNavArea *rescueArea = TheCSBots->GetRandomAreaInZone(zone);
		if (rescueArea != NULL)
		{
			Hide(rescueArea, -1.0f, range);
			return true;
		}
	}

	return false;
}

// Do a breadth-first search to find a good retreat spot.
// Don't pick a spot that a Player is currently occupying.
const Vector *FindNearbyRetreatSpot(CCSBot *me, float maxRange)
{
	CNavArea *area = me->GetLastKnownArea();
	if (area == NULL)
		return NULL;

	// collect spots that enemies cannot see
	CollectRetreatSpotsFunctor collector(me, maxRange);
	SearchSurroundingAreas(area, &me->pev->origin, collector, maxRange);

	if (collector.m_count == 0)
		return NULL;

	// select a hiding spot at random
	int which = RANDOM_LONG(0, collector.m_count - 1);
	return collector.m_spot[ which ];
}

// Return euclidean distance to farthest escorted hostage.
// Return -1 if no hostage is following us.
//float CCSBot::GetRangeToFarthestEscortedHostage() const
//{
//	FarthestHostage away(this);
//
//	g_pHostages->ForEachHostage(away);
//
//	return away.m_farRange;
//}
