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


// Move to a potentially far away position.
void MoveToState::OnEnter(CCSBot *me)
{
	if (me->IsUsingKnife() && me->IsWellPastSafe() && !me->IsHurrying())
	{
		me->Walk();
	}
	else
	{
		me->Run();
	}

	// if we need to find the bomb, get there as quick as we can
	RouteType route;
	switch (me->GetTask())
	{
		case CCSBot::FIND_TICKING_BOMB:
		case CCSBot::DEFUSE_BOMB:
		case CCSBot::MOVE_TO_LAST_KNOWN_ENEMY_POSITION:
			route = FASTEST_ROUTE;
			break;

		default:
			route = SAFEST_ROUTE;
			break;
	}

	// build path to, or nearly to, goal position
	me->ComputePath(TheNavAreaGrid.GetNavArea(&m_goalPosition), &m_goalPosition, route);

	m_radioedPlan = false;
	m_askedForCover = false;
}

// Move to a potentially far away position.
void MoveToState::OnUpdate(CCSBot *me)
{
	// assume that we are paying attention and close enough to know our enemy died
	if (me->GetTask() == CCSBot::MOVE_TO_LAST_KNOWN_ENEMY_POSITION)
	{
		// TODO: Account for reaction time so we take some time to realized the enemy is dead
		CBasePlayer *victim = static_cast<CBasePlayer *>(me->GetTaskEntity());
		if (victim == NULL || !victim->IsAlive())
		{
			me->PrintIfWatched("The enemy I was chasing was killed - giving up.\n");
			me->Idle();
			return;
		}
	}

	// look around
	me->UpdateLookAround();

	// Scenario logic
//	switch (TheCSBots()->GetScenario())
//	{
//		case CCSBotManager::SCENARIO_DEFUSE_BOMB:
//		{
//			// if the bomb has been planted, find it
//			// NOTE: This task is used by both CT and T's to find the bomb
//			if (me->GetTask() == CCSBot::FIND_TICKING_BOMB)
//			{
//				if (!me->GetGameState()->IsBombPlanted())
//				{
//					// the bomb is not planted - give up this task
//					me->Idle();
//					return;
//				}
//
//				if (me->GetGameState()->GetPlantedBombsite() != CSGameState::UNKNOWN)
//				{
//					// we know where the bomb is planted, stop searching
//					me->Idle();
//					return;
//				}
//
//				// check off bombsites that we explore or happen to stumble into
//				for (int z = 0; z < TheCSBots()->GetZoneCount(); ++z)
//				{
//					// don't re-check zones
//					if (me->GetGameState()->IsBombsiteClear(z))
//						continue;
//
//					if (TheCSBots()->GetZone(z)->m_extent.Contains(&me->pev->origin))
//					{
//						// note this bombsite is clear
//						me->GetGameState()->ClearBombsite(z);
//
//						if (me->m_iTeam == CT)
//						{
//							// tell teammates this bombsite is clear
//							me->GetChatter()->BombsiteClear(z);
//						}
//
//						// find another zone to check
//						me->Idle();
//						return;
//					}
//				}
//
//				// move to a bombsite
//				break;
//			}
//			else if (me->m_iTeam == CT)
//			{
//				if (me->GetGameState()->IsBombPlanted())
//				{
//					switch (me->GetTask())
//					{
//						case CCSBot::DEFUSE_BOMB:
//						{
//							// if we are trying to defuse the bomb, and someone has started defusing, guard them instead
//							if (me->CanSeePlantedBomb() && TheCSBots()->GetBombDefuser())
//							{
//								me->GetChatter()->Say("CoveringFriend");
//								me->Idle();
//								return;
//							}
//
//							break;
//						}
//						default:
//						{
//							// we need to find the bomb
//							me->Idle();
//							return;
//						}
//					}
//				}
//			}
//			// TERRORIST
//			else
//			{
//				if (me->GetTask() == CCSBot::PLANT_BOMB)
//				{
//					if (me->GetFriendsRemaining())
//					{
//						// if we are about to plant, radio for cover
//						if (!m_askedForCover)
//						{
//							const float nearPlantSite = 50.0f;
//							if (me->IsAtBombsite() && me->GetPathDistanceRemaining() < nearPlantSite)
//							{
//								// radio to the team
//								me->GetChatter()->PlantingTheBomb(me->GetPlace());
//								m_askedForCover = true;
//							}
//
//							// after we have started to move to the bombsite, tell team we're going to plant, and where
//							// don't do this if we have already radioed that we are starting to plant
//							if (!m_radioedPlan)
//							{
//								const float radioTime = 2.0f;
//								if (gpGlobals->time - me->GetStateTimestamp() > radioTime)
//								{
//									me->GetChatter()->GoingToPlantTheBomb(TheNavAreaGrid.GetPlace(&m_goalPosition));
//									m_radioedPlan = true;
//								}
//							}
//						}
//					}
//				}
//			}
//			break;
//		}
//		case CCSBotManager::SCENARIO_RESCUE_HOSTAGES:
//		{
//			if (me->GetTask() == CCSBot::COLLECT_HOSTAGES)
//			{
//				// Since CT's have a radar, they can directly look at the actual hostage state
//				// check if someone else collected our hostage, or the hostage died or was rescued
//				CHostage *hostage = static_cast<CHostage *>(me->GetGoalEntity());
//				if (!hostage || !hostage->IsAlive() || hostage->IsFollowingSomeone())
//				{
//					me->Idle();
//					return;
//				}
//
//				// if our hostage has moved, repath
//				const float repathToleranceSq = 75.0f * 75.0f;
//				float error = (hostage->pev->origin - m_goalPosition).LengthSquared();
//				if (error > repathToleranceSq)
//				{
//					m_goalPosition = hostage->pev->origin;
//					me->ComputePath(TheNavAreaGrid.GetNavArea(&m_goalPosition), &m_goalPosition, SAFEST_ROUTE);
//				}
//
//				// TODO: Generalize ladder priorities over other tasks
//				if (!me->IsUsingLadder())
//				{
//					Vector pos = hostage->pev->origin + Vector(0, 0, HumanHeight * 0.75f);
//					Vector to = pos - me->pev->origin;
//
//					// look at the hostage as we approach
//					const float watchHostageRange = 100.0f;
//					if (to.IsLengthLessThan(watchHostageRange))
//					{
//						me->SetLookAt("Hostage", &pos, PRIORITY_LOW, 0.5f);
//
//						// randomly move just a bit to avoid infinite use loops from bad hostage placement
//						NavRelativeDirType dir = (NavRelativeDirType)RANDOM_LONG(0, 3);
//						switch (dir)
//						{
//							case LEFT:	me->StrafeLeft(); break;
//							case RIGHT:	me->StrafeRight(); break;
//							case FORWARD:	me->MoveForward(); break;
//							case BACKWARD:	me->MoveBackward(); break;
//						}
//
//						// check if we are close enough to the hostage to talk to him
//						const float useRange = PLAYER_USE_RADIUS - 14.0f; // shave off a fudge factor to make sure we're within range
//						if (to.IsLengthLessThan(useRange))
//						{
//							me->UseEntity(me->GetGoalEntity());
//							return;
//						}
//					}
//				}
//			}
//			else if (me->GetTask() == CCSBot::RESCUE_HOSTAGES)
//			{
//				// periodically check if we lost all our hostages
//				if (me->GetHostageEscortCount() == 0)
//				{
//					// lost our hostages - go get 'em
//					me->Idle();
//					return;
//				}
//			}
//			break;
//		}
//	}

	if (me->UpdatePathMovement() != CCSBot::PROGRESSING)
	{
		// reached destination
		switch (me->GetTask())
		{
			case CCSBot::PLANT_BOMB:
			{
				// if we are at bombsite with the bomb, plant it
				if (me->IsAtBombsite() && me->IsCarryingBomb())
				{
					me->PlantBomb();
					return;
				}
				break;
			}
			case CCSBot::DEFUSE_BOMB:
			{
				if (!me->IsActiveWeaponReloading())
				{
					// if we are near the bomb, defuse it (if we are reloading, don't try to defuse until we finish)
					const Vector *bombPos = me->GetGameState()->GetBombPosition();
					if (bombPos != NULL)
					{
						const float defuseRange = 100.0f;
						Vector toBomb = *bombPos - me->pev->origin;
						toBomb.z = bombPos->z - me->GetFeetZ();

						if (toBomb.IsLengthLessThan(defuseRange))
						{
							me->DefuseBomb();
							return;
						}
					}
				}
				break;
			}
			case CCSBot::MOVE_TO_LAST_KNOWN_ENEMY_POSITION:
			{
				CBasePlayer *victim = static_cast<CBasePlayer *>(me->GetTaskEntity());
				if (victim != NULL && victim->IsAlive())
				{
					// if we got here and haven't re-acquired the enemy, we lost him
//					me->GetChatter()->Say("LostEnemy");
				}
				break;
			}
		}

		// default behavior when destination is reached
		me->Idle();
		return;
	}
}

void MoveToState::OnExit(CCSBot *me)
{
	// reset to run in case we were walking near our goal position
	me->Run();
	me->SetDisposition(CCSBot::ENGAGE_AND_INVESTIGATE);
	//me->StopAiming();
}
