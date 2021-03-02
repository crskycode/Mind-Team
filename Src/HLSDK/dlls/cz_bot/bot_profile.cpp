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

#include "bot_profile.h"


BotProfileManager *TheBotProfiles = NULL;

BotProfile::BotProfile()
{
	m_iTeamType = BOT_TEAM_ANY;
	strcpy(m_szProfileName, "111");
	strcpy(m_szBotName, "222");
	m_flAggression = 0.0f;
	m_flSkill = 0.0f;
	m_flTeamwork = 0.0f;
	m_flReactionTime = 0.3f;
	m_flAttackDelay = 0.3f;
}

BotProfileTeamType BotProfile::GetTeamType(void)  const
{
	return m_iTeamType;
}

const char *BotProfile::GetBotName(void)  const
{
	return m_szBotName;
}

float BotProfile::GetAggression(void)  const
{
	return m_flAggression;
}

float BotProfile::GetSkill(void)  const
{
	return m_flSkill;
}

float BotProfile::GetTeamwork(void)  const
{
	return m_flTeamwork;
}

float BotProfile::GetReactionTime(void)  const
{
	return m_flReactionTime;
}

float BotProfile::GetAttackDelay(void)  const
{
	return m_flAttackDelay;
}


BotProfileManager::BotProfileManager()
{
}
void BotProfileManager::Init(const char *filename)
{
}