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




// Face the entity and "use" it
// NOTE: This state assumes we are standing in range of the entity to be used, with no obstructions.
void UseEntityState::OnEnter(CCSBot *me)
{
	;
}

void UseEntityState::OnUpdate(CCSBot *me)
{
	// in the very rare situation where two or more bots "used" a hostage at the same time,
	// one bot will fail and needs to time out of this state
	const float useTimeout = 5.0f;
	if (me->GetStateTimestamp() - gpGlobals->time > useTimeout)
	{
		me->Idle();
		return;
	}

	// look at the entity
	Vector pos = m_entity->pev->origin + Vector(0, 0, HumanHeight * 0.5f);
	me->SetLookAt("Use entity", &pos, PRIORITY_HIGH);

	// if we are looking at the entity, "use" it and exit
	if (me->IsLookingAtPosition(&pos))
	{
//		if (TheCSBots()->GetScenario() == CCSBotManager::SCENARIO_RESCUE_HOSTAGES
//			&& me->m_iTeam == CT
//			&& me->GetTask() == CCSBot::COLLECT_HOSTAGES)
//		{
//			// we are collecting a hostage, assume we were successful - the update check will correct us if we weren't
//			me->IncreaseHostageEscortCount();
//		}

		me->UseEnvironment();
		me->Idle();
	}
}

void UseEntityState::OnExit(CCSBot *me)
{
	me->ClearLookAt();
	me->ResetStuckMonitor();
}
