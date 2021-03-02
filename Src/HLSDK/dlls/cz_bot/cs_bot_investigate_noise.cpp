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


// Move towards currently heard noise
void InvestigateNoiseState::AttendCurrentNoise(CCSBot *me)
{
	if (!me->IsNoiseHeard() && me->GetNoisePosition())
		return;

	// remember where the noise we heard was
	m_checkNoisePosition = *me->GetNoisePosition();

	// tell our teammates (unless the noise is obvious, like gunfire)
//	if (me->IsWellPastSafe() && me->HasNotSeenEnemyForLongTime() && me->GetNoisePriority() != PRIORITY_HIGH)
//		me->GetChatter()->HeardNoise(me->GetNoisePosition());

	// figure out how to get to the noise
	me->PrintIfWatched("Attending to noise...\n");
	me->ComputePath(me->GetNoiseArea(), &m_checkNoisePosition, SAFEST_ROUTE);

	// consume the noise
	me->ForgetNoise();
}

void InvestigateNoiseState::OnEnter(CCSBot *me)
{
	AttendCurrentNoise(me);
}

// Use TravelDistance instead of distance...
void InvestigateNoiseState::OnUpdate(CCSBot *me)
{
	float newNoiseDist;
	if (me->ShouldInvestigateNoise(&newNoiseDist))
	{
		Vector toOldNoise = m_checkNoisePosition - me->pev->origin;
		const float muchCloserDist = 100.0f;
		if (toOldNoise.IsLengthGreaterThan(newNoiseDist + muchCloserDist))
		{
			// new sound is closer
			AttendCurrentNoise(me);
		}
	}

	// if the pathfind fails, give up
	if (!me->HasPath())
	{
		me->Idle();
		return;
	}

	// look around
	me->UpdateLookAround();

	// get distance remaining on our path until we reach the source of the noise
	float noiseDist = (m_checkNoisePosition - me->pev->origin).Length();

	if (me->IsUsingKnife())
	{
		if (me->IsHurrying())
			me->Run();
		else
			me->Walk();
	}
	else
	{
		const float closeToNoiseRange = 1500.0f;
		if (noiseDist < closeToNoiseRange)
		{
			// if we dont have many friends left, or we are alone, and we are near noise source, sneak quietly
			if (me->GetFriendsRemaining() <= 2 && !me->IsHurrying())
			{
				me->Walk();
			}
			else
			{
				me->Run();
			}
		}
		else
		{
			me->Run();
		}
	}

	// if we can see the noise position and we're close enough to it and looking at it,
	// we don't need to actually move there (it's checked enough)
	const float closeRange = 200.0f;
	if (noiseDist < closeRange)
	{
		if (me->IsLookingAtPosition(&m_checkNoisePosition) && me->IsVisible(&m_checkNoisePosition))
		{
			// can see noise position
			me->PrintIfWatched("Noise location is clear.\n");
			//me->ForgetNoise();
			me->Idle();
			return;
		}
	}

	// move towards noise
	if (me->UpdatePathMovement() != CCSBot::PROGRESSING)
	{
		me->Idle();
	}
}

void InvestigateNoiseState::OnExit(CCSBot *me)
{
	// reset to run mode in case we were sneaking about
	me->Run();
}
