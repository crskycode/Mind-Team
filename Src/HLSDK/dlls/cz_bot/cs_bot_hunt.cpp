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
#include "gamerule.h"

#include "cs_bot.h"



// Begin the hunt
void HuntState::OnEnter(CCSBot *me)
{
	// lurking death
	if (me->IsUsingKnife() && me->IsWellPastSafe() && !me->IsHurrying())
		me->Walk();
	else
		me->Run();

	me->StandUp();
	me->SetDisposition(CCSBot::ENGAGE_AND_INVESTIGATE);
	me->SetTask(CCSBot::SEEK_AND_DESTROY);
	me->DestroyPath();
}

// Hunt down our enemies
void HuntState::OnUpdate(CCSBot *me)
{
	// if we've been hunting for a long time, drop into Idle for a moment to
	// select something else to do
	const float huntingTooLongTime = 30.0f;
	if (gpGlobals->time - me->GetStateTimestamp() > huntingTooLongTime)
	{
		// stop being a rogue and do the scenario, since there must not be many enemies left to hunt
		me->PrintIfWatched("Giving up hunting, and being a rogue\n");
		me->SetRogue(false);
		me->Idle();
		return;
	}

	// scenario logic
//	if (TheCSBots()->GetScenario() == CCSBotManager::SCENARIO_DEFUSE_BOMB)
//	{
//		if (me->m_iTeam == TERRORIST)
//		{
//			// if we have the bomb and it's time to plant, or we happen to be in a bombsite and it seems safe, do it
//			if (me->IsCarryingBomb())
//			{
//				const float safeTime = 3.0f;
//				if (TheCSBots()->IsTimeToPlantBomb() || (me->IsAtBombsite() && gpGlobals->time - me->GetLastSawEnemyTimestamp() > safeTime))
//				{
//					me->Idle();
//					return;
//				}
//			}
//
//			// if we notice the bomb lying on the ground, go get it
//			if (me->NoticeLooseBomb())
//			{
//				me->FetchBomb();
//				return;
//			}
//
//			// if bomb has been planted, and we hear it, move to a hiding spot near the bomb and watch it
//			const Vector *bombPos = me->GetGameState()->GetBombPosition();
//			if (!me->IsRogue() && me->GetGameState()->IsBombPlanted() && bombPos != NULL)
//			{
//				me->SetTask(CCSBot::GUARD_TICKING_BOMB);
//				me->Hide(TheNavAreaGrid.GetNavArea(bombPos));
//				return;
//			}
//		}
//		// CT
//		else
//		{
//			if (!me->IsRogue() && me->CanSeeLooseBomb())
//			{
//				// if we are near the loose bomb and can see it, hide nearby and guard it
//				me->SetTask(CCSBot::GUARD_LOOSE_BOMB);
//				me->Hide(TheCSBots()->GetLooseBombArea());
//				me->GetChatter()->AnnouncePlan("GoingToGuardLooseBomb", TheCSBots()->GetLooseBombArea()->GetPlace());
//				return;
//			}
//			else if (TheCSBots()->IsBombPlanted())
//			{
//				// rogues will defuse a bomb, but not guard the defuser
//				if (!me->IsRogue() || !TheCSBots()->GetBombDefuser())
//				{
//					// search for the planted bomb to defuse
//					me->Idle();
//					return;
//				}
//			}
//		}
//	}
//	else if (TheCSBots()->GetScenario() == CCSBotManager::SCENARIO_RESCUE_HOSTAGES)
//	{
//		if (me->m_iTeam == TERRORIST)
//		{
//			if (me->GetGameState()->AreAllHostagesBeingRescued())
//			{
//				// all hostages are being rescued, head them off at the escape zones
//				if (me->GuardRandomZone())
//				{
//					me->SetTask(CCSBot::GUARD_HOSTAGE_RESCUE_ZONE);
//					me->PrintIfWatched("Trying to beat them to an escape zone!\n");
//					me->SetDisposition(CCSBot::OPPORTUNITY_FIRE);
//					me->GetChatter()->GuardingHostageEscapeZone(IS_PLAN);
//					return;
//				}
//			}
//
//			// if safe time is up, and we stumble across a hostage, guard it
//			if (!me->IsRogue() && !me->IsSafe())
//			{
//				CHostage *hostage = me->GetGameState()->GetNearestVisibleFreeHostage();
//				if (hostage != NULL)
//				{
//					CNavArea *area = TheNavAreaGrid.GetNearestNavArea(&hostage->pev->origin);
//					if (area != NULL)
//					{
//						// we see a free hostage, guard it
//						me->SetTask(CCSBot::GUARD_HOSTAGES);
//						me->Hide(area);
//						me->PrintIfWatched("I'm guarding hostages\n");
//						me->GetChatter()->GuardingHostages(area->GetPlace(), IS_PLAN);
//						return;
//					}
//				}
//			}
//		}
//	}

	// listen for enemy noises
	if (me->ShouldInvestigateNoise())
	{
		me->InvestigateNoise();
		return;
	}

	// look around
	me->UpdateLookAround();

	// if we have reached our destination area, pick a new one
	// if our path fails, pick a new one
	if (me->GetLastKnownArea() == m_huntArea || me->UpdatePathMovement() != CCSBot::PROGRESSING)
	{
		m_huntArea = NULL;
		float oldest = 0.0f;

		int areaCount = 0;
		const float minSize = 150.0f;

		NavAreaList::iterator iter;

		for (iter = TheNavAreaList.begin(); iter != TheNavAreaList.end(); ++iter)
		{
			CNavArea *area = (*iter);

			++areaCount;

			// skip the small areas
			const Extent *extent = area->GetExtent();
			if (extent->hi.x - extent->lo.x < minSize || extent->hi.y - extent->lo.y < minSize)
				continue;

			// keep track of the least recently cleared area
			float_precision age = gpGlobals->time - area->GetClearedTimestamp(me->m_iTeam - 1);
			if (age > oldest)
			{
				oldest = age;
				m_huntArea = area;
			}
		}

		// if all the areas were too small, pick one at random
		int which = RANDOM_LONG(0, areaCount - 1);

		areaCount = 0;
		for (iter = TheNavAreaList.begin(); iter != TheNavAreaList.end(); ++iter)
		{
			m_huntArea = (*iter);

			if (which == areaCount)
				break;

			--which;
		}

		if (m_huntArea != NULL)
		{
			// create a new path to a far away area of the map
			me->ComputePath(m_huntArea, NULL, SAFEST_ROUTE);
		}
	}
}

// Done hunting
void HuntState::OnExit(CCSBot *me)
{
	;
}
